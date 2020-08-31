/*
 * FluidSynth.cpp
 *
 *  Created on: 18 Apr 2020
 *      Author: adam
 */

#include "FilterFluidSynth.h"
#include "fluid_log.h"

#define POLYPHONY      16 // 64
#define SOUNDFONT_FILE "/FPD98.SF2"

void fluidLog(int level, char* message, void* data)
{
	printf("%s\r\n", message);
}

FilterFluidSynth::FilterFluidSynth()
	: _sfId(0)
{
	  fluid_settings_t* settings;

	  fluid_set_log_function(FLUID_INFO, &fluidLog, 0);
	  fluid_set_log_function(FLUID_ERR, &fluidLog, 0);
	  fluid_set_log_function(FLUID_WARN, &fluidLog, 0);
	  fluid_set_log_function(FLUID_DBG, &fluidLog, 0);

	  FLUID_LOG(FLUID_INFO, (char *)"STARTING FLUID");

	  // Create the settings.
	  settings = new_fluid_settings();
	  fluid_settings_setnum(settings, "synth.sample-rate", kSampleRate);
	  fluid_settings_setstr(settings, "synth.reverb.active", "no");
	  fluid_settings_setstr(settings, "synth.chorus.active", "no");
	  fluid_settings_setint(settings, "synth.polyphony", POLYPHONY);

	  /* Create the synthesizer. */
	  _synth = new_fluid_synth(settings);

	  _sfId = fluid_synth_sfload(_synth, SOUNDFONT_FILE, 1);
	  fluid_synth_set_interp_method(_synth, -1, FLUID_INTERP_NONE);
	//  fluid_synth_set_interp_method(synth, -1, FLUID_INTERP_LINEAR);
}

void FilterFluidSynth::fillFrame(StereoSample *frame)
{
#ifdef AUDIO_FORMAT_32BITS
  fluid_synth_write_s32(_synth, AUDIO_BUF_SIZE / 16, (uint32_t *)&buf[0], 0, 2, (uint32_t *)&buf[0], 1, 2 );
#else
  fluid_synth_write_s16(_synth, kAudioFrameSize, (uint16_t *)frame, 0, 2, (uint16_t *)frame, 1, 2 );
#endif
}

void FilterFluidSynth::replaceSF(const char *filename)
{
	fluid_synth_sfunload(_synth, _sfId, 1);
	// CRASHES HERE??!?!
	_sfId = fluid_synth_sfload(_synth, filename, 1);
}

void FilterFluidSynth::noteOn(unsigned noteNum, unsigned velocity)
{
	int midiChannel = 0;
	fluid_synth_noteon(_synth, midiChannel, noteNum, velocity);
}

void FilterFluidSynth::noteOff(unsigned noteNum)
{
	int midiChannel = 0;
	fluid_synth_noteoff(_synth, midiChannel, noteNum);
}

void FilterFluidSynth::midiRecv(uint8_t *msg, uint32_t len) {

	uint8_t chan = msg[1] & 0xf;
	uint8_t msgtype = msg[1] & 0xf0;
	uint8_t b1 =  msg[2];
	uint8_t b2 =  msg[3];
	uint16_t b = ((b2 & 0x7f) << 7) | (b1 & 0x7f);

	switch (msgtype) {
	case 0x80:
		fluid_synth_noteoff(_synth, chan, b1);
		break;
	case 0x90:
		fluid_synth_noteon(_synth, chan, b1, b2);
		break;
	case 0xB0:
		fluid_synth_cc(_synth, chan, b1, b2);
		break;
	case 0xC0:
		fluid_synth_program_change(_synth, chan, b1);
		break;
	case 0xD0:
		fluid_synth_channel_pressure(_synth, chan, b1);
		break;
	case 0xE0:
		fluid_synth_pitch_bend(_synth, chan, b);
		break;
	default:
		break;
	}
}
