// Drivers and middleware can be found in ../../STM32Cube_FW_F7_V1.16.0
// I'm trying to keep the original directory structure to make it easy to
// apply updates from ST Micro in the future.

// Peripherals I'll be using:
//  GPIO OK
//  SDRAM OK
//  LCD OK
//  Touch screen OK
//  SD card OK
//  FatFs OK
//  USB device (mass storage) OK
//  USB device (MIDI) ??
//  I2S in and out
//  UART
//  DAC maybe? probably not.
//  Watchdog
//  DMA (does that count as a peripheral?)

// TODO: Make all interrupt priorities defined symbols, perhaps in hal_conf.h so
//       I can adjust them to avoid deadlocks.

#include "platform.h"
#include "Gui.h"
//#include "ff_gen_drv.h"
//#include "sd_diskio.h"
#include "Audio.h"
#include "usbd_conf.h"
#include "PerfMon.h"
#include <stdio.h>
#include "main.h"

// Audio uses a lot of interrupts pretty heavily so if I want
// to debug something else, it is handy to turn it off temporarily.
#define ENABLE_AUDIO

//WAV wavTest;

void fnPlayWav()
{
//	wavTest.play();
}

int main()
{
    MPU_Config();       // Configure CPU
    CPU_CACHE_Enable(); // Set up CPU caching
    /* STM32F7xx HAL library initialization:
         - Configure the Flash ART accelerator on ITCM interface
         - Configure the Systick to generate an interrupt each 1 msec
         - Set NVIC Group Priority to 4
         - Low Level Initialization
       */
    HAL_Init();
    SystemClock_Config(); // Set a faster core speed (216MHz)

    perfInit();

    // Set up screen and text rendering.
    Gui::Gui *gui = new Gui::Gui();
    printf(">> Disco Board Audio Test <<\n\n");

    // Start up USB Mass storage device to access the SD card.
    if(USBD_OK == USB_MSC_Init()) {
    	printf("USB init OK\n");
    } else {
    	printf("USB init failed!\n");
    }

#ifdef ENABLE_AUDIO
    // Init audio.
    FilterLineIn mic;
    if(Audio::kStatusOk == Audio::instance()->init()) {
    	Audio::instance()->setFilterChain(&mic);
    	printf("Audio init OK\n");
    } else {
    	printf("Audio init failed!\n");
    }
#endif // ENABLE_AUDIO

    // Set up on-screen controls.
    Gui::Button btnWav(Gui::Rect(10, 10, 146, 30), "PLAY WAV", &fnPlayWav);
    Gui::Meter meterAudio(Gui::Rect(180, 10, 256, 30));
    Gui::Label *lblPerf[nPids];
    for(int i = 0; i < nPids; i++) {
    	lblPerf[i] = new Gui::Label(Gui::Rect(10, 50 + (i * 12), 146, 30), perfPidName[i]);
    	gui->add(lblPerf[i]);
    }
    gui->add(&btnWav);
    gui->add(&meterAudio);

#if 0
    printf("Mounting SD card...\n");
    FATFS SDFatFs;  /* File system object for SD card logical drive */
    char SDPath[4]; /* SD card logical drive path */
    if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0) {
    	printf("SD card mounted OK.\n");
    	if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) != FR_OK) {
    		printf("Filesystem mount failed.\n");
    	} else {
        	printf("Filesystem mounted ok.\n");

            DIR dj;         /* Directory object */
            FILINFO fno;    /* File information */

            FRESULT fr = f_findfirst(&dj, &fno, "", "*");
            while (fr == FR_OK && fno.fname[0]) {         /* Repeat while an item is found */
            	if(0 != (fno.fattrib & AM_DIR)) {
            		printf("%s/\n", fno.fname);
            	} else {
            		printf("%s\n", fno.fname);                /* Print the object name */
            	}
                fr = f_findnext(&dj, &fno);               /* Search for next item */
            }
            f_closedir(&dj);
    	}
    } else {
    	printf("SD card mount failed.\n");
    }
#endif // 0


#ifdef ENABLE_AUDIO
    if(Audio::kStatusOk == Audio::instance()->start()) {
    	printf("Audio streaming started.\n");
    } else {
    	printf("Audio streaming failed to start!\n");
    }
#endif // ENABLE_AUDIO

    uint32_t _tLastPerfUpdate = 0;

    // Main loop
    while(1)
    {
    	gui->tick();

    	// Update CPU usage.
		uint32_t tNow = HAL_GetTick();
		uint32_t dt = tNow - _tLastPerfUpdate;
		if(dt >= 1000) {
			char buf[32];
			unsigned perf[nPids];
			unsigned n = perfReport(perf);
			int tot = 0;
			for(int i = 0; i < nPids; i++)
				tot += perf[i];

			for(int i = 0; i < nPids; i++) {
				int p = (100 * perf[i]) / tot;
				sprintf(buf, "%6s: %3d%%", perfPidName[i], p);
				lblPerf[i]->setText(buf);
			}
			_tLastPerfUpdate = tNow;
		}

    	/*
    	// Display audio level graph.
    	SAMPLE *audioData;
    	unsigned nSamples = audio.getData(&audioData);
		int max = 0;
		for(unsigned i = 0; i < nSamples; i++) {
			if(audioData[i] > max) {
				max = audioData[i];
			}
		}

		meterAudio.setValue(max >> 8);
    	 */
    }
}
