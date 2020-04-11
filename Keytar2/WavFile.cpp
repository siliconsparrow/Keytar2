/*
 * WavFile.cpp
 *
 *  Created on: 5 Apr 2020
 *      Author: adam
 */

#include "WavFile.h"

// WAV file header.
struct __attribute__((packed)) WAVE_FormatTypeDef {
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
};

WavFile::WavFile()
{
}

bool WavFile::open(const TCHAR *filename)
{
	if(!_f.open(filename))
		return false;

	WAVE_FormatTypeDef hdr;
	int cb = _f.read((uint8_t *)&hdr, sizeof(hdr));
	if(cb < sizeof(hdr)) {
		return false;
	}

	_format = AudioFormat(hdr.SampleRate, hdr.BitPerSample, hdr.NbrChannels);
	return true;
}

unsigned WavFile::readBlock(uint8_t *dest, unsigned nBytes)
{
	return _f.read(dest, nBytes);
}

unsigned WavFile::getByteSize() const
{
	return _f.getSize() - sizeof(WAVE_FormatTypeDef);
}

bool WavFile::rewind()
{
	return _f.seek(sizeof(WAVE_FormatTypeDef));
}
