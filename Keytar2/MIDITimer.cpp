// *****************************************************************************
// **
// ** Timer to generate tempo and count for MIDI file replay.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 28-Apr-2018
// **
// *****************************************************************************

// IDEA: Maybe there could be a singleton timer instance and each instance of
//       the MIDITimer class has an independent counter, that way you could run
//       several sequences of different lengths and they all stay in time with
//       each other.

// TODO: Override the timer when there are incoming MIDI clock messages.

#include "MIDITimer.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc_ex.h"
#include <vector>

#define MIDITIMER_DEVICE     TIM5
#define MIDITIMER_FREQ       1000000
#define MIDITIMER_IRQN       TIM5_IRQn
#define MIDITIMER_HANDLER    TIM5_IRQHandler
#define MIDITIMER_RCC_ENABLE __HAL_RCC_TIM5_CLK_ENABLE

#ifdef OLD
#endif // OLD

// Erk, globals!
static std::vector<MIDITimer*> g_timers; // List of timer objects to be notified each tick.
static TIM_HandleTypeDef       hMidiTimer;
static unsigned                g_newMatchValue = 0;
static unsigned                g_beatsPerBar = 4;

extern "C" void MIDITIMER_HANDLER()
{
	// Check for counter overflow event.
	if(0 != (MIDITIMER_DEVICE->SR & TIM_SR_UIF))
	{
		MIDITIMER_DEVICE->SR &= ~(TIM_SR_UIF);

		// If the tempo has changed, adjust the interrupt timing.
		if(g_newMatchValue > 0)
		{
			__HAL_TIM_SET_AUTORELOAD(&hMidiTimer, g_newMatchValue);
			g_newMatchValue = 0;
		}

		// Trigger anything that needs to be triggered.
		for(unsigned i = 0; i < g_timers.size(); i++)
			g_timers[i]->tick();
	}
}


// Set up a hardware timer
MIDITimer::MIDITimer()
	: _tickCount(0)
	, _loopPoint(kTimerNoLoop)
//	, _hasLooped(false)
//	, _beat(0)
//	, _bar(0)
//	, _beatCount(0)
{
	static bool g_hwInit = false;

	// Set up timer interrupt if we previously had not.
	if(!g_hwInit)
	{
		timerInit();
		g_hwInit = true;
	}

	// Add the new object to the list.
	NVIC_DisableIRQ(MIDITIMER_IRQN);
	g_timers.push_back(this);
	NVIC_EnableIRQ(MIDITIMER_IRQN);
}

MIDITimer::~MIDITimer()
{
	// Remove ourself from the notification list.
	NVIC_DisableIRQ(MIDITIMER_IRQN);
	for(unsigned i = 0; i < g_timers.size(); )
	{
		if(g_timers[i] == this)
			g_timers.erase(g_timers.begin() + i);

		else
			i++;
	}
	NVIC_EnableIRQ(MIDITIMER_IRQN);
}


unsigned MIDITimer::getBeatsPerBar()
{
	return g_beatsPerBar;
}

// Set up the timer to regularly call the interrupt.
void MIDITimer::timerInit()
{
	// Activate the timer.
	MIDITIMER_RCC_ENABLE();

	// Compute the prescaler value
	uint32_t uwPrescalerValue = (uint32_t) ((SystemCoreClock / 2) / MIDITIMER_FREQ) - 1;

	// Set TIMx instance
	hMidiTimer.Instance = MIDITIMER_DEVICE;
	hMidiTimer.Init.Period = calculateTimerMatchValue(kDefaultTempo);
	hMidiTimer.Init.Prescaler = uwPrescalerValue;
	hMidiTimer.Init.ClockDivision = 0;
	hMidiTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	hMidiTimer.Init.RepetitionCounter = 0;
	hMidiTimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if(HAL_TIM_Base_Init(&hMidiTimer) != HAL_OK) {
		while(1)
			; // Initialization Error
	}

	// Set up timer overflow interrupt (occurs evert 65536us).
	MIDITIMER_DEVICE->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(MIDITIMER_IRQN);

	// Start the timer.
	if(HAL_TIM_Base_Start(&hMidiTimer) != HAL_OK) {
		while(1)
			; // Starting Error
	}
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
		//_hasLooped = true;
		_tickCount = 0;
		//_bar = 0;
		//_beat = 0;
		//_beatCount = 0;
	}

//	_beatCount++;
//	if(_beatCount >= MIDI_TICKS_PER_BEAT)
//	{
//		_beat++;
//		_beatCount = 0;
//		if(_beat >= g_beatsPerBar)
//		{
//			_beat = 0;
//			_bar++;
//		}
//	}
}

#ifdef OLD
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
#endif // OLD

// Set up for looping or pass MIDITIMER_NO_LOOP for no looping.
void MIDITimer::setLoopPoint(unsigned lp)
{
	_loopPoint = lp;
}

unsigned MIDITimer::calculateTimerMatchValue(unsigned bpm)
{
	return (60000000 / bpm) / kDefaultTicksPerBeat;
}
