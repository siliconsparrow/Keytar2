/*
 * crash.cpp
 *
 *  Created on: 28 Mar 2020
 *      Author: adam
 */

#include "platform.h"
#include <stdint.h>

void crash(int code)
{
	SCB_Type *scb = SCB;
	uint32_t hfsr = scb->HFSR;

	while(1)
		;
}

extern "C" {
	void  NMI_Handler() { crash(1); }
	void  HardFault_Handler() { crash(2); }
	void  MemManage_Handler() { crash(3); }
	void  BusFault_Handler() { crash(4); }
	void  UsageFault_Handler() { crash(5); }
	void  SVC_Handler() { crash(6); }
	void  DebugMon_Handler() { crash(7); }
	void  PendSV_Handler() { crash(8); }
//	void  SysTick_Handler() { crash(9); }
	void     WWDG_IRQHandler() { crash(10); }
	void     PVD_IRQHandler() { crash(11); }
	void     TAMP_STAMP_IRQHandler() { crash(12); }
	void     RTC_WKUP_IRQHandler() { crash(13); }
	void     FLASH_IRQHandler() { crash(14); }
	void     RCC_IRQHandler() { crash(15); }
	void     EXTI0_IRQHandler() { crash(16); }
	void     EXTI1_IRQHandler() { crash(17); }
	void     EXTI2_IRQHandler() { crash(18); }
	void     EXTI3_IRQHandler() { crash(19); }
	void     EXTI4_IRQHandler() { crash(20); }
	void     DMA1_Stream0_IRQHandler() { crash(21); }
	void     DMA1_Stream1_IRQHandler() { crash(22); }
	void     DMA1_Stream2_IRQHandler() { crash(23); }
	void     DMA1_Stream3_IRQHandler() { crash(24); }
	void     DMA1_Stream4_IRQHandler() { crash(25); }
	void     DMA1_Stream5_IRQHandler() { crash(26); }
	void     DMA1_Stream6_IRQHandler() { crash(27); }
	void     ADC_IRQHandler() { crash(28); }
	void     CAN1_TX_IRQHandler() { crash(29); }
	void     CAN1_RX0_IRQHandler() { crash(30); }
	void     CAN1_RX1_IRQHandler() { crash(31); }
	void     CAN1_SCE_IRQHandler() { crash(32); }
	void     EXTI9_5_IRQHandler() { crash(33); }
	void     TIM1_BRK_TIM9_IRQHandler() { crash(34); }
	void     TIM1_UP_TIM10_IRQHandler() { crash(35); }
	void     TIM1_TRG_COM_TIM11_IRQHandler() { crash(36); }
	void     TIM1_CC_IRQHandler() { crash(37); }
	void     TIM2_IRQHandler() { crash(38); }
	void     TIM3_IRQHandler() { crash(39); }
	void     TIM4_IRQHandler() { crash(40); }
	void     I2C1_EV_IRQHandler() { crash(41); }
	void     I2C1_ER_IRQHandler() { crash(42); }
	void     I2C2_EV_IRQHandler() { crash(43); }
	void     I2C2_ER_IRQHandler() { crash(44); }
	void     SPI1_IRQHandler() { crash(45); }
	void     SPI2_IRQHandler() { crash(46); }
	void     USART1_IRQHandler() { crash(47); }
	void     USART2_IRQHandler() { crash(48); }
	void     USART3_IRQHandler() { crash(49); }
	void     EXTI15_10_IRQHandler() { crash(50); }
	void     RTC_Alarm_IRQHandler() { crash(51); }
	void     OTG_FS_WKUP_IRQHandler() { crash(52); }
	void     TIM8_BRK_TIM12_IRQHandler() { crash(53); }
	void     TIM8_UP_TIM13_IRQHandler() { crash(54); }
	void     TIM8_TRG_COM_TIM14_IRQHandler() { crash(55); }
	void     TIM8_CC_IRQHandler() { crash(56); }
	void     DMA1_Stream7_IRQHandler() { crash(57); }
	void     FMC_IRQHandler() { crash(58); }
//	void     SDMMC1_IRQHandler() { crash(59); }
	void     TIM5_IRQHandler() { crash(60); }
	void     SPI3_IRQHandler() { crash(61); }
	void     UART4_IRQHandler() { crash(62); }
	void     UART5_IRQHandler() { crash(63); }
	void     TIM6_DAC_IRQHandler() { crash(64); }
	void     TIM7_IRQHandler() { crash(65); }
	void     DMA2_Stream0_IRQHandler() { crash(66); }
	void     DMA2_Stream1_IRQHandler() { crash(67); }
	void     DMA2_Stream2_IRQHandler() { crash(68); }
//	void     DMA2_Stream3_IRQHandler() { crash(69); }
//	void     DMA2_Stream4_IRQHandler() { crash(70); }
	void     ETH_IRQHandler() { crash(71); }
	void     ETH_WKUP_IRQHandler() { crash(72); }
	void     CAN2_TX_IRQHandler() { crash(73); }
	void     CAN2_RX0_IRQHandler() { crash(74); }
	void     CAN2_RX1_IRQHandler() { crash(75); }
	void     CAN2_SCE_IRQHandler() { crash(76); }
//	void     OTG_FS_IRQHandler() { crash(77); }
	void     DMA2_Stream5_IRQHandler() { crash(78); }
//	void     DMA2_Stream6_IRQHandler() { crash(79); }
//	void     DMA2_Stream7_IRQHandler() { crash(80); }
	void     USART6_IRQHandler() { crash(81); }
	void     I2C3_EV_IRQHandler() { crash(82); }
	void     I2C3_ER_IRQHandler() { crash(83); }
	void     OTG_HS_EP1_OUT_IRQHandler() { crash(84); }
	void     OTG_HS_EP1_IN_IRQHandler() { crash(85); }
	void     OTG_HS_WKUP_IRQHandler() { crash(86); }
//	void     OTG_HS_IRQHandler() { crash(87); }
	void     DCMI_IRQHandler() { crash(88); }
	void     RNG_IRQHandler() { crash(89); }
	void     FPU_IRQHandler() { crash(90); }
	void     UART7_IRQHandler() { crash(91); }
	void     UART8_IRQHandler() { crash(92); }
	void     SPI4_IRQHandler() { crash(93); }
	void     SPI5_IRQHandler() { crash(94); }
	void     SPI6_IRQHandler() { crash(95); }
	void     SAI1_IRQHandler() { crash(96); }
	void     LTDC_IRQHandler() { crash(97); }
	void     LTDC_ER_IRQHandler() { crash(98); }
	void     DMA2D_IRQHandler() { crash(99); }
	void     SAI2_IRQHandler() { crash(100); }
	void     QUADSPI_IRQHandler() { crash(101); }
	void     LPTIM1_IRQHandler() { crash(102); }
	void     CEC_IRQHandler() { crash(103); }
	void     I2C4_EV_IRQHandler() { crash(104); }
	void     I2C4_ER_IRQHandler() { crash(105); }
	void     SPDIF_RX_IRQHandler() { crash(106); }
}
