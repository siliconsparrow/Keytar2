
#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include <stdint.h>

class Systick
{
public:
	class Delegate
	{
	public:
		virtual ~Delegate() { }
		virtual void evtTick() = 0;
	};

	enum {
		kMaxDelegates = 32
	};

	static Systick *instance();

	Systick();

	void add(Delegate *delegate, uint32_t period);

private:
	struct Item
	{
		void init(Delegate *delegate, uint32_t period);
		void exec();

	private:
		uint32_t  _period;
		Delegate *_delegate;
		uint32_t  _counter;

		bool isTime();
	};

	unsigned _nItem;
	Item     _item[kMaxDelegates];

public:
	void tick(); // Called by ISR
};

#endif // _SYSTICK_H_
