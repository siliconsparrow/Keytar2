/*
 * TextWindow.cpp - Scrolling text window.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#include "TextWindow.h"

namespace Gui {

	TextWindow::TextWindow(const Rect &r)
		: Obj(r)
		, _cursX(0)
		, _cursY(0)
	{
	}

	void TextWindow::draw(Gfx &gfx)
	{
	}

	// Write some characters to the console.
	int TextWindow::write(Gfx &gfx, const char *ptr, int len, Gfx::Colour col)
	{
		blinkCursor(gfx, false);
		gfx.setColourFg(col);

		for(int i = 0; i < len; i++) {
			char c = ptr[i];

			if(c >= 32 && c < 127) {
				// Regular character.
				gfx.drawChar(_cursX, _cursY, c);
				cursNextChar(gfx);
			} else switch(c) {
			// Special characters.
			case '\n':
				cursNextLine(gfx);
				break;

			case '\f':
				gfx.clear();
				_cursX = _cursY = 0;
			}
		}

		return len;
	}

	void TextWindow::setCursor(Gfx &gfx, int x, int y)
	{
		blinkCursor(gfx, false);
		_cursX = x;
		_cursY = y;
	}

	void TextWindow::blinkCursor(Gfx &gfx, bool on)
	{
		gfx.drawChar(_cursX, _cursY, on ? '_' : ' ');
	}

	void TextWindow::cursNextChar(Gfx &gfx)
	{
		_cursX += gfx.getFontWidth();

		if(_cursX + gfx.getFontWidth() > gfx.getRect().getWidth()) {
			cursNextLine(gfx);
		}
	}

	void TextWindow::cursNextLine(Gfx &gfx)
	{
		_cursX = 0;
		uint16_t y = _cursY + gfx.getFontHeight();
		if(y + gfx.getFontHeight() > gfx.getHeight()) {
			scroll(gfx);
		} else {
			_cursY = y;
		}
	}

	// Scroll the window by one line.
	void TextWindow::scroll(Gfx &gfx)
	{
		// Scroll the window.
		gfx.blt(0, 0, Rect(0, gfx.getFontHeight(), gfx.getWidth(), gfx.getHeight() - gfx.getFontHeight()));

		// Clear the new line.
		gfx.clearRect(Rect(0, _cursY, gfx.getWidth(), gfx.getFontHeight()));
	}

} // namespace Gui
