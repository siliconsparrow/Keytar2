/*
 * Audio.cpp
 *
 *  Created on: 28 Mar 2020
 *      Author: adam
 */

// The goal of this module is to set up streaming - that is simultaneous input and
// output of audio. I also need to use DMA for the best speed.

#include "Audio.h"
#include "stm32746g_discovery_audio.h"
#include <string.h>

//#define USE_PDM_MIC

// DMA buffers to send and receive audio.
// These must be in DTCM RAM so they can be accessed by the DMA.
#define AUDIO_BLOCK_SIZE (kAudioFrameSize * sizeof(StereoSample))
uint16_t  AUDIO_BUFFER_IN[AUDIO_BLOCK_SIZE];
uint16_t  AUDIO_BUFFER_OUT[AUDIO_BLOCK_SIZE];
uint16_t *currentAudioBuffer = 0;

FilterLineIn::FilterLineIn(Channel chan)
	: _chan(chan)
{ }


void FilterLineIn::fillFrame(StereoSample *frame)
{
	StereoSample *src = (StereoSample *)currentAudioBuffer;

	switch(_chan)
	{
	case chanLeft:
		for(int i = 0; i < kAudioFrameSize; i++) {
			frame[i].l = src[i].l;
			frame[i].r = src[i].l;
		}
		break;

	case chanRight:
		for(int i = 0; i < kAudioFrameSize; i++) {
			frame[i].l = src[i].r;
			frame[i].r = src[i].r;
		}
		break;

	default:
		memcpy(frame, currentAudioBuffer, kAudioFrameSize * sizeof(StereoSample));
		break;
	}
}

Audio *Audio::instance()
{
	static Audio *g_audio = 0;

	if(g_audio == 0)
		g_audio = new Audio();

	return g_audio;
}

Audio::Audio()
	: _filterChain(0)
{
}

// Set up I2S and DMA to shovel audio in and out simultaneously.
Audio::STATUS Audio::init()
{
#ifdef USE_PDM_MIC
	if(AUDIO_OK != initCodec(INPUT_DEVICE_DIGITAL_MICROPHONE_2, OUTPUT_DEVICE_HEADPHONE, kSampleRate, kSampleBits, kSampleChannels))
		return kStatusFail;
#else
	if(AUDIO_OK != initCodec(INPUT_DEVICE_INPUT_LINE_1, OUTPUT_DEVICE_HEADPHONE, kSampleRate, kSampleBits, kSampleChannels))
		return kStatusFail;
#endif

	return kStatusOk;
}

// Start shoveling audio.
Audio::STATUS Audio::start()
{
	  /* Initialize SDRAM buffers */
	  memset((uint16_t*)AUDIO_BUFFER_IN, 0, AUDIO_BLOCK_SIZE * sizeof(uint16_t));
	  memset((uint16_t*)AUDIO_BUFFER_OUT, 0, AUDIO_BLOCK_SIZE * sizeof(uint16_t));

	  /* Start Recording */
	  if(AUDIO_OK != BSP_AUDIO_IN_Record((uint16_t*)AUDIO_BUFFER_IN, AUDIO_BLOCK_SIZE))
		  return kStatusFail;

	  /* Start Playback */
	  BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	  if(AUDIO_OK != BSP_AUDIO_OUT_Play((uint16_t*)AUDIO_BUFFER_OUT, AUDIO_BLOCK_SIZE * 2))
		  return kStatusFail;

	  return kStatusOk;
}

// Stop shoveling audio.
Audio::STATUS Audio::stop()
{
    if(AUDIO_OK != BSP_AUDIO_IN_Stop(CODEC_PDWN_SW))
    	return kStatusFail;

    if(AUDIO_OK != BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW))
    	return kStatusFail;

    return kStatusOk;
}

// Set up the audio processing chain. This should only be changed when
// the audio processing is stopped.
void Audio::setFilterChain(AudioFilter *f)
{
	_filterChain = f;
}

// Fill up one outgoing buffer. Pull the data from the audio processing chain.
void Audio::pullBuffer(StereoSample *dest)
{
	_filterChain->fillFrame(dest);
}

// I need a few hooks into the BSP code.
extern "C" {
	extern SAI_HandleTypeDef haudio_in_sai;
	extern SAI_HandleTypeDef haudio_out_sai;
	extern AUDIO_DrvTypeDef  *audio_drv;
	void SAIx_In_DeInit(void);
	void SAIx_Out_DeInit(void);
	void BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params);
	void SAIx_In_Init(uint32_t SaiOutMode, uint32_t SlotActive, uint32_t AudioFreq);
}

// The BSP functions supplied in CUBEMX only support the PDM microphone but I want to use
// the analogue line input so I need a custom init function.
int Audio::initCodec(uint16_t InputDevice, uint16_t OutputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{
    int ret = AUDIO_ERROR;

    // Disable SAI
    SAIx_In_DeInit();
    SAIx_Out_DeInit();

    // PLL clock is set depending on the AudioFreq (44.1khz vs 48khz groups)
    BSP_AUDIO_OUT_ClockConfig(&haudio_in_sai, AudioFreq, NULL); /* Clock config is shared between AUDIO IN and OUT */

    haudio_in_sai.Instance = AUDIO_IN_SAIx;
    if(HAL_SAI_GetState(&haudio_in_sai) == HAL_SAI_STATE_RESET) {
      // Init the SAI MSP: this __weak function can be redefined by the application
      BSP_AUDIO_IN_MspInit(&haudio_in_sai, NULL);
    }

    // SAI data transfer preparation:
    // Prepare the Media to be used for the audio transfer from memory to SAI peripheral
    haudio_out_sai.Instance = AUDIO_OUT_SAIx;
    if(HAL_SAI_GetState(&haudio_out_sai) == HAL_SAI_STATE_RESET) {
      // Init the SAI MSP: this __weak function can be redefined by the application
      BSP_AUDIO_OUT_MspInit(&haudio_out_sai, NULL);
    }

    // Configure SAI in master mode :
    //   - SAI2_block_A in master TX mode
    //   - SAI2_block_B in slave RX mode synchronous from SAI2_block_A
    uint32_t slot_active;
    if (InputDevice == INPUT_DEVICE_DIGITAL_MICROPHONE_2) {
      slot_active = CODEC_AUDIOFRAME_SLOT_13;
    } else {
      slot_active = CODEC_AUDIOFRAME_SLOT_02; //CODEC_AUDIOFRAME_SLOT_0123;
    }
    SAIx_In_Init(SAI_MODEMASTER_TX, slot_active, AudioFreq);

    // wm8994 codec initialization.
    uint32_t deviceid = wm8994_drv.ReadID(AUDIO_I2C_ADDRESS);

    if((deviceid) == WM8994_ID) {
      // Reset the Codec Registers
      wm8994_drv.Reset(AUDIO_I2C_ADDRESS);
      /* Initialize the audio driver structure */
      audio_drv = &wm8994_drv;
      ret = AUDIO_OK;
    }
    else
    {
      ret = AUDIO_ERROR;
    }

    if(ret == AUDIO_OK)
    {
      /* Initialize the codec internal registers */
    	uint8_t volume = 80; // 100;
      audio_drv->Init(AUDIO_I2C_ADDRESS, InputDevice | OutputDevice, volume, AudioFreq);

      audio_drv->SetVolume(AUDIO_I2C_ADDRESS, volume); // Volume ranges from 0 to 100.
    }

    // Raise the priority of the audio interrupts (the default driver sets them quite low).
    HAL_NVIC_SetPriority(AUDIO_IN_SAIx_DMAx_IRQ, INT_PRI_AUDIO_IN, 0);
    HAL_NVIC_SetPriority(AUDIO_IN_INT_IRQ, INT_PRI_AUDIO_IN, 0);
    HAL_NVIC_SetPriority(AUDIO_OUT_SAIx_DMAx_IRQ, INT_PRI_AUDIO_OUT, 0);

    return ret;
}


// Callbacks.
extern "C" {
	/**
	  * @brief Manages the DMA Transfer complete interrupt.
	  * @param None
	  * @retval None
	  */
	void BSP_AUDIO_IN_TransferComplete_CallBack(void)
	{
		// This input buffer is ready for processing.
		currentAudioBuffer = &AUDIO_BUFFER_IN[AUDIO_BLOCK_SIZE / 2];
	}

	/**
	  * @brief  Manages the DMA Half Transfer complete interrupt.
	  * @param  None
	  * @retval None
	  */
	void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
	{
		// This input buffer is ready for processing.
		currentAudioBuffer = &AUDIO_BUFFER_IN[0];
	}

	/* User Callbacks: user has to implement these functions in his code if they are needed. */
	/* This function is called when the requested data has been completely transferred.*/
	void    BSP_AUDIO_OUT_TransferComplete_CallBack(void)
	{
		// Output buffer needs filling. Get some data for it.
		Audio::instance()->pullBuffer((StereoSample *)&AUDIO_BUFFER_OUT[AUDIO_BLOCK_SIZE / 2]);
	}

	/* This function is called when half of the requested buffer has been transferred. */
	void    BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
	{
		// Output buffer needs filling. Get some data for it.
		Audio::instance()->pullBuffer((StereoSample *)&AUDIO_BUFFER_OUT[0]);
	}


	/**
	  * @brief  Audio IN Error callback function.
	  * @param  None
	  * @retval None
	  */
	void BSP_AUDIO_IN_Error_CallBack(void)
	{
	  /* This function is called when an Interrupt due to transfer error on or peripheral
		 error occurs. */
	  /* Display message on the LCD screen */
	  //BSP_LCD_SetBackColor(LCD_COLOR_RED);
	  //BSP_LCD_DisplayStringAt(0, LINE(14), (uint8_t *)"       DMA  ERROR     ", CENTER_MODE);


		while(1)
			; // Halt and catch fire.

	  /* Stop the program with an infinite loop */
	  //while (BSP_PB_GetState(BUTTON_KEY) != RESET)
	  //{
	   // return;
	  //}
	  /* could also generate a system reset to recover from the error */
	  /* .... */
	}

} // extern "C"
