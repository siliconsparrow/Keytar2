#include "stm32f7xx_it.h"
#include "PerfMon.h"

extern HCD_HandleTypeDef hhcd;
extern SD_HandleTypeDef  uSdHandle;
extern PCD_HandleTypeDef hpcd;
extern SAI_HandleTypeDef haudio_out_sai; // SAI handler declared in "stm32746g_discovery_audio.c"
extern SAI_HandleTypeDef haudio_in_sai; // I2S handler declared in "stm32746g_discovery_audio.c"

#ifdef OLD

///* Private variables ---------------------------------------------------------*/
//extern DMA_HandleTypeDef   hdma;
///*DMA status declared in "sdram_dma.c" file */
//extern uint32_t uwDMA_Transfer_Complete;
///* SAI handler declared in "stm32746g_discovery_audio.c" file */
//extern SAI_HandleTypeDef haudio_out_sai;
///* SAI handler declared in "stm32746g_discovery_audio.c" file */
//extern SAI_HandleTypeDef haudio_in_sai;
///* SDRAM handler declared in "stm32746g_discovery_sdram.c" file */
//extern SDRAM_HandleTypeDef sdramHandle;

#endif // OLD

/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/

// Called frequently by SysTick interrupt.
void SysTick_Handler(void)
{
  HAL_IncTick();
}

#ifdef OLD

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

#endif // OLD

// Handler for the USB host peripheral.
#if defined(USE_USBH_FS)
	void OTG_FS_IRQHandler(void)
	{
		perfEnter(pidUsbH);
		HAL_HCD_IRQHandler(&hhcd);
		perfLeave();
	}
#elif defined(USE_USBH_HS)
	void OTG_HS_IRQHandler(void)
	{
		perfEnter(pidUsbH);
		HAL_HCD_IRQHandler(&hhcd);
		perfLeave();
	}
#endif

// Handler for the USB device peripheral.
#if defined(USE_USBD_FS)
	void OTG_FS_IRQHandler(void)
	{
		perfEnter(pidUsbD);
		HAL_PCD_IRQHandler(&hpcd);
		perfLeave();
	}
#elif defined(USE_USBH_HS)
	void OTG_HS_IRQHandler(void)
	{
		perfEnter(pidUsbD);
		HAL_PCD_IRQHandler(&hpcd);
		perfLeave();
	}
#endif


/**
  * @brief  This function handles SDMMC1 global interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SDMMC_IRQHandler(void)
{
	perfEnter(pidDisk);
	HAL_SD_IRQHandler(&uSdHandle);
	perfLeave();
}

/**
* @brief  This function handles DMA2 Stream 6 interrupt request.
* @param  None
* @retval None
*/
void BSP_SDMMC_DMA_Tx_IRQHandler(void)
{
	perfEnter(pidDisk);
	HAL_DMA_IRQHandler(uSdHandle.hdmatx);
	perfLeave();
}

/**
* @brief  This function handles DMA2 Stream 3 interrupt request.
* @param  None
* @retval None
*/
void BSP_SDMMC_DMA_Rx_IRQHandler(void)
{
	perfEnter(pidDisk);
	HAL_DMA_IRQHandler(uSdHandle.hdmarx);
	perfLeave();
}

/**
  * @brief This function handles DMA2 Stream 4 interrupt request.
  * @param None
  * @retval None
  */
void AUDIO_OUT_SAIx_DMAx_IRQHandler(void)
{
	perfEnter(pidAudio);
	HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
	perfLeave();
}

/**
  * @brief This function handles DMA2 Stream 7 interrupt request.
  * @param None
  * @retval None
  */
void AUDIO_IN_SAIx_DMAx_IRQHandler(void)
{
	perfEnter(pidAudio);
	HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
	perfLeave();
}
