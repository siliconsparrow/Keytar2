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
//  I2S in and out OK
//  UART
//  DAC maybe? probably not.
//  Watchdog
//  DMA (does that count as a peripheral?)

// Things I could do next:
//   Time-domain effects (delay, granulation)
//   Drum machine
//   Software synth
//   FFT


// TODO: Make all interrupt priorities defined symbols, perhaps in hal_conf.h so
//       I can adjust them to avoid deadlocks.

#include "FilterSample.h"
#include "platform.h"
#include "Gui.h"
#include "Audio.h"
#include "FilterMixer.h"
#include "FileSystem.h"
#include "FilterReverbFir.h"
#include "FilterVocoder.h"
#include "FilterWavStream.h"
#include "usbd_conf.h"
#include "PerfMon.h"
#include "PerfMeter.h"
#include <stdio.h>
#include "main.h"

// Audio uses a lot of interrupts pretty heavily so if I want
// to debug something else, it is handy to turn it off temporarily.
#define ENABLE_AUDIO

// #define WAV_TEST_FILENAME "/Minimal Heaven vol. 1/LS-MH1 Laser Sweep 10.wav"
#define WAV_TEST_FILENAME "/Large FX Collection/LS LFXC Short-Sound 022.wav"

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

// Quickie to take left-channel data and copy it to the right channel
// so I get both channels playing microphone data.
//class FilterLeftToStereo : public AudioFilter
//{
//public:
//	FilterLeftToStereo() : _source(0) { }
//	virtual ~FilterLeftToStereo() { }
//
//	void setSource(AudioFilter *src) { _source = src; }
//
//	virtual void fillFrame(Sample *frame)
//	{
//		_source->fillFrame(frame);
//		StereoSample *ss = (StereoSample *)frame;
//		for(int i = 0; i < kAudioFrameSize / 2; i++) {
//			ss[i].r = ss[i].l;
//		}
//	}
//
//private:
//	AudioFilter *_source;
//};

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
    uwTickFreq = HAL_TICK_FREQ_100HZ;
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
    FilterLineIn mic(FilterLineIn::chanLeft);
    //FilterLeftToStereo smic;
    //smic.setSource(&mic);

    //FilterVocoder vocoder;
    //vocoder.setSource(&smic);
    //FilterWavStream wav;
    FilterSample wav;
    if(wav.load(WAV_TEST_FILENAME)) {
    	printf("WAV loaded OK.\n");
    } else {
    	fprintf(stderr, "Failed to load WAV file!\n");
    }
    g_wavTest = &wav;

    FilterReverbFir reverb;
    reverb.setSource(&mic);

    FilterMixer mixer(2);
    mixer.setChannelSource(0, &reverb, FilterMixer::kMaxLevel / 16);
    mixer.setChannelSource(1, &wav, FilterMixer::kMaxLevel / 16);
    if(Audio::kStatusOk == Audio::instance()->init()) {
    	Audio::instance()->setFilterChain(&mixer);
    	printf("Audio init OK\n");
    } else {
    	printf("Audio init failed!\n");
    }
#endif // ENABLE_AUDIO

    // Set up on-screen controls.
    Gui::Button btnWav(Gui::Rect(10, 10, 146, 30), "PLAY WAV", &fnPlayWav);
    PerfMeter perf(gui, 480 - PerfMeter::kWidth, 0);
    //Gui::Meter meterAudio(Gui::Rect(180, 10, 256, 30));
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

    // Main loop
    while(1)
    {
    	gui->tick();


		// There doesn't seem to be any kind of USB disconnect event
		// so I have to poll for USB disconnect.
		sdCardPoll();

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
