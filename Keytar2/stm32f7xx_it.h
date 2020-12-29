#ifndef __STM32F7xx_IT_H
#define __STM32F7xx_IT_H

#include "main.h"

#ifdef __cplusplus
 extern "C" {
#endif 

//void NMI_Handler(void);
//void HardFault_Handler(void);
//void MemManage_Handler(void);
//void BusFault_Handler(void);
//void UsageFault_Handler(void);
//void SVC_Handler(void);
//void DebugMon_Handler(void);
//void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void);
#else
void OTG_HS_IRQHandler(void);
#endif
void BSP_SDMMC_IRQHandler(void);
void BSP_SDMMC_DMA_Tx_IRQHandler(void);
void BSP_SDMMC_DMA_Rx_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif
