/*
 * Button.cpp - Simple pressy button.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#include "Button.h"

namespace Gui
{

	Button::Button(const Rect &pos, const char *label, void (*onClick)(unsigned), void (*onRelease)(unsigned), unsigned tag)
		: Obj(pos)
		, _label(label)
		, _onClick(onClick)
		, _onRelease(onRelease)
		, _tag(tag)
		, _pressed(false)
	{
	}

	void Button::onTouch(int x, int y)
	{
		_pressed = true;
		dirty();

		if(_onClick != 0) {
			_onClick(_tag);
		}
	}

	void Button::onRelease()
	{
		_pressed = false;
		dirty();

		if(_onRelease != 0) {
			_onRelease(_tag);
		}
	}

	void Button::draw(Gfx &gfx)
	{
		if(_pressed) {
			gfx.setColourBg(Gfx::kColourOrange);
		} else {
			gfx.setColourBg(Gfx::kColourGrey);
		}
		gfx.clear();
		gfx.setColourFg(Gfx::kColourBlack);
		gfx.setFont(Gfx::kFont24);
		gfx.drawText(5, 5, _label);
	}

	void Button::setText(const char *newText)
	{
		_label = newText;
		dirty();
	}

} // namespace Gui
