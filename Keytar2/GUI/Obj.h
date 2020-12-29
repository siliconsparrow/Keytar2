/*
 * Obj.h - Base class for all GUI objects.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_OBJ_H_
#define GUI_OBJ_H_

#include "Rect.h"
#include "Gfx.h"

namespace Gui
{
	class Obj
	{
	public:
		Obj(const Rect &r)
			: _gfx(r), _dirty(true)
		{ }

		virtual ~Obj() { }

		virtual void draw(Gfx &gfx) = 0;

		virtual void onTouch(int x, int y) { }
		virtual void onRelease() { }


	protected:
		void dirty() { _dirty = true; }

		int getWidth()  const { return _gfx.getRect().getWidth(); }
		int getHeight() const { return _gfx.getRect().getHeight(); }

	private:
		friend class Gui;

		Gfx  _gfx;
		bool _dirty;

		bool checkDirty() { bool r = _dirty; _dirty = false; return r; }
	};

} // namespace Gui

#endif // GUI_OBJ_H_
