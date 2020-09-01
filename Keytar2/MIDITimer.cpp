// *****************************************************************************
// **
// ** Timer to generate tempo and count where we are up to in a sequence.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 28-Apr-2018
// **
// *****************************************************************************

#ifdef OLD

// IDEA: Maybe there could be a singleton timer instance and each instance of
//       the MIDITimer class has an independent counter, that way you could run
//       several sequences of different lengths and they all stay in time with
//       each other.

// TODO: Override the timer when there are incoming MIDI clock messages.

#include "MIDITimer.h"
#include "lpc43xx_timer.h"
#include <vector>

// You can change these if you want to use a different timer device.
// Timers 0, 1, 2 and 3 are compatible with this module.
#define MIDI_TIMER_DEVICE  LPC_TIMER0
#define MIDI_TIMER_IRQN    TIMER0_IRQn
#define MIDI_TIMER_HANDLER TIMER0_IRQHandler

static std::vector<MIDITimer*> g_timers;

static unsigned g_newMatchValue = 0;
static unsigned g_beatsPerBar = 4;

extern "C" void MIDI_TIMER_HANDLER()
{
	TIM_ClearIntPending(MIDI_TIMER_DEVICE, TIM_MR0_INT);

	// If the tempo has changed, adjust the interrupt timing.
	if(g_newMatchValue > 0)
	{
		//if(MIDI_TIMER_DEVICE->TC < g_newMatchValue)
		MIDI_TIMER_DEVICE->MR[0] = g_newMatchValue;
		MIDI_TIMER_DEVICE->TC = 0;
		//TIM_UpdateMatchValue(MIDI_TIMER_DEVICE, 0, g_newMatchValue);
		g_newMatchValue = 0;
	}

	// Trigger anything that needs to be triggered.
	for(unsigned i = 0; i < g_timers.size(); i++)
		g_timers[i]->tick();
}

// Set up a hardware timer
MIDITimer::MIDITimer()
	: _tickCount(0)
	, _loopPoint(MIDITIMER_NO_LOOP)
	, _hasLooped(false)
	, _beat(0)
	, _bar(0)
	, _beatCount(0)
{
	// Set up the timer if that had not been done before.
	if(g_timers.empty())
		timerInit();

	// Add ourself to the notification list. This will call our tick() function every timer interrupt.
	NVIC_DisableIRQ(MIDI_TIMER_IRQN);
	g_timers.push_back(this);
	NVIC_EnableIRQ(MIDI_TIMER_IRQN);
}

MIDITimer::~MIDITimer()
{
	// Remove ourself from the notification list.
	NVIC_DisableIRQ(MIDI_TIMER_IRQN);
	for(unsigned i = 0; i < g_timers.size(); )
	{
		if(g_timers[i] == this)
			g_timers.erase(g_timers.begin() + i);

		else
			i++;
	}
	NVIC_EnableIRQ(MIDI_TIMER_IRQN);
}

//// When added to the list, this object will have it's tick() member called
//// with every MIDI tick.
//void MIDITimer::addToNotificationList()
//{
//	// Check we are not already on the list.
//	for(unsigned i = 0; i < g_timers.size(); i++)
//	{
//		if(g_timers[i] == this)
//			return;
//	}
//
//	// Add to the list.
//	g_timers.push_back(this);
//}

//// This object will no longer receive tick() notifications from the timer interrupt.
//void MIDITimer::removeFromNotificationList()
//{
//	for(unsigned i = 0; i < g_timers.size(); )
//	{
//		if(g_timers[i] == this)
//			g_timers.erase(g_timers.begin() + i);
//
//		else
//			i++;
//	}
//}

unsigned MIDITimer::getBeatsPerBar()
{
	return g_beatsPerBar;
}

// Set up the timer to regularly call the interrupt.
void MIDITimer::timerInit()
{
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;

	//TIM_DeInit(MIDI_TIMER_DEVICE);

	// Initialize timer 0, prescale count time of 1uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1;

	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	// Set Match value
	TIM_MatchConfigStruct.MatchValue   = calculateTimerMatchValue(MIDI_DEFAULT_TEMPO);

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(MIDI_TIMER_DEVICE, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(MIDI_TIMER_DEVICE,&TIM_MatchConfigStruct);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(MIDI_TIMER_IRQN, ((0x01<<3)|0x01));
	/* Enable interrupt for timer 0 */
	NVIC_EnableIRQ(MIDI_TIMER_IRQN);
	// To start timer 0
	TIM_Cmd(MIDI_TIMER_DEVICE, ENABLE);
}

// Set the timer to trigger at the desired clock rate.
void MIDITimer::setTempo(unsigned bpm)
{
	g_newMatchValue = calculateTimerMatchValue(bpm);
}

void MIDITimer::setTimeSignature(unsigned num, unsigned denom)
{
	g_beatsPerBar = num;
}

// Called every MIDI tick by the interrupt.
void MIDITimer::tick()
{
	_tickCount++;
	if(_tickCount >= _loopPoint)
	{
		_hasLooped = true;
		_tickCount = 0;
		_bar = 0;
		_beat = 0;
		_beatCount = 0;
	}

	_beatCount++;
	if(_beatCount >= MIDI_TICKS_PER_BEAT)
	{
		_beat++;
		_beatCount = 0;
		if(_beat >= g_beatsPerBar)
		{
			_beat = 0;
			_bar++;
		}
	}
}

// Get the current tick count.
unsigned MIDITimer::getCurrentTime() const
{
	return _tickCount;
}

void MIDITimer::setCurrentTime(unsigned t)
{
	_tickCount = t % _loopPoint;
	_hasLooped = false;
}

// Set current time counter to zero.
void MIDITimer::resetTime()
{
	_hasLooped = false;
	_tickCount = 0;
	_bar = 0;
	_beat = 0;
	_beatCount = 0;
}

// Set up for looping or pass MIDITIMER_NO_LOOP for no looping.
void MIDITimer::setLoopPoint(unsigned lp)
{
	_loopPoint = lp;
}

unsigned MIDITimer::calculateTimerMatchValue(unsigned bpm)
{
	return (60000000 / bpm) / MIDI_TICKS_PER_BEAT;
}

#endif // OLD
