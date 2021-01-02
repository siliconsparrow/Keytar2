/*
 * FluidSynth.cpp
 *
 *  Created on: 18 Apr 2020
 *      Author: adam
 */

#include "FilterFluidSynth.h"
#include "fluid_log.h"

#define POLYPHONY      24
//#define SOUNDFONT_FILE "/FPD98.SF2"
#define SOUNDFONT_FILE "/wt_191k_G.SF2"

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

// Dump the current soundfont file and load a new one.
void FilterFluidSynth::replaceSF(const char *filename)
{
	fluid_synth_sfunload(_synth, _sfId, 1);
	_sfId = fluid_synth_sfload(_synth, filename, 1);
}

// Change bank.
bool FilterFluidSynth::setBank(unsigned channel, unsigned bank)
{
	return 0 == fluid_synth_bank_select(_synth, channel, bank);
}

// Change program.
bool FilterFluidSynth::setProgram(unsigned channel, unsigned program)
{
	return 0 == fluid_synth_program_change(_synth, channel, program);
}

void FilterFluidSynth::send(const MIDIMessage &msg)
{
	switch(msg.message())
	{
	case MIDIMessage::NOTE_OFF:
		fluid_synth_noteoff(_synth, msg.channel(), msg.param1());
		break;

	case MIDIMessage::NOTE_ON:
		fluid_synth_noteon(_synth, msg.channel(), msg.param1(), msg.param2());
		break;

	case MIDIMessage::CONTROL_CHANGE:
		fluid_synth_cc(_synth, msg.channel(), msg.param1(), msg.param2());
		break;

	case MIDIMessage::PROGRAM_CHANGE:
		fluid_synth_program_change(_synth, msg.channel(), msg.param1());
		break;

	case MIDIMessage::CHANNEL_PRESSURE:
		fluid_synth_channel_pressure(_synth, msg.channel(), msg.param1());
		break;

	case MIDIMessage::PITCH_BEND:
		fluid_synth_pitch_bend(_synth, msg.channel(), msg.param16bit());
		break;

	default:
		break;
	}
}

#ifdef DEPRECATED
// TODO: All the following could be replaced with a single MIDIMessage handler.

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

#endif // DEPRECATED
