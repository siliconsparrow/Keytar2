// *****************************************************************************
// **
// ** Timer to generate tempo and count where we are up to in a sequence.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 28-Apr-2018
// **
// *****************************************************************************

#ifndef MIDITIMER_H_
#define MIDITIMER_H_

#ifdef OLD

#define MIDI_TICKS_PER_BEAT        120 // 480 is the resolution used by the Kronos.
#define MIDI_DEFAULT_TEMPO         120
#define MIDI_DEFAULT_TIMESIG_NUM     4
#define MIDI_DEFAULT_TIMESIG_DENOM   4

#define MIDITIMER_NO_LOOP 0xFFFFFFFF

class MIDITimer
{
public:
	MIDITimer();
	~MIDITimer();

	static void     setTempo(unsigned bpm); // Sets tempo for all timers.
	static void     setTimeSignature(unsigned num, unsigned denom);
	static unsigned getBeatsPerBar();

	void     setCurrentTime(unsigned t);

	unsigned getCurrentTime() const;
	void     resetTime();
	void     setLoopPoint(unsigned lp);
	unsigned getLoopPoint() const { return _loopPoint; }

	bool     hasLooped() const { return _hasLooped; }
	void     hasLoopedAck() { _hasLooped = false; }

	unsigned getBeat() const { return _beat; }
	unsigned getBar()  const { return _bar;  }

	//void addToNotificationList();


	void tick(); // Do not call this. Should be private really.

private:
	volatile unsigned _tickCount;
	volatile unsigned _loopPoint;
	volatile bool     _hasLooped;
	volatile unsigned _beat;
	volatile unsigned _bar;
	volatile unsigned _beatCount;

	static unsigned calculateTimerMatchValue(unsigned bpm);
	static void     timerInit();

	void removeFromNotificationList();
};

#endif // OLD


#endif /* MIDITIMER_H_ */
