// *****************************************************************************
// **
// ** Load a MID file from disk.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 1-Apr-2018
// **
// *****************************************************************************

#include "MIDIFile.h"
//#include "MIDI.h"
//#include <string.h>
//#include <malloc.h>

#ifdef OLD

// Overview of the MIDI file format:
//    http://faydoc.tripod.com/formats/mid.htm

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
#endif // OLD

MIDIFile::MIDIFile()
#ifdef OLD
	: _fileFormat(MIDIFile::MF_SINGLE_TRACK)
	, _ticksPerCrotchet(MIDI_TICKS_PER_BEAT)
	, _numTracks(0)
	, _track(0)
	, _tempo(0)
	, _timesigNum(MIDI_DEFAULT_TIMESIG_NUM)
	, _timesigDenom(MIDI_DEFAULT_TIMESIG_DENOM)
	, _songLength(0)
#endif // OLD
{
}

MIDIFile::~MIDIFile()
{
#ifdef OLD
	delete[] _track;
#endif // OLD
}

bool MIDIFile::load(const TCHAR *filename)
{
#ifdef OLD
	FileSystem::File f;
	if(!f.open(filename)) {
		return false;
	}

	MIDFileHeader hdr;
	
	// Read the MID header.
	if(!f.read((uint8_t *)&hdr, sizeof(MIDFileHeader)))
		return false; // Failed to read file header.int
	
	// Check the identity sequence.
	if(0 != memcmp(hdr.magic, MID_FILE_MAGIC, sizeof(hdr.magic)) || 6 != ntohl(hdr.hdrSize) || ntohs(hdr.fileFormat) > 2)
		return false; // Magic number incorrect. This is not a MID file.

	// Dump any old data.
	clear();

	// There are a couple of different ways the tracks can be organized.
	_fileFormat       = (MIDIFileFormat)ntohs(hdr.fileFormat);
	_ticksPerCrotchet = ntohs(hdr.ticksPerCrotchet);

	// Read each track
	_numTracks = ntohs(hdr.numTracks);
	_track = new MIDITrack[_numTracks];

	_songLength = 0;
	for(unsigned idxTrack = 0; idxTrack < _numTracks; idxTrack++)
	{
		if(!_track[idxTrack].load(f, *this))
			return false;

		// TODO: Sometimes you get MIDI tracks with no events. Can they be deleted?

		unsigned trackLength = _track[idxTrack].getTotalLength();
		if(trackLength > _songLength)
			_songLength = trackLength;
	}

	_timer.setLoopPoint(_songLength);

	return true;
#endif // OLD
	return false;
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

// Erase all data.
void MIDIFile::clear()
{
	delete[] _track;
	_track = 0;
	_numTracks = 0;
}

// Calculate how many ticks per bar.
unsigned MIDIFile::getTicksPerBar() const
{
	return (_ticksPerCrotchet * 4 * _timesigNum) / _timesigDenom;
}

#endif // OLD

bool MIDIFile::isPlaying() const
{
	// TODO: Implement me!
	return false;
}


// Rewind all tracks to the start.
void MIDIFile::rewind()
{
#ifdef OLD
	_timer.resetTime();
	for(unsigned i = 0; i < _numTracks; i++)
		_track[i].rewind();
#endif // OLD
}

// Send note-off commands and stop all playback.
void MIDIFile::stop(MIDISink *midiOut)
{
#ifdef OLD
	for(unsigned i = 0; i < _numTracks; i++)
		_track[i].stop(midiOut);
#endif // OLD
}

// Prepare tracks for playback starting from the given timestamp.
void MIDIFile::start(MIDISink *midiOut, unsigned timestamp)
{
#ifdef OLD
	_timer.setCurrentTime(timestamp);

	for(unsigned i = 0; i < _numTracks; i++)
	{
		_track[i].setupForPlayback(midiOut);
		_track[i].jumpTo(_timer.getCurrentTime());
	}
#endif // OLD
}

#ifdef OLD
// Play any events that need to happen at the current time.
// Returns true if the song has looped.
bool MIDIFile::exec(MIDI *midiOut, AccompState &accomp)
{
	for(unsigned i = 0; i < _numTracks; i++)
	{
		_track[i].exec(midiOut, accomp, _timer.getCurrentTime());
	}

	if(_timer.hasLooped())
	{
		_timer.hasLoopedAck();
		return true;
	}

	return false;
}

//// Return the given track or 0 if there is no such track.
//MIDITrack *MIDIFile::getTrack(unsigned trackNum) const
//{
//	if (trackNum >= _numTracks)
//		return 0;
//
//	return &_track[trackNum];
//}

#endif // OLD
