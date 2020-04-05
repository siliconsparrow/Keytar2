/*
 * AudioFilter.h
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#ifndef AUDIOFILTER_H_
#define AUDIOFILTER_H_

#include <stdint.h>

// This type is used to store audio samples.
typedef int16_t Sample;

// The disco board can support up to 32 bits, 192kHz, see stm32f7xx_hal_i2s.h for more information.

// Supported sample rates.
enum {
	kSampleRate192k  = 192000,
	kSampleRate96k   = 96000,
	kSampleRate48k   = 48000,
	kSampleRate44k1  = 44100,
	kSampleRate22k05 = 22050,
	kSampleRate16k   = 16000,
	kSampleRate11025 = 11025,
	kSampleRate8k    = 8000
};

// Audio parameters.
enum {
	kAudioFrameSize = 256,             // Number of samples in a frame (if stereo, left and right are interleaved).
	kSampleRate     = kSampleRate44k1, // Samples per second.
	kSampleBits     = 16,              // Bits per sample.
	kSampleChannels = 2                // 1=mono, 2=stereo
};

// Base class for all audio processing filters.
class AudioFilter
{
public:
	AudioFilter() { }
	virtual ~AudioFilter() { }

	virtual void fillFrame(Sample *frame) = 0;
};

#endif /* AUDIOFILTER_H_ */
