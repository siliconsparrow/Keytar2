/*
 * Meter.cpp - Simple VU meter.
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#include "Meter.h"

namespace Gui
{
	Meter::Meter(const Rect &pos)
		: Obj(pos)
		, _value(0)
	{
	}

	void Meter::setValue(unsigned v)
	{
		_value = v;
		dirty();
	}

	void Meter::draw(Gfx &gfx)
	{
		gfx.setColourFg(Gfx::kColourCyan);
		gfx.fillRect(Rect(0, 0, _value, gfx.getHeight()));
		gfx.clearRect(Rect(_value, 0, gfx.getWidth() - _value, gfx.getHeight()));
	}

} // namespace Gui
