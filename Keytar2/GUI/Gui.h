/*
 * Gui.h - Manage screen and UI events.
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
	class Gui
	{
	public:
		static Gui *instance();

		void tick();
		int  consoleWrite(const char *ptr, int len, Gfx::Colour col = Gfx::kColourText);
		void clearScreen();

		void add(Obj *obj);

	private:
		enum {
			kConsoleHeight = 72,
			kMaxObjects = 50,
			kBlinkTimeMs = 500
		};

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
