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

#include "FilterSample.h"
#include "platform.h"
#include "Gui.h"
#include "Audio.h"
#include "FilterMixer.h"
#include "FileSystem.h"
#include "usbd_conf.h"
#include "PerfMon.h"
#include <stdio.h>
#include "main.h"

// Audio uses a lot of interrupts pretty heavily so if I want
// to debug something else, it is handy to turn it off temporarily.
#define ENABLE_AUDIO

#define WAV_TEST_FILENAME "/Minimal Heaven vol. 1/LS-MH1 Laser Sweep 10.wav"
FilterSample *g_wavTest = 0;

void fnPlayWav()
{
	if(g_wavTest != 0) {
		g_wavTest->play();
	}
}

// The SD card cannot safely be used by the USB and the audio
// system at the same time. This must be called every now and
// then to handle switching between the two devices.
bool _sdToUsb = false;
void sdCardPoll()
{
	if(_sdToUsb) {
		if(0 == usbIsConnected()) {
			FileSystem::instance()->resume();
			_sdToUsb = false;
		}
	}
}

// Called by the USB driver to obtain possession of the SD card.
// Closes all files and unmounts the card.
extern "C" void sdCardToUsb()
{
	_sdToUsb = true;
	FileSystem::instance()->suspend();
}


//// Test function to cat a file to the console.
//void printFile(const TCHAR *filename)
//{
//	FileSystem::File f;
//
//	if(!f.open(filename)) {
//		fprintf(stderr, "File open failed.\n");
//		return;
//	}
//
//	while(1) {
//		uint8_t buf[100];
//		int count = f.read(buf, 100);
//
//		if(count == 0) {
//			break;
//		}
//
//		if(count < 0) {
//			fprintf(stderr, "File read failed.\n");
//			return;
//		}
//
//		fwrite(buf, 1, count, stdout);
//	}
//
//	printf("\n");
//}

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
    // TODO: There must be a better way to detect USB connection!
    if(USBD_OK == USB_MSC_Init()) {
    	printf("USB init OK\n");
    } else {
    	printf("USB init failed!\n");
    }
#ifdef ENABLE_AUDIO
    // Init audio.
    FilterLineIn mic;
    //FilterWavStream wav;
    FilterSample wav;
    if(wav.load(WAV_TEST_FILENAME)) {
    	printf("WAV loaded OK.\n");
    } else {
    	fprintf(stderr, "Failed to load WAV file!\n");
    }
    g_wavTest = &wav;
    FilterMixer mixer(2);
    mixer.setChannelSource(0, &mic, FilterMixer::kMaxLevel);
    mixer.setChannelSource(1, &wav, FilterMixer::kMaxLevel / 16);
    if(Audio::kStatusOk == Audio::instance()->init()) {
    	Audio::instance()->setFilterChain(&mixer);
    	//Audio::instance()->setFilterChain(&wav);
    	//Audio::instance()->setFilterChain(&mic);
    	printf("Audio init OK\n");
    } else {
    	printf("Audio init failed!\n");
    }
#endif // ENABLE_AUDIO

    // Set up on-screen controls.
    Gui::Button btnWav(Gui::Rect(10, 10, 146, 30), "PLAY WAV", &fnPlayWav);
    //Gui::Meter meterAudio(Gui::Rect(180, 10, 256, 30));
    Gui::Label *lblPerf[nPids];
    for(int i = 0; i < nPids; i++) {
    	lblPerf[i] = new Gui::Label(Gui::Rect(10, 50 + (i * 12), 146, 12), perfPidName[i]);
    	gui->add(lblPerf[i]);
    }
    gui->add(&btnWav);
    //gui->add(&meterAudio);

#ifdef ENABLE_AUDIO
    if(Audio::kStatusOk == Audio::instance()->start()) {
    	printf("Audio streaming started.\n");
    } else {
    	printf("Audio streaming failed to start!\n");
    }
#endif // ENABLE_AUDIO

    uint32_t _tLastPerfUpdate = 0;

    FileSystem::instance(); // Mount the file system.

//    bool _usbConnected = false;
//    Gui::Label _lblUSB(Gui::Rect(180, 30, 100, 16), "");
//    gui->add(&_lblUSB);

//    printFile("/licence.txt");

    unsigned perfCache[nPids];
    for(int i = 0; i < nPids; i++) {
    	perfCache[i] = 255;
    }

    // Main loop
    while(1)
    {
    	gui->tick();

    	// Update CPU usage.
		uint32_t tNow = HAL_GetTick();
		uint32_t dt = tNow - _tLastPerfUpdate;
		if(dt >= 500) {
			char buf[32];
			unsigned perf[nPids];
			unsigned n = perfReport(perf);
			int tot = 0;
			for(int i = 0; i < nPids; i++)
				tot += perf[i];

			for(int i = 0; i < nPids; i++) {
				int p = (100 * perf[i]) / tot;
				if(p != perfCache[i]) {
					sprintf(buf, "%6s: %3d%%", perfPidName[i], p);
					lblPerf[i]->setText(buf);
					perfCache[i] = p;
				}
			}
			_tLastPerfUpdate = tNow;
		}

		// There doesn't seem to be any kind of USB disconnect event
		// so I have to poll for USB disconnect.
		sdCardPoll();

//		// Check USB connection status.
//		if(0 != usbIsConnected()) {
//			if(!_usbConnected) {
//				_lblUSB.setText("[USB]");
//				_usbConnected = true;
//			}
//		} else {
//			if(_usbConnected) {
//				_lblUSB.setText("");
//				_usbConnected = false;
//			}
//		}


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
