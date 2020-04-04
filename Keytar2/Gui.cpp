/*
 * Gui.cpp
 *
 *  Created on: 29 Mar 2020
 *      Author: adam
 */

#include "Gui.h"
#include "stm32746g_discovery_ts.h"  // Touch screen
#include "stm32746g_discovery_lcd.h" // LCD
#include "PerfMon.h"
#include <string.h>
#include <stdio.h>

namespace Gui
{
	// Our singleton instance.
	static Gui *g_gui;

	// Map font IDs to actual font objects.
	static sFONT *g_consoleFonts[] = {
		&Font8,
		&Font12,
		&Font16,
		&Font20,
		&Font24
	};


	// *******************************************
	// ** Graphical drawing context.

	Gfx::Gfx(const Rect &r)
		: _r(r)
		, _f(kFont12)
		, _fg(kColourText)
		, _bg(kColourBackground)
	{
	}

	void Gfx::restore()
	{
		BSP_LCD_SetTextColor((uint32_t)_fg);
		BSP_LCD_SetBackColor((uint32_t)_bg);
	    BSP_LCD_SetFont(g_consoleFonts[_f]);
	}

	void Gfx::setColourFg(Colour col)
	{
		_fg = col;
		BSP_LCD_SetTextColor((uint32_t)col);
	}

	void Gfx::setColourBg(Colour col)
	{
		_bg = col;
		BSP_LCD_SetBackColor((uint32_t)col);
	}

	void Gfx::setFont(Font font)
	{
		_f = font;
	    BSP_LCD_SetFont(g_consoleFonts[font]);
	}

	int Gfx::getFontWidth() const
	{
		return BSP_LCD_GetFont()->Width;
	}

	int Gfx::getFontHeight() const
	{
		return BSP_LCD_GetFont()->Height;
	}

	// Convert from global coordinates to local coordinates.
	void Gfx::adjustCoords(int &x, int &y) const
	{
		x += getRect().getX();
		y += getRect().getY();
	}

	// Convert from global coordinates to local coordinates and clip.
	Rect Gfx::adjustCoords(const Rect &r) const
	{
		int w = r.getWidth();
		if(w > getRect().getWidth())
			w = getRect().getWidth();

		int h = r.getHeight();
		if(h > getRect().getHeight())
			h = getRect().getHeight();

		return Rect(r.getX() + getRect().getX(), r.getY() + getRect().getY(), w, h);
	}

	// Draw a single character.
	void Gfx::drawChar(int x, int y, char c)
	{
		adjustCoords(x, y);
		BSP_LCD_DisplayChar(x, y, c);
	}

	// Simple text render (does not handle newlines etc).
	void Gfx::drawText(int x, int y, const char *text)
	{
		adjustCoords(x, y);
		int w = getFontWidth();
		while(*text != 0) {
			BSP_LCD_DisplayChar(x, y, *text++);
			x += w;
		}
	}

	void Gfx::drawPixel(int x, int y, Colour col)
	{
		adjustCoords(x, y);
		BSP_LCD_DrawPixel(x, y, col);
	}

	// Fill the given rectangle with foreground colour.
	void Gfx::fillRect(const Rect &r)
	{
		Rect rr = adjustCoords(r);
		BSP_LCD_FillRect(rr.getX(), rr.getY(), rr.getWidth(), rr.getHeight());
	}

	// Fill the given rectangle with background colour.
	void Gfx::clearRect(const Rect &r)
	{
		Rect rr = adjustCoords(r);
		BSP_LCD_SetTextColor(_bg);
		fillRect(r);
		BSP_LCD_SetTextColor(_fg);
	}

	// Clear the whole context.
	void Gfx::clear()
	{
		BSP_LCD_SetTextColor(_bg);
		BSP_LCD_FillRect(_r.getX(), _r.getY(), _r.getWidth(), _r.getHeight());
		BSP_LCD_SetTextColor(_fg);
	}


	// *******************************************
	// ** Base class for all GUI objects.

	Obj::Obj(const Rect &r)
		: _gfx(r)
		, _dirty(true)
	{
	}

	Obj::~Obj()
	{
	}


	// *******************************************
	// ** Simple VU meter.

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


	// *******************************************
	// ** Simple pressy button.

	Button::Button(const Rect &pos, const char *label, void (onClick)())
		: Obj(pos)
		, _label(label)
		, _onClick(onClick)
		, _pressed(false)
	{
	}

	void Button::onTouch(int x, int y)
	{
		_pressed = true;
		dirty();

		_onClick();
	}

	void Button::onRelease()
	{
		_pressed = false;
		dirty();
	}

	void Button::draw(Gfx &gfx)
	{
		if(_pressed)
		{
			gfx.setColourBg(Gfx::kColourOrange);
		} else {
			gfx.setColourBg(Gfx::kColourGrey);
		}
		gfx.clear();
		gfx.setColourFg(Gfx::kColourBlack);
		gfx.setFont(Gfx::kFont24);
		gfx.drawText(5, 5, _label);
	}


	// *******************************************
	// ** Scrolling text window.

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

	void TextWindow::scroll(Gfx &gfx, int y)
	{
		// Calculate buffer start position.
		uint8_t *buf = (uint8_t *)LCD_FB_START_ADDRESS
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


	// *******************************************
	// ** Simple text label.

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

	// *******************************************
	// ** Manage screen and UI events.

	Gui::Gui()
		: _screen(0, 0, kLcdWidth, kLcdHeight)
		, _console(Rect(0, 128, 480, 144)) // Set up a console.
		, _nObj(0)
		, _touch(false)
		, _touchObj(0)
		, _cursBlink(false)
		, _tLastBlink(0)
	{
		g_gui = this;

		// Initialize the LCD.
		BSP_LCD_Init();
		BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
		BSP_LCD_SelectLayer(0);
		BSP_LCD_DisplayOn();

		// Initialize the touch screen.
		BSP_TS_Init(kLcdWidth, kLcdHeight);

		// Clear the screen.
		clearScreen();

		add(&_console);
		_console._gfx._bg = Gfx::kColourBlack;
		_console._gfx.clear();
	}

	// Find the main GUI instance.
	Gui *Gui::instance()
	{
		return g_gui;
	}

	// Add a new GUI object to the screen.
	void Gui::add(Obj *obj)
	{
		if(_nObj < kMaxObjects) {
			_obj[_nObj++] = obj;
		}
	}

	// Draw all objects.
	void Gui::redrawAll()
	{
		for(unsigned i = 0; i < _nObj; i++) {
			draw(_obj[i]);
		}
	}

	void Gui::draw(Obj *obj)
	{
		obj->_gfx.restore();
		obj->draw(obj->_gfx);
	}

	// Check for events.
	// TODO: This should be interrupt-driven.
	void Gui::tick()
	{
		// Blink the cursor.
		// TODO: Once again, interrupts would be good for this.
		uint32_t tNow = HAL_GetTick();
		uint32_t dt = tNow - _tLastBlink;
		if(dt >= kBlinkTimeMs) {
	    	perfEnter(pidGui);
			_cursBlink = !_cursBlink;
			_console._gfx.restore();
			_console.blinkCursor(_console._gfx, _cursBlink);
			_tLastBlink = tNow;
	    	perfLeave();
		}

		// Redraw anything that needs to be redrawn.
		for(unsigned i = 0; i < _nObj; i++) {
			Obj *obj = _obj[i];
			if(obj->checkDirty()) {
		    	perfEnter(pidGui);
				draw(obj);
		    	perfLeave();
			}
		}

		// Check for any new touch events.
		// TODO: Use interrupts for this.
		TS_StateTypeDef ts;
		BSP_TS_GetState(&ts);
		if(ts.touchDetected > 0) {
			if(!_touch) {
				perfEnter(pidGui);
				int x = ts.touchX[0];
				int y = ts.touchY[0];
				_touchObj = objectAt(x,y);

				//printf("Touch at %d,%d\n", x, y);

				if(_touchObj != 0) {
					x -= _touchObj->_gfx.getRect().getX();
					y -= _touchObj->_gfx.getRect().getY();
					_touchObj->onTouch(x, y);
				}
				_touch = true;
				perfLeave();
			}
		} else {
			if(_touchObj != 0) {
				perfEnter(pidGui);
				_touchObj->onRelease();
				_touchObj = 0;
				perfLeave();
			}
			_touch = false;
		}
	}


	// Return the object at the given location or 0 if none.
	Obj *Gui::objectAt(int x, int y)
	{
		Obj *result = 0;

		for(unsigned i = 0; i < _nObj; i++) {
			Obj *obj = _obj[i];
			if(obj->_gfx._r.contains(x, y)) {
				result = obj;
			}
		}

		return result;
	}

	void Gui::clearScreen()
	{
		BSP_LCD_SetBackColor(Gfx::kColourBackground);
		BSP_LCD_Clear(Gfx::kColourBackground);
	}

	// Write some characters to the console.
	int Gui::consoleWrite(const char *ptr, int len, Gfx::Colour col)
	{
		_console._gfx.restore();
		return _console.write(_console._gfx, ptr, len, col);
	}

} // namespace Gui

