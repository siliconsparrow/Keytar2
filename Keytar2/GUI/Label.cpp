/*
 * Label.cpp - Simple text label.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#include "Label.h"
#include "Gfx.h"
#include <string.h>

namespace Gui {

	Label::Label(const Rect &r, const char *text)
		: Obj(r)
	{
		setText(text);
	}

	void Label::setText(const char *text)
	{
		strncpy(_text, text, kMaxText);
		dirty();
	}

	void Label::draw(Gfx &gfx)
	{
		gfx.clear();
		gfx.drawText(2, 2, _text);
	}

} /* namespace Gui */
