/*
 * Gui.h
 *
 *  Created on: 29 Mar 2020
 *      Author: adam
 */

#ifndef GUI_H_
#define GUI_H_

#include "Rect.h"
#include "Gfx.h"
#include "TextWindow.h"
#include <stdint.h>

namespace Gui
{
#ifdef OLD

	// *******************************************
	// ** Simple VU meter.

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


#endif // OLD

	// *******************************************
	// ** Manage screen and UI events.

	class Gui
	{
	public:
		static Gui *instance();

		void tick();
		int  consoleWrite(const char *ptr, int len, Gfx::Colour col = Gfx::kColourText);
		void clearScreen();

		void add(Obj *obj);

#ifdef OLD
		void redrawAll();
#endif // OLD

		PIXEL *getFrameBuffer() const { return _frameBuffer; }

	private:
		enum {
			kLcdWidth = 480,
			kLcdHeight = 272,
			kMaxObjects = 50,
			kBlinkTimeMs = 500
		};

		PIXEL      *_frameBuffer;
		Rect        _screen;
		TextWindow  _console;
		unsigned    _nObj;
		Obj        *_obj[kMaxObjects];
		bool        _touch;
		Obj        *_touchObj;
		bool        _cursBlink;
		uint32_t    _tLastBlink;

		Gui();

		Obj  *objectAt(int x, int y);
		void  draw(Obj *obj);
	};

} // namespace Gui

#endif // GUI_H_
