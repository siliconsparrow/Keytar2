/*
 * Button.h - Simple pressy button.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_BUTTON_H_
#define GUI_BUTTON_H_

#include "Obj.h"

namespace Gui
{
	class Button
		: public Obj
	{
	public:
		Button(const Rect &pos, const char *label, void (*onClick)(unsigned), void (*onRelease)(unsigned) = 0, unsigned tag = 0);
		virtual ~Button() { }

		virtual void onTouch(int x, int y);
		virtual void onRelease();
		virtual void draw(Gfx &gfx);

		void setText(const char *newText);

	private:
		const char  *_label;
		void       (*_onClick)(unsigned);
		void       (*_onRelease)(unsigned);
		unsigned     _tag;
		bool         _pressed;
	};

} // namespace Gui

#endif // GUI_BUTTON_H_
