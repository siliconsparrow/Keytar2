/*
 * TextWindow.cpp - Scrolling text window.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#include "TextWindow.h"

#include "stm32746g_discovery_lcd.h" // For the scrolling function - should really use a BLT in class Gfx.
#include <string.h> // Also for the scroll function.


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

	//	void TextWindow::setColourFg(Colour col);
	//	void TextWindow::setColourBg(Colour col);
	//	void TextWindow::clear();

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
			scroll(gfx, gfx.getFontHeight());
			clearLine(gfx);
		} else {
			_cursY = y;
		}
	}

	// TODO: Make a generic BLT function in class Gfx.
	void TextWindow::scroll(Gfx &gfx, int y)
	{
		// Calculate buffer start position.
		uint8_t *buf = (uint8_t *)gfx.getFrameBuffer()
					 + (sizeof(PIXEL) * gfx.getRect().getX())
					 + (sizeof(PIXEL) * BSP_LCD_GetXSize() * gfx.getRect().getY());

		// Calculate number of bytes per scan line.
		unsigned lineBytes = sizeof(PIXEL) * BSP_LCD_GetXSize();
		unsigned windowLineBytes = sizeof(PIXEL) * gfx.getWidth();
		unsigned offset = lineBytes * y;
		unsigned nLines = gfx.getHeight() - y;

		for(int i = 0; i < nLines; i++) {
			memcpy(buf, buf + offset, windowLineBytes);
			buf += lineBytes;
		}
	}

	// Erase the current line of the screen.
	void TextWindow::clearLine(Gfx &gfx)
	{
		gfx.clearRect(Rect(0, _cursY, gfx.getWidth(), gfx.getFontHeight()));
	}

} // namespace Gui
