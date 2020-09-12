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
//  USB host (MIDI) ??
//  I2S in and out OK
//  UART
//  DAC maybe? probably not.
//  Watchdog
//  DMA OK

// Things I could do next:
//   Time-domain effects (delay, granulation)
//   Drum machine
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
#include "FilterFluidSynth.h"
#include "usbd_conf.h"
#include "PerfMon.h"
#include "PerfMeter.h"
#include "UTimer.h"
#include "MIDIFile.h"
#include <stdio.h>
#include "main.h"

// TEST
#include "ff_wrapper.h"

// Audio uses a lot of interrupts pretty heavily so if I want
// to debug something else, it is handy to turn it off temporarily.
#define ENABLE_AUDIO

// #define WAV_TEST_FILENAME "/Minimal Heaven vol. 1/LS-MH1 Laser Sweep 10.wav"
#define WAV_TEST_FILENAME "/Large FX Collection/LS LFXC Short-Sound 022.wav"

//FilterSample *g_wavTest = 0;

//void fnPlayWav(unsigned tag)
//{
//	if(g_drums != 0) {
//		g_drums->midi(MidiMessage(MidiMessage::NOTE_ON,
//				                  MidiMessage::kDrumtrackChannel,
//								  (uint8_t)MidiMessage::C4 + (uint8_t)FilterDrumKit::snare));
//	}
////	if(g_wavTest != 0) {
////		g_wavTest->play();
////	}
//}

FilterFluidSynth *g_synth = 0;
MIDIFile         *g_mid   = 0;

void fnKbPress(unsigned tag)
{
	if(g_synth != 0) {
		g_synth->send(MIDIMessage(MIDIMessage::NOTE_ON, MIDIMessage::CHANNEL1, tag, 127));
	}
}

void fnKbRelease(unsigned tag)
{
	if(g_synth != 0) {
		g_synth->send(MIDIMessage(MIDIMessage::NOTE_OFF, MIDIMessage::CHANNEL1, tag));
	}
}

void fnDrumStartStop(unsigned tag)
{
	if(0 == g_mid)
		return;

	if(g_mid->isPlaying()) {
		g_mid->stop(g_synth);
	} else {
		g_mid->rewind();
		g_mid->start(g_synth);
	}
}

//const char *PATCHFILE[] = {
//	"/Soundfonts/Kaputt Sine.sf2",
//	"/Soundfonts/Analog Saw.sf2",
//	"/Soundfonts/Happy Mellow.sf2",
//	"/Soundfonts/Dirty Sub.sf2",
//	"/Soundfonts/FM Modulator.sf2",
//	"/Soundfonts/Acid SQ Neutral.sf2",
//	"/Soundfonts/Beeper.sf2",
//	"/Soundfonts/Perfect Sine.sf2",
//	"/Soundfonts/Synth E.sf2",
//	"/Soundfonts/Sine Wave.sf2",
//	"/Soundfonts/Solar Wind.sf2",
//	"/Soundfonts/Dance Trance.sf2",
//	"/Soundfonts/Candy Bee.sf2",
//	"/Soundfonts/Poly Special Mono.sf2",
//	"/Soundfonts/Hyper Saw.sf2",
//	"/Soundfonts/Super Saw 3.sf2",
//	"/Soundfonts/Super Saw 1.sf2",
//	"/Soundfonts/Super Saw 2.sf2",
//	"/Soundfonts/Pulse Wobbler.sf2",
//	0
//};

void fnPatch(unsigned tag)
{
	//static unsigned drumpatch[] = { 0, 16, 24, 25, 32, 40, 48, 64 };
	//g_synth->setProgram(MIDIMessage::CHANNEL10, drumpatch[tag]);
	g_synth->setProgram(MIDIMessage::CHANNEL1, tag);
	//g_synth->replaceSF(PATCHFILE[tag]);
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
    Gui::Gui sGui;
    Gui::Gui *gui = &sGui;
    printf(">> Disco Board Audio Test <<\n\n");

    // Start up USB Mass storage device to access the SD card.
    // TODO: There must be a better way to detect USB connection!
    if(USBD_OK == USB_MSC_Init()) {
    	printf("USB init OK\n");
    } else {
    	printf("USB init failed!\n");
    }

    FileSystem::instance(); // Mount the file system.

#ifdef ENABLE_AUDIO

    // Set up microphone input.
    FilterLineIn mic(FilterLineIn::chanLeft);

    // Set up fluid synth.
    uTimerInit();
    FilterFluidSynth synth;
    g_synth = &synth;

    // Final mixdown before the sound is output.
    FilterMixer mixer(2);
    mixer.setChannelSource(0, &mic, FilterMixer::kMaxLevel / 2);
    mixer.setChannelSource(1, &synth, FilterMixer::kMaxLevel / 2);

    // Init audio streaming.
    if(Audio::kStatusOk == Audio::instance()->init()) {
    	Audio::instance()->setFilterChain(&mixer);
    	printf("Audio init OK\n");
    } else {
    	printf("Audio init failed!\n");
    }
#endif // ENABLE_AUDIO

    // Set up on-screen controls.
    //Gui::Button btnWav(Gui::Rect(10, 10, 146, 30), "PLAY WAV", &fnPlayWav);
    PerfMeter perf(gui, 480 - PerfMeter::kWidth, 0);
    //gui->add(&btnWav);

#ifdef ENABLE_AUDIO
    if(Audio::kStatusOk == Audio::instance()->start()) {
    	printf("Audio streaming started.\n");
    } else {
    	printf("Audio streaming failed to start!\n");
    }
#endif // ENABLE_AUDIO

    // Keyboard
    const unsigned NOTENUM[] = {
    	MIDIMessage::C3,
		MIDIMessage::D3,
		MIDIMessage::E3,
		MIDIMessage::F3,
		MIDIMessage::G3,
		MIDIMessage::A3,
		MIDIMessage::B3,
		MIDIMessage::C4
    };
    const char *KEYNAME[] = { "C","D","E","F","G","A","B","C" };
    for(int i = 0, x = 0; i < 8; i++) {
    	gui->add(new Gui::Button(Gui::Rect(x, 66, 55, 50), KEYNAME[i], &fnKbPress, &fnKbRelease, NOTENUM[i]));
    	x += 60;
    }

    // Patch select
    // TODO: Select patches within SF2 file.
    int x = 170;
    const char *NUM[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" };
    for(int i = 0; i < 8; i++) {
    	char buf[2];
    	buf[0] = '1' + i;
    	buf[1] = 0;
    	gui->add(new Gui::Button(Gui::Rect(x, 10, 25, 30), NUM[i], 0, &fnPatch, i));
    	x += 28;
    }

    // TEST: Load a MIDI file.
    MIDIFile midiFile;
    AccompState accomp;
    if(midiFile.load("/testMidi.mid")) {
    	printf("Loaded MIDI file OK.\r\n");
    	g_mid = &midiFile;
    } else {
    	printf("MIDI File failed to load.\r\n");
    }

    // Create button to start/stop drum pattern.
    gui->add(new Gui::Button(Gui::Rect(10, 10, 146, 30), "START", 0, &fnDrumStartStop));

    uint32_t _tLastPerfUpdate = 0;

    Gui::Label beatDisplay(Gui::Rect(170, 40, 100, 25), "BEAT");
    gui->add(&beatDisplay);

    int lastBeat = 0;

    // Main loop
    while(1)
    {
    	// Check for touch events and update GUI objects on-screen.
    	gui->tick();

		// There doesn't seem to be any kind of USB disconnect event
		// so I have to poll for USB disconnect.
		sdCardPoll();

		// Do MIDI playback stuff.
		if(g_mid->isPlaying())
			g_mid->exec(g_synth, accomp);

		char buf[80];
		int beat = midiFile.getBeat();
		if(beat != lastBeat) {
			lastBeat = beat;
			sprintf(buf, "%03d-%d", midiFile.getBar() + 1, beat + 1);
			beatDisplay.setText(buf);
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
