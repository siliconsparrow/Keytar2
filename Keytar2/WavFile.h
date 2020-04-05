/*
 * WavFile.h
 *
 *  Created on: 5 Apr 2020
 *      Author: adam
 */

#ifndef WAVFILE_H_
#define WAVFILE_H_

#include "FileSystem.h"
#include "AudioFormat.h"

class WavFile {
public:
	WavFile();

	bool open(const TCHAR *filename);

	const AudioFormat &getAudioFormat() const { return _format; }

	unsigned readBlock(uint8_t *dest, unsigned nBytes);

	bool rewind();

	unsigned getByteSize() const;

private:
	FileSystem::File _f;
	AudioFormat _format;
};

#endif /* WAVFILE_H_ */
