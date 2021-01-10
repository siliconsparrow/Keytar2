// *****************************************************************************
// **
// ** Load a MID file from disk.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 1-Apr-2018
// **
// *****************************************************************************

#include "MIDIFile.h"
#include "MIDI.h"
#include "FileSystem.h"
#include <string.h>
//#include <malloc.h>

// Overview of the MIDI file format:   http://faydoc.tripod.com/formats/mid.htm

// 4CC identifier found at the start of a MID file.
#define MID_FILE_MAGIC  ((const uint8_t *)"MThd")

// MID file will begin with this header.
// All values are in big-endian byte order so watch out!
struct __attribute__((packed)) MIDFileHeader
{
	char     magic[4];         // 4CC Identity sequence so you know it's a MID file.
	uint32_t hdrSize;          // Size of the rest of this header (always 6)
	uint16_t fileFormat;       // File format 0, 1 or 2.
	uint16_t numTracks;        // Number of MIDI tracks in this file.
	uint16_t ticksPerCrotchet; // How many tempo ticks per quarter-note.
};


MIDIFile::MIDIFile()
	: _fileFormat(MIDIFile::MF_SINGLE_TRACK)
	, _numTracks(0)
	, _track(0)
	, _ticksPerCrotchet(MIDITimer::kDefaultTicksPerBeat)
	, _tempo(0)
	, _timesigNum(MIDITimer::kDefaultTimeSigNum)
	, _timesigDenom(MIDITimer::kDefaultTimeSigDenom)
	, _songLength(0)
	, _isPlaying(false)
	, _lastTick(0)
{
}

MIDIFile::~MIDIFile()
{
	delete[] _track;
}

bool MIDIFile::load(const TCHAR *filename)
{
	// Clear out any old data.
	clear();

	// Open the file.
	FileSystem::File f;
	if(!f.open(filename)) {
		return false;
	}

	// Read the file header.
	MIDFileHeader hdr;
	
	// Read the MID header.
	if(!f.read((uint8_t *)&hdr, sizeof(MIDFileHeader)))
		return false; // Failed to read file header.
	
	// Check the identity sequence.
	if(0 != memcmp(hdr.magic, MID_FILE_MAGIC, sizeof(hdr.magic)) || 6 != ntohl(hdr.hdrSize) || ntohs(hdr.fileFormat) > 2)
		return false; // Magic number incorrect. This is not a MID file.

	// Dump any old data.
	clear();

	// There are a couple of different ways the tracks can be organized.
	_fileFormat       = (MIDIFileFormat)ntohs(hdr.fileFormat);
	_ticksPerCrotchet = ntohs(hdr.ticksPerCrotchet);

	// Read each track
	int numTracksInFile = ntohs(hdr.numTracks);
	_track = new MIDITrack[numTracksInFile];

	//_songLength = 0;
	for(int idxTrack = 0; idxTrack < numTracksInFile; idxTrack++)
	{
		MIDITrack &currentTrack = _track[_numTracks];

		if(!currentTrack.load(f, *this))
		{
			clear();
			return false;
		}

		// Does the track contain any notes?
		if(currentTrack.isEmpty())
		{
			// Track was blank or just contained metadata which is useless for replay
			// purposes. So ignore it and don't count it towards the total tracks.
			currentTrack = MIDITrack(); // Clear useless data.
			continue;
		}

		// Successfully loaded a track.
		_numTracks++;

		// Set the song length to that of the longest track.
		if(currentTrack.getTotalLength() > _songLength)
			_songLength = currentTrack.getTotalLength();
	}

	_timer.setLoopPoint(_songLength);

	return true;
}

#ifdef OLD
void MIDIFile::changeTimebase(unsigned newTimebase)
{
	_songLength = 0;
	for(unsigned i = 0; i < _numTracks; i++)
	{
		_track[i].changeTimebase(newTimebase, _ticksPerCrotchet);

		unsigned trackLength = _track[i].getTotalLength();
		if(trackLength > _songLength)
			_songLength = trackLength;
	}

	_ticksPerCrotchet = newTimebase;

	if(_timer.getLoopPoint() != MIDITIMER_NO_LOOP)
		_timer.setLoopPoint(_songLength);
}
#endif // OLD

// Erase all data.
void MIDIFile::clear()
{
	delete[] _track;
	_track = 0;
	_numTracks = 0;
	_songLength = 0;
}

// Calculate how many ticks per bar.
unsigned MIDIFile::getTicksPerBar() const
{
	return (_ticksPerCrotchet * 4 * _timesigNum) / _timesigDenom;
}

// Rewind all tracks to the start.
void MIDIFile::rewind()
{
	_timer.resetTime();
	_timer.setTempo(_tempo);
	_timer.setTimeSignature(_timesigNum, _timesigDenom);
	for(unsigned i = 0; i < _numTracks; i++)
		_track[i].rewind();
}

// Send note-off commands and stop all playback.
void MIDIFile::stop(MIDISink *midiOut)
{
	_isPlaying = false;
	for(unsigned i = 0; i < _numTracks; i++)
		_track[i].stop(midiOut);
}

// Prepare tracks for playback starting from the given timestamp.
void MIDIFile::start(MIDISink *midiOut, unsigned timestamp)
{
	_isPlaying = true;

	_timer.setCurrentTime(timestamp);
	_lastTick = 0xFFFFFFFF;

	for(unsigned i = 0; i < _numTracks; i++)
	{
		_track[i].setupForPlayback(midiOut);
		_track[i].jumpTo(_timer.getCurrentTime());
	}
}

// Play any events that need to happen at the current time.
// Returns true if the song has looped.
// TODO: Really needs to be interrupt-driven but polled is OK for now.
bool MIDIFile::exec(MIDISink *midiOut, AccompState &accomp)
{
	if(!isPlaying())
		return true;

	unsigned tNow = _timer.getCurrentTime();
	if(_lastTick == tNow)
		return false;

	_lastTick = tNow;

	for(unsigned i = 0; i < _numTracks; i++)
		_track[i].exec(midiOut, accomp, tNow);

#ifdef OLD
	if(_timer.hasLooped())
	{
		_timer.hasLoopedAck();
		return true;
	}
#endif // OLD

	return false;
}

#ifdef OLD
//// Return the given track or 0 if there is no such track.
//MIDITrack *MIDIFile::getTrack(unsigned trackNum) const
//{
//	if (trackNum >= _numTracks)
//		return 0;
//
//	return &_track[trackNum];
//}

#endif // OLD
