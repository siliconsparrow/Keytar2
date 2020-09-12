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
		kDefaultTicksPerBeat = 120, // 480 is the resolution used by the Kronos. 120 is more standard.
		kDefaultTempo        = 120, // Default tempo in beats per minute.
		kDefaultTimeSigNum   = 4,   // Default time signature.
		kDefaultTimeSigDenom = 4,
		kTimerNoLoop         = 0xFFFFFFFF, // Token to prevent looping.
	};

	class Delegate
	{
	public:
		virtual ~Delegate() { }
		virtual void tick(unsigned currentTime) = 0;
	};

	MIDITimer();
	~MIDITimer();

	static void     setTempo(unsigned bpm); // Sets tempo for all timers.
	static void     setTimeSignature(unsigned num, unsigned denom);
	static unsigned getBeatsPerBar();

	void     setCurrentTime(unsigned t);
	unsigned getCurrentTime() const;
	void     resetTime();
	void     setLoopPoint(unsigned lp);
#ifdef OLD
	unsigned getLoopPoint() const { return _loopPoint; }

	bool     hasLooped() const { return _hasLooped; }
	void     hasLoopedAck() { _hasLooped = false; }
#endif // OLD

	unsigned getBeat() const { return _beat; }
	unsigned getBar()  const { return _bar;  }

	//void addToNotificationList();

	void setDelegate(Delegate *delegate);
	void start();
	void stop();

private:
	volatile unsigned _tickCount;
	volatile unsigned _loopPoint;
	volatile unsigned _beatCount;
	volatile unsigned _beat;
	volatile unsigned _bar;
	Delegate          *_delegate;

#ifdef OLD
	volatile bool     _hasLooped;

	void removeFromNotificationList();
#endif // OLD

	static unsigned calculateTimerMatchValue(unsigned bpm);
	static void     timerInit();

public:
	void tick();
};

#endif /* MIDITIMER_H_ */
