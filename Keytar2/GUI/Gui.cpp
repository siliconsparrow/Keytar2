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
#include "chipAlloc.h"
//#include <string.h>
//#include <stdio.h>

namespace Gui
{
#ifdef OLD
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


#endif // OLD

	// *******************************************
	// ** Manage screen and UI events.

	// Find and/or create the main GUI instance.
	Gui *Gui::instance()
	{
		static Gui *g_gui = 0;

		if(g_gui == 0) {
			g_gui = new Gui();
		}

		return g_gui;
	}

	Gui::Gui()
		: _frameBuffer((PIXEL*)FRAMEBUFFER_ADDR)
		, _screen(0, 0, kLcdWidth, kLcdHeight)
		, _console(Rect(0, 200, 480, 72)) // Set up a console.
		, _nObj(0)
		, _touch(false)
		, _touchObj(0)
		, _cursBlink(false)
		, _tLastBlink(0)
	{
		// Initialize the LCD.
		//_frameBuffer = (PIXEL *)allocSDRam(kLcdWidth * kLcdHeight * sizeof(PIXEL));
		//_frameBuffer = (PIXEL *)FRAMEBUFFER_ADDR;
		BSP_LCD_Init();
		BSP_LCD_LayerDefaultInit(0, (uint32_t)_frameBuffer);
		BSP_LCD_SelectLayer(0);
		BSP_LCD_DisplayOn();

		// Initialize the touch screen.
		BSP_TS_Init(kLcdWidth, kLcdHeight);

		// Clear the screen.
		clearScreen();

		// Set up text console.
		add(&_console);
		_console._gfx._bg = Gfx::kColourBlack;
		_console._gfx.clear();
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
	    	perfEnter(pidGui);
			if(!_touch) {
				int x = ts.touchX[0];
				int y = ts.touchY[0];
				_touchObj = objectAt(x,y);

				if(_touchObj != 0) {
					x -= _touchObj->_gfx.getRect().getX();
					y -= _touchObj->_gfx.getRect().getY();
					_touchObj->onTouch(x, y);
				}
				_touch = true;
			}
			perfLeave();
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

	void Gui::clearScreen()
	{
    	perfEnter(pidGui);
		BSP_LCD_SetBackColor(Gfx::kColourBackground);
		BSP_LCD_Clear(Gfx::kColourBackground);
		perfLeave();
	}

	// Write some characters to the console.
	int Gui::consoleWrite(const char *ptr, int len, Gfx::Colour col)
	{
    	perfEnter(pidGui);
		_console._gfx.restore();
		int result = _console.write(_console._gfx, ptr, len, col);
		perfLeave();
		return result;
	}

	// Add a new GUI object to the screen.
	void Gui::add(Obj *obj)
	{
		if(_nObj < kMaxObjects) {
			_obj[_nObj++] = obj;
		}
	}

#ifdef OLD
	// Draw all objects.
	void Gui::redrawAll()
	{
		for(unsigned i = 0; i < _nObj; i++) {
			draw(_obj[i]);
		}
	}
#endif // OLD
	void Gui::draw(Obj *obj)
	{
		obj->_gfx.restore();
		obj->draw(obj->_gfx);
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

} // namespace Gui

