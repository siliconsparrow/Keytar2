// This is a test firmware to try and create a MIDI USB host. When I get it
// working, I'll port it across to the Keytar firmware.

// Drivers and middleware can be found in ../../STM32Cube_FW_F7_V1.16.0
// I'm trying to keep the original directory structure to make it easy to
// apply updates from ST Micro in the future.

#include "platform.h"
#include "USBMidi.h"
extern "C" {
    #include "lcd_log.h"
}

////#include "Gui.h"
////#include "Audio.h"
////#include "FilterMixer.h"
////#include "FileSystem.h"
////#include "FilterReverbFir.h"
////#include "FilterVocoder.h"
////#include "FilterWavStream.h"
////#include "FilterFluidSynth.h"
////#include "PerfMon.h"
////#include "PerfMeter.h"
////#include "UTimer.h"
////#include "MIDIFile.h"
////#include <stdio.h>
////#include <USBStorage/usbd_conf.h>
////#include "main.h"
//
//#define USE_USB_OTG_HS
//
//static __IO uint32_t TimingDelay;
//
//void delay_ms(__IO uint32_t nTime) {
//	TimingDelay = nTime * 10;
//	while (TimingDelay != 0) {}
//}
//
//void TimingDelay_Decrement(void) {
//	if (TimingDelay > 0) TimingDelay--;
//}
//
///*
//extern "C" void TIM2_IRQHandler()
//{
//  USB_OTG_BSP_TimerIRQ();
//}
//
//#ifdef USE_USB_OTG_FS
//extern "C" void OTG_FS_IRQHandler()
//#else
//extern "C" void OTG_HS_IRQHandler()
//#endif
//{
//  USBH_OTG_ISR_Handler(&USB_OTG_Core_dev);
//}
// */

//extern "C" void SysTick_Handler()
//{
//  TimingDelay_Decrement();
//  USB_Host_Handle();
//}

int main()
{
    MPU_Config();       // Configure CPU
    CPU_CACHE_Enable(); // Set up CPU caching

    // STM32F7xx HAL library initialization:
    //   - Configure the Flash ART accelerator on ITCM interface
    //   - Configure the Systick to generate an interrupt each 1 msec
    //   - Set NVIC Group Priority to 4
    //   - Low Level Initialization
    HAL_Init();
    SystemClock_Config(); // Set a faster core speed (216MHz)

    // Set up console on the LCD.
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(1, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(1);
    BSP_LCD_DisplayOn();
    LCD_LOG_Init();
    LCD_LOG_SetHeader((uint8_t *)"USB MIDI Host Test");

    // Start up the USB MIDI host.
    USBMidi *usbMidi = USBMidi::instance();
    printf("USB Started.\n");

    // Main loop
    while(1)
    {
      usbMidi->poll();
    }
}
