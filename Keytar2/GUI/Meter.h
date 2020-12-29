/*
 * Meter.h - Simple VU meter.
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#ifndef GUI_METER_H_
#define GUI_METER_H_

#include "Obj.h"

namespace Gui
{
	class Meter
	    : public Obj
	{
	public:
		Meter(const Rect &pos);
		virtual ~Meter() { }

		virtual void draw(Gfx &gfx);

		void setValue(unsigned v);

	private:
		unsigned _value;
	};

} // namespace Gui

#endif // GUI_METER_H_
