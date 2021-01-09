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

// NOTE: I found the USB host stuff doesn't work if you allocate it's data into SDRAM
//       so I currently have malloc() allocating from SRAM but there's only 240k of that.

// TODO: Make all interrupt priorities defined symbols, perhaps in hal_conf.h so
//       I can adjust them to avoid deadlocks.

//#include "FilterSample.h"
#include "Audio.h"
//#include "FilterMixer.h"
//#include "FilterReverbFir.h"
//#include "FilterVocoder.h"
//#include "FilterWavStream.h"
//#include "MIDIFile.h"
#include "stm32746g_discovery_sdram.h"
#include "Gui.h"
#include "platform.h"
#include "PerfMon.h"
#include "PerfMeter.h"
#include "MusicKeyboard.h"
#include "USBMidi.h"
#include "USBStorage.h"
#include "FileSystem.h"
#include "Synth.h"
#include "main.h"
#include "chipAlloc.h"
#include <stdio.h>



#ifdef OLD

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


int main()
{
#ifdef ENABLE_AUDIO

    // Set up microphone input.
    FilterLineIn mic(FilterLineIn::chanLeft);

    // Set up fluid synth.
    //#include "UTimer.h"
    uTimerInit();
    FilterFluidSynth synth;
    g_synth = &synth;

    // Final mixdown before the sound is output.
    FilterMixer mixer(2);
    mixer.setChannelSource(0, &mic, FilterMixer::kMaxLevel / 2);
    mixer.setChannelSource(1, &synth, FilterMixer::kMaxLevel / 2);

#endif // ENABLE_AUDIO


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

    Gui::Label beatDisplay(Gui::Rect(170, 40, 100, 25), "BEAT");
    gui->add(&beatDisplay);

    int lastBeat = 0;

    // Main loop
    while(1)
    {
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
#endif // OLD

int main()
{
	// Set up CPU core.
    MPU_Config();
    CPU_CACHE_Enable();

    // Set up HAL library:
    //  - Flash ART accelerator on ITCM interface
    //  - Systick to generate an interrupt each 1 msec
    //  - Set NVIC Group Priority to 4
    //   - Low Level Initialization
    uwTickFreq = HAL_TICK_FREQ_100HZ;
    HAL_Init();

    // Set a faster core speed (216MHz)
    SystemClock_Config();

    // Activate the SDRAM.
    BSP_SDRAM_Init();
    SDmallocInit();

    // Set up process monitor.
    perfInit();

    // Set up screen and text rendering.
    Gui::Gui *gui = Gui::Gui::instance();
    printf(">> Sound Module Test With USB MIDI <<\n");

    // Set up SD card.
    FileSystem *fs = FileSystem::instance();

    // Set up the MIDI USB Host.
    USBMidi *usbMidi = USBMidi::instance();

    // Set up USB Storage to access the SD Card.
    USBStorage *usbStorage = USBStorage::instance();

    // The main object that runs everything.
    Synth *synth = new Synth();
    usbMidi->setDelegate(synth);

    // Create controls.
    synth->createControls(gui);

    // Set up microphone input.
    //FilterLineIn mic(FilterLineIn::chanLeft); // Mono headset mic on line-in left channel.

    // Final mixdown before the sound is output.
    //FilterMixer mixer(2);
    //mixer.setChannelSource(0, &mic, FilterMixer::kMaxLevel / 2);
    //mixer.setChannelSource(1, &synth, FilterMixer::kMaxLevel / 2);
	//Audio::instance()->setFilterChain(&mic/*&mixer*/);

    // Init audio streaming.
    if(Audio::kStatusOk == Audio::instance()->init()) {
    	Audio::instance()->setFilterChain(synth->getFilter());
    	printf("Audio init OK\n");
    } else {
    	printf("Audio init failed!\n");
    }

    // Set up CPU and memory stats.
    PerfMeter perf(gui, 480 - PerfMeter::kWidth, 0);

    if(Audio::kStatusOk == Audio::instance()->start()) {
    	printf("Audio streaming started.\n");
    } else {
    	printf("Audio streaming failed to start!\n");
    }

    // Main loop
    while(1) {

    	// Check for USB events.
    	usbMidi->poll();
    	usbStorage->poll();

    	// Check for touch events and update GUI objects on-screen.
    	gui->tick();
    }
}
