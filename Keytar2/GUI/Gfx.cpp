/*
 * Gfx.cpp - Screen rendering functions for the Disco board.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

// TODO: Make sure these are using the DMA2D blitter as much as possible.

#include "stm32746g_discovery_lcd.h"
#include "Gfx.h"
#include "Gui.h"

namespace Gui
{
	// Map font IDs to actual font objects.
	static sFONT *g_consoleFonts[] = {
		&Font8,
		&Font12,
		&Font16,
		&Font20,
		&Font24
	};

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
		BSP_LCD_SetTextColor((uint32_t)_fg);
		BSP_LCD_SetBackColor((uint32_t)_bg);
		BSP_LCD_SetFont(g_consoleFonts[_f]);
	}

	void Gfx::setColourFg(Colour col)
	{
		_fg = col;
		BSP_LCD_SetTextColor((uint32_t)col);
	}

	void Gfx::setColourBg(Colour col)
	{
		_bg = col;
		BSP_LCD_SetBackColor((uint32_t)col);
	}

	void Gfx::setFont(Font font)
	{
		_f = font;
		BSP_LCD_SetFont(g_consoleFonts[font]);
	}

	int Gfx::getFontWidth() const
	{
		return BSP_LCD_GetFont()->Width;
	}

	int Gfx::getFontHeight() const
	{
		return BSP_LCD_GetFont()->Height;
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
		BSP_LCD_DisplayChar(x, y, c);
	}

	// Simple text render (does not handle newlines etc).
	void Gfx::drawText(int x, int y, const char *text)
	{
		adjustCoords(x, y);
		int w = getFontWidth();
		while(*text != 0) {
			BSP_LCD_DisplayChar(x, y, *text++);
			x += w;
		}
	}

	void Gfx::drawPixel(int x, int y, Colour col)
	{
		adjustCoords(x, y);
		BSP_LCD_DrawPixel(x, y, col);
	}

	// Fill the given rectangle with foreground colour.
	void Gfx::fillRect(const Rect &r)
	{
		Rect rr = adjustCoords(r);
		BSP_LCD_FillRect(rr.getX(), rr.getY(), rr.getWidth(), rr.getHeight());
	}

	// Fill the given rectangle with background colour.
	void Gfx::clearRect(const Rect &r)
	{
		Rect rr = adjustCoords(r);
		BSP_LCD_SetTextColor(_bg);
		fillRect(r);
		BSP_LCD_SetTextColor(_fg);
	}

	// Clear the whole context.
	void Gfx::clear()
	{
		BSP_LCD_SetTextColor(_bg);
		BSP_LCD_FillRect(_r.getX(), _r.getY(), _r.getWidth(), _r.getHeight());
		BSP_LCD_SetTextColor(_fg);
	}

	PIXEL *Gfx::getFrameBuffer() const
	{
		return Gui::instance()->getFrameBuffer();
	}

} // namespace Gui
