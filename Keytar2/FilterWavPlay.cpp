/*
 * FilterWavPlay.cpp
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#include "FilterWavPlay.h"
#include <string.h>

// WAV file header.
typedef struct {
  uint32_t ChunkID;       /* 0 */
  uint32_t FileSize;      /* 4 */
  uint32_t FileFormat;    /* 8 */
  uint32_t SubChunk1ID;   /* 12 */
  uint32_t SubChunk1Size; /* 16*/
  uint16_t AudioFormat;   /* 20 */
  uint16_t NbrChannels;   /* 22 */
  uint32_t SampleRate;    /* 24 */

  uint32_t ByteRate;      /* 28 */
  uint16_t BlockAlign;    /* 32 */
  uint16_t BitPerSample;  /* 34 */
  uint32_t SubChunk2ID;   /* 36 */
  uint32_t SubChunk2Size; /* 40 */
}WAVE_FormatTypeDef;

FilterWavPlay::FilterWavPlay()
	: _isPlaying(false)
	, _channels(0)
	, _bits(0)
	, _rate(0)
{
}

FilterWavPlay::~FilterWavPlay()
{
}

bool FilterWavPlay::load(const TCHAR *filename)
{
	if(!_f.open(filename)) {
		return false;
	}

	WAVE_FormatTypeDef hdr;
	int cb = _f.read((uint8_t *)&hdr, sizeof(hdr));
	if(cb < sizeof(hdr)) {
		return false;
	}

	_channels = hdr.NbrChannels;
	_bits = hdr.BitPerSample;
	_rate = hdr.SampleRate;

	return true;
}

void FilterWavPlay::play()
{
	_f.seek(sizeof(WAVE_FormatTypeDef));
	_isPlaying = true;
}

void FilterWavPlay::stop()
{
	_isPlaying = false;
}

void FilterWavPlay::fillFrame(Sample *frame)
{
	if(_isPlaying) {
		int nBytes = sizeof(Sample) * kAudioFrameSize;
		uint8_t *dest = (uint8_t *)frame;
		int cb = _f.read(dest, nBytes);
		if(cb < 0) {
			_isPlaying = false;
			_f.seek(sizeof(WAVE_FormatTypeDef));
		}
		if(cb < nBytes) {
			// end of file.
			memset(&dest[cb], 0, nBytes - cb);
			_isPlaying = false;
			_f.seek(sizeof(WAVE_FormatTypeDef));
		}
	} else {
		// Not currently playing, fill the buffer with silence.
		memset(frame, 0, sizeof(Sample) * kAudioFrameSize);
	}
}
