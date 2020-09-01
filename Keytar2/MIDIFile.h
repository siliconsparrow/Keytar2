// *****************************************************************************
// **
// ** Load a MID file from disk.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 1-Apr-2018
// **
// *****************************************************************************

#ifndef _MIDFILE_H_
#define _MIDFILE_H_

#include "MIDITrack.h"
#include "FileSystem.h"

class MIDISink;
class MIDIFile
{
public:
#ifdef OLD
	// Values for the MIDFileHeader::fileFormat field.
	enum MIDIFileFormat
	{
		MF_SINGLE_TRACK,     // 0 - File contains a single track only.
		MF_MULTI_TRACK_SYNC, // 1 - File contains multiple tracks which start at the same time.
		MF_MULTI_TRACK_ASYNC // 2 - File contains multiple tracks which start at different times.
	};
#endif // OLD

	MIDIFile();
	~MIDIFile();
	
	bool     load(const TCHAR *filename);
#ifdef OLD
	void     changeTimebase(unsigned newTimebase);
	void     clear();
	bool     isValid() const { return _numTracks > 0; }
	
	unsigned getNumTracks()         const { return _numTracks; }
	unsigned getTicksPerBar()       const;
	unsigned getTicksPerCrotchet()  const { return _ticksPerCrotchet; }
	unsigned getTempo()             const { return _tempo;        }
	unsigned getTimesigNum()        const { return _timesigNum;   }
	unsigned getTimesigDenom()      const { return _timesigDenom; }
	unsigned getLength()            const { return _songLength;   }
	MIDIFileFormat getFileFormat()  const { return _fileFormat;   }

#endif // OLD
	bool     isPlaying() const;
	void     rewind();
	void     stop(MIDISink *midiOut);
	void     start(MIDISink *midiOut, unsigned timestamp = 0);
#ifdef OLD
	bool     exec(MIDISink *midiOut, AccompState &accomp);
	void     setTempo(unsigned tempo) { _tempo = tempo; }
	void     setTimeSignature(unsigned num, unsigned denom) { _timesigNum = num; _timesigDenom = denom; }
	unsigned getBeat() const { return _timer.getBeat(); }
	unsigned getBar()  const { return _timer.getBar();  }


	unsigned getTimestamp() const { return _timer.getCurrentTime(); }

	//_timer.getCurrentTime()
	//	MIDITrack *getTrack(unsigned trackNum) const;

#endif // OLD
private:

	typedef uint32_t MIDITimestamp;

	class MIDIEvent : public MIDIMessage
	{
	public:
		MIDIEvent() : _ts(0) { }

	private:
		MIDITimestamp _ts;
	};

#ifdef OLD
	MIDIFileFormat  _fileFormat;
	unsigned        _ticksPerCrotchet;
	unsigned        _numTracks;
	MIDITrack      *_track;
	unsigned        _tempo;
	unsigned        _timesigNum;
	unsigned        _timesigDenom;
	unsigned        _songLength;
	MIDITimer       _timer;
#endif // OLD
};

#endif // _MIDFILE_H_
