// *****************************************************************************
// **
// ** Timer to generate tempo and count for MIDI file replay.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 28-Apr-2018
// **
// *****************************************************************************

#ifndef MIDITIMER_H_
#define MIDITIMER_H_

class MIDITimer
{
public:

	enum {
		kDefaultTicksPerBeat = 120, // 480 is the resolution used by the Kronos.
		kDefaultTempo        = 120, // Tempo in beats per minute.
		kDefaultTimeSigNum   = 4,
		kDefaultTimeSigDenom = 4,
		kTimerNoLoop         = 0xFFFFFFFF, // Token to prevent looping.
	};

	MIDITimer();
	~MIDITimer();

	static void     setTempo(unsigned bpm); // Sets tempo for all timers.
	static void     setTimeSignature(unsigned num, unsigned denom);
	static unsigned getBeatsPerBar();

#ifdef OLD
	void     setCurrentTime(unsigned t);
	unsigned getCurrentTime() const;
	void     resetTime();
#endif // OLD
	void     setLoopPoint(unsigned lp);
#ifdef OLD
	unsigned getLoopPoint() const { return _loopPoint; }

	bool     hasLooped() const { return _hasLooped; }
	void     hasLoopedAck() { _hasLooped = false; }

	unsigned getBeat() const { return _beat; }
	unsigned getBar()  const { return _bar;  }

	//void addToNotificationList();

#endif // OLD

private:
	volatile unsigned _tickCount;
	volatile unsigned _loopPoint;
#ifdef OLD
	volatile bool     _hasLooped;
	volatile unsigned _beat;
	volatile unsigned _bar;
	volatile unsigned _beatCount;

	void removeFromNotificationList();
#endif // OLD

	static unsigned calculateTimerMatchValue(unsigned bpm);
	static void     timerInit();

public:
	void tick();
};

#endif /* MIDITIMER_H_ */
