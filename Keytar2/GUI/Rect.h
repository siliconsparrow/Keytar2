/*
 * Rect.h - Define rectangles for the screen.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_RECT_H_
#define GUI_RECT_H_

namespace Gui
{
	class Rect
	{
	public:
		Rect()
			: _x(0), _y(0), _w(0), _h(0)
		{ }

		Rect(int x, int y, int width, int height)
			: _x(x), _y(y), _w(width), _h(height)
		{ }

		int getX()      const { return _x; }
		int getY()      const { return _y; }
		int getWidth()  const { return _w; }
		int getHeight() const { return _h; }

		bool contains(int x, int y) const { return x >= _x && x < (_x + _w) && y >= _y && y < (_y + _h); }

	private:
		int _x;
		int _y;
		int _w;
		int _h;
	};

} // namespace Gui

#endif // GUI_RECT_H_
