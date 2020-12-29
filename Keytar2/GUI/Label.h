/*
 * Label.h - Simple text label.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_LABEL_H_
#define GUI_LABEL_H_

#include "Obj.h"

namespace Gui
{
	class Gfx;
	class Label
		: public Obj
	{
	public:
		enum {
			kMaxText = 100
		};

		Label(const Rect &r, const char *text);
		virtual ~Label() { }

		void setText(const char *text);

		virtual void draw(Gfx &gfx);

	private:
		char _text[kMaxText];
	};

} // namespace Gui

#endif /* GUI_LABEL_H_ */
