/*
 * Audio.cpp
 *
 *  Created on: 28 Mar 2020
 *      Author: adam
 */

// The goal of this module is to set up streaming - that is simultaneous input and
// output of audio. I also need to use DMA for the best speed.

//#include "../Components/wm8994/wm8994.h"
#include "Audio.h"
#include "stm32746g_discovery_audio.h"
#include <string.h>

enum {
	kSampleRate = I2S_AUDIOFREQ_22K,
	kSampleBits = 16,
	kSampleChannels = 2
};

// DMA buffers to send and receive audio.
// TODO: Might be faster if I put it in DTCM RAM.
#define AUDIO_BLOCK_SIZE 512
SAMPLE AUDIO_BUFFER_IN[AUDIO_BLOCK_SIZE];
SAMPLE AUDIO_BUFFER_OUT[AUDIO_BLOCK_SIZE];

Audio *Audio::instance()
{
	static Audio *g_audio = 0;

	if(g_audio == 0)
		g_audio = new Audio();

	return g_audio;
}

Audio::Audio()
{
}

// Set up I2S and DMA to shovel audio in and out simultaneously.
Audio::STATUS Audio::init()
{
	if(AUDIO_OK != BSP_AUDIO_IN_OUT_Init(INPUT_DEVICE_DIGITAL_MICROPHONE_2, OUTPUT_DEVICE_HEADPHONE, kSampleRate, kSampleBits, kSampleChannels))
		return kStatusFail;

	return kStatusOk;
}

// Start shoveling audio.
Audio::STATUS Audio::start()
{
	  /* Initialize SDRAM buffers */
	  memset((uint16_t*)AUDIO_BUFFER_IN, 0, AUDIO_BLOCK_SIZE*2);
	  memset((uint16_t*)AUDIO_BUFFER_OUT, 0, AUDIO_BLOCK_SIZE*2);

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

// Fill up one outgoing buffer. Pull the data from the audio processing chain.
SAMPLE *currentAudioBuffer = 0;
void Audio::pullBuffer(SAMPLE *dest)
{
	memcpy(dest, currentAudioBuffer, AUDIO_BLOCK_SIZE);
}

//// Get a copy of the most recent audio buffer.
//unsigned Audio::getData(SAMPLE **data)
//{
//	*data = AUDIO_BUFFER_OUT;
//	return AUDIO_BLOCK_SIZE;
//}


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
	Audio::instance()->pullBuffer(&AUDIO_BUFFER_OUT[AUDIO_BLOCK_SIZE / 2]);
}

/* This function is called when half of the requested buffer has been transferred. */
void    BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	// Output buffer needs filling. Get some data for it.
	Audio::instance()->pullBuffer(&AUDIO_BUFFER_OUT[0]);
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

#ifdef OLD

#include "Audio.h"
#include "stm32746g_discovery_audio.h"
#include <string.h>

extern "C" {


	typedef enum {
	  BUFFER_EMPTY = 0,
	  BUFFER_FULL,
	}WR_BUFFER_StateTypeDef;

	typedef enum {
	  AUDIO_STATE_IDLE = 0,
	  AUDIO_STATE_WAIT,
	  AUDIO_STATE_INIT,
	  AUDIO_STATE_PLAY,
	  AUDIO_STATE_PRERECORD,
	  AUDIO_STATE_RECORD,
	  AUDIO_STATE_NEXT,
	  AUDIO_STATE_PREVIOUS,
	  AUDIO_STATE_FORWARD,
	  AUDIO_STATE_BACKWARD,
	  AUDIO_STATE_STOP,
	  AUDIO_STATE_PAUSE,
	  AUDIO_STATE_RESUME,
	  AUDIO_STATE_VOLUME_UP,
	  AUDIO_STATE_VOLUME_DOWN,
	  AUDIO_STATE_ERROR,
	}AUDIO_PLAYBACK_StateTypeDef;

	typedef enum {
	  BUFFER_OFFSET_NONE = 0,
	  BUFFER_OFFSET_HALF,
	  BUFFER_OFFSET_FULL,
	}BUFFER_StateTypeDef;

	typedef struct {
	  uint16_t pcm_buff[AUDIO_IN_PCM_BUFFER_SIZE];
	  uint32_t pcm_ptr;
	  WR_BUFFER_StateTypeDef wr_state;
	  uint32_t offset;
	  uint32_t fptr;
	}AUDIO_IN_BufferTypeDef;

	/* Audio buffer control struct */
	typedef struct {
	  uint8_t buff[AUDIO_OUT_BUFFER_SIZE];
	  BUFFER_StateTypeDef state;
	  uint32_t fptr;
	}AUDIO_OUT_BufferTypeDef;

	typedef enum {
	  AUDIO_ERROR_NONE = 0,
	  AUDIO_ERROR_IO,
	  AUDIO_ERROR_EOF,
	  AUDIO_ERROR_INVALID_VALUE,
	}AUDIO_ErrorTypeDef;

	AUDIO_IN_BufferTypeDef   BufferInCtl;
	AUDIO_OUT_BufferTypeDef  BufferOutCtl;
	__IO uint32_t uwVolume = 70;
	AUDIO_PLAYBACK_StateTypeDef AudioState;

	void audioInit()
	{


	}

	void audioRecord()
	{
		uint32_t byteswritten = 0;

		BSP_AUDIO_IN_Init(DEFAULT_AUDIO_IN_FREQ, DEFAULT_AUDIO_IN_BIT_RESOLUTION, DEFAULT_AUDIO_IN_CHANNEL_NBR);
		BSP_AUDIO_IN_Record((uint16_t*)&BufferInCtl.pcm_buff[0], AUDIO_IN_PCM_BUFFER_SIZE);
		BufferInCtl.fptr = byteswritten;
		BufferInCtl.pcm_ptr = 0;
		BufferInCtl.offset = 0;
		BufferInCtl.wr_state = BUFFER_EMPTY;
	}

	/**
	  * @brief  Calculates the remaining file size and new position of the pointer.
	  * @param  None
	  * @retval None
	  */
	void BSP_AUDIO_IN_TransferComplete_CallBack(void)
	{
		BufferInCtl.pcm_ptr+= AUDIO_IN_PCM_BUFFER_SIZE/2;
	  if(BufferInCtl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE/2)
	  {
		  BufferInCtl.wr_state   =  BUFFER_FULL;
		  BufferInCtl.offset  = 0;
	  }

	  if(BufferInCtl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
	  {
		  BufferInCtl.wr_state   =  BUFFER_FULL;
		  BufferInCtl.offset  = AUDIO_IN_PCM_BUFFER_SIZE/2;
		  BufferInCtl.pcm_ptr = 0;
	  }
	}

	/**
	  * @brief  Manages the DMA Half Transfer complete interrupt.
	  * @param  None
	  * @retval None
	  */
	void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
	{
		BufferInCtl.pcm_ptr+= AUDIO_IN_PCM_BUFFER_SIZE/2;
	  if(BufferInCtl.pcm_ptr == AUDIO_IN_PCM_BUFFER_SIZE/2)
	  {
		  BufferInCtl.wr_state   =  BUFFER_FULL;
		  BufferInCtl.offset  = 0;
	  }

	  if(BufferInCtl.pcm_ptr >= AUDIO_IN_PCM_BUFFER_SIZE)
	  {
		  BufferInCtl.wr_state   =  BUFFER_FULL;
		  BufferInCtl.offset  = AUDIO_IN_PCM_BUFFER_SIZE/2;
		  BufferInCtl.pcm_ptr = 0;
	  }
	}

	/**
	  * @brief  Calculates the remaining file size and new position of the pointer.
	  * @param  None
	  * @retval None
	  */
	void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
	{
	  if(AudioState == AUDIO_STATE_PLAY)
	  {
	    BufferOutCtl.state = BUFFER_OFFSET_FULL;
	  }
	}

	/**
	  * @brief  Manages the DMA Half Transfer complete interrupt.
	  * @param  None
	  * @retval None
	  */
	void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
	{
	  if(AudioState == AUDIO_STATE_PLAY)
	  {
	    BufferOutCtl.state = BUFFER_OFFSET_HALF;
	  }
	}


	AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void)
		{
		  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, uwVolume, I2S_AUDIOFREQ_48K) == 0)
		  {
		    return AUDIO_ERROR_NONE;
		  }
		  else
		  {
		    return AUDIO_ERROR_IO;
		  }
	}

	static uint8_t PlayerInit(uint32_t AudioFreq)
	{
	  /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */
	  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0)
	  {
	    return 1;
	  }
	  else
	  {
	    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
	    return 0;
	  }
	}

	AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx)
	{
	  uint32_t bytesread = 0;

//	  f_close(&WavFile);
//	  if(AUDIO_GetWavObjectNumber() > idx)
//	  {
//	    GetFileInfo(idx, &WaveFormat);

	    /*Adjust the Audio frequency */
	    PlayerInit(16000 /*WaveFormat.SampleRate*/);

	    BufferOutCtl.state = BUFFER_OFFSET_NONE;

//	    /* Get Data from USB Flash Disk */
//	    f_lseek(&WavFile, 0);

	    /* Fill whole buffer at first time */
	    memset(&BufferOutCtl.buff[0], 0, sizeof(BufferOutCtl.buff));
//	    if(f_read(&WavFile,
//	              &BufferCtl.buff[0],
//	              AUDIO_OUT_BUFFER_SIZE,
//	              (void *)&bytesread) == FR_OK)
//	    {
	      AudioState = AUDIO_STATE_PLAY;
//	      AUDIO_PlaybackDisplayButtons();
//	      BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PLAY ]", LEFT_MODE);
//	      {
//	        if(bytesread != 0)
//	        {
	          BSP_AUDIO_OUT_Play((uint16_t*)&BufferOutCtl.buff[0], AUDIO_OUT_BUFFER_SIZE);
	          BufferOutCtl.fptr = bytesread;
	          return AUDIO_ERROR_NONE;
//	        }
//	      }
//	    }
//	  }
	  return AUDIO_ERROR_IO;
	}
} // extern "C"



Audio::Audio()
{

}

void Audio::playback()
{
	AUDIO_PLAYER_Init();
	AUDIO_PLAYER_Start(0);
}

void Audio::record()
{
	audioRecord();
}

unsigned Audio::getBuffer(int16_t **buf)
{
    if(BufferInCtl.wr_state == BUFFER_FULL)
    {
    	uint8_t * data = (uint8_t*)(BufferInCtl.pcm_buff + BufferInCtl.offset);
    	*buf = (int16_t *)data;

      BufferInCtl.fptr += AUDIO_IN_PCM_BUFFER_SIZE;
      BufferInCtl.wr_state =  BUFFER_EMPTY;
  	return AUDIO_IN_PCM_BUFFER_SIZE / sizeof(int16_t);
    }
    return 0;
}

void Audio::fillBuffer(const int16_t *audioData, unsigned nSamples)
{

	if(BufferOutCtl.state == BUFFER_OFFSET_FULL) {
		if(nSamples > AUDIO_OUT_BUFFER_SIZE)
			nSamples = AUDIO_OUT_BUFFER_SIZE;
		memcpy((uint8_t *)&BufferOutCtl.buff[0], (const uint8_t *)audioData, nSamples * sizeof(uint16_t));
	}
}
#endif // OLD
