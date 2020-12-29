/*
 * Gui.cpp - Manage screen and UI events.
 *
 *  Created on: 29 Mar 2020
 *      Author: adam
 */

#include "Gui.h"
#include "Screen.h"
#include "PerfMon.h"
#include "stm32f7xx_hal.h"

namespace Gui
{
	// Find and/or create the main GUI instance.
	Gui *Gui::instance()
	{
		static Gui *g_gui = 0;

		if(g_gui == 0) {
			g_gui = new Gui();
		}

		return g_gui;
	}

	// Set up the screen and GUI.
	Gui::Gui()
		: _screen(0, 0, Screen::width(), Screen::height())
		, _console(Rect(0, Screen::height() - kConsoleHeight, Screen::width(), kConsoleHeight)) // Set up a console.
		, _nObj(0)
		, _touch(false)
		, _touchObj(0)
		, _cursBlink(false)
		, _tLastBlink(0)
	{
		// Set up screen.
		Screen::init();

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
		int x;
		int y;
		if(Screen::getSingleTouch(x, y)) {
	    	perfEnter(pidGui);
			if(!_touch) {
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

	// Render an object onto the screen.
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

