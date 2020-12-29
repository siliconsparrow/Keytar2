/*
 * TextWindow.h - Scrolling text window.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_TEXTWINDOW_H_
#define GUI_TEXTWINDOW_H_

#include "Obj.h"
#include <stdint.h>

namespace Gui {

	class TextWindow
		: public Obj
	{
	public:
		TextWindow(const Rect &r);
		virtual ~TextWindow() { }

		virtual void draw(Gfx &gfx);

		int  write(Gfx &gfx, const char *ptr, int len, Gfx::Colour col);
		void setCursor(Gfx &gfx, int x, int y);
		void blinkCursor(Gfx &gfx, bool on);

	private:
		uint16_t _cursX;
		uint16_t _cursY;

		void cursNextChar(Gfx &gfx);
		void cursNextLine(Gfx &gfx);
		void scroll(Gfx &gfx);
	};


} /* namespace Gui */

#endif /* GUI_TEXTWINDOW_H_ */
