#include "stm32f7xx_it.h"
#include "PerfMon.h"

extern HCD_HandleTypeDef hhcd;
extern SD_HandleTypeDef uSdHandle;

#ifdef OLD

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

extern PCD_HandleTypeDef hpcd;

/* SAI handler declared in "stm32746g_discovery_audio.c" file */
extern SAI_HandleTypeDef haudio_out_sai;
/* I2S handler declared in "stm32746g_discovery_audio.c" file */
extern SAI_HandleTypeDef haudio_in_sai;

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





/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}
#endif // OLD


// Called frequently by SysTick interrupt.
void SysTick_Handler(void)
{
  HAL_IncTick();
}

#ifdef OLD
/******************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f7xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */
#endif // OLD

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
	perfEnter(pidUsb);
	HAL_HCD_IRQHandler(&hhcd);
	//HAL_PCD_IRQHandler(&hpcd);
	perfLeave();
}

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

#ifdef OLD
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
#endif // OLD
