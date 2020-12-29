/*
 * Gfx.cpp - Screen rendering functions for the Disco board.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#include "Screen.h"
#include "Gfx.h"
#include "Gui.h"

namespace Gui
{
	Gfx::Gfx(const Rect &r)
		: _r(r)
		, _f(kFont12)
		, _fg(kColourText)
		, _bg(kColourBackground)
	{
	}

	// Restore the graphics context for this object.
	void Gfx::restore()
	{
		Screen::setTextColour((uint32_t)_fg);
		Screen::setBackColour((uint32_t)_bg);
		Screen::setFont((unsigned)_f);

	}

	void Gfx::setColourFg(Colour col)
	{
		Screen::setTextColour(_fg = col);
	}

	void Gfx::setColourBg(Colour col)
	{
		Screen::setBackColour(_bg = col);
	}

	void Gfx::setFont(Font font)
	{
		Screen::setFont((unsigned)(_f = font));
	}

	int Gfx::getFontWidth() const
	{
		return Screen::getFontWidth();
	}

	int Gfx::getFontHeight() const
	{
		return Screen::getFontHeight();
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
		Screen::drawChar(x, y, c);
	}

	// Simple text render (does not handle newlines etc).
	void Gfx::drawText(int x, int y, const char *text)
	{
		adjustCoords(x, y);
		int w = getFontWidth();
		while(*text != 0) {
			Screen::drawChar(x, y, *text++);
			x += w;
		}
	}

	void Gfx::drawPixel(int x, int y, Colour col)
	{
		adjustCoords(x, y);
		Screen::drawPixel(x, y, col);
	}

	// Fill the given rectangle with foreground colour.
	void Gfx::fillRect(const Rect &r)
	{
		Rect rr = adjustCoords(r);
		Screen::fillRect(rr.getX(), rr.getY(), rr.getWidth(), rr.getHeight());
	}

	// Fill the given rectangle with background colour.
	void Gfx::clearRect(const Rect &r)
	{
		Screen::setTextColour(_bg);
		fillRect(r);
		Screen::setTextColour(_fg);
	}

	// Clear the whole context.
	void Gfx::clear()
	{
		Screen::setTextColour(_bg);
		Screen::fillRect(_r.getX(), _r.getY(), _r.getWidth(), _r.getHeight());
		Screen::setTextColour(_fg);
	}

	void Gfx::blt(int dstX, int dstY, const Rect &src)
	{
		adjustCoords(dstX, dstY);
		Rect rrSrc = adjustCoords(src);

		Screen::blt(dstX, dstY, rrSrc.getX(), rrSrc.getY(), rrSrc.getWidth(), rrSrc.getHeight());
	}

} // namespace Gui
