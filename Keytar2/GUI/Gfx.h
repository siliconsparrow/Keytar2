/*
 * Gfx.h - Screen rendering functions for the Disco board.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_GFX_H_
#define GUI_GFX_H_

#include "Rect.h"
#include <stdint.h>

namespace Gui
{
	typedef uint32_t PIXEL;

	class Gfx
	{
	public:
		enum Colour {
			kColourBlack        = 0x00000000,
			kColourBlue         = 0xFF0000FF,
			kColourGreen        = 0xFF00FF00,
			kColourRed          = 0xFFFF0000,
			kColourCyan         = 0xFF00FFFF,
			kColourMagenta      = 0xFFFF00FF,
			kColourYellow       = 0xFFFFFF00,
			kColourLightBlue    = 0xFF8080FF,
			kColourLightGreen   = 0xFF80FF80,
			kColourLightRed     = 0xFFFF8080,
			kColourLightCyan    = 0xFF80FFFF,
			kColourLightMagenta = 0xFFFF80FF,
			kColourLightYellow  = 0xFFFFFF80,
			kColourDarkBlue     = 0xFF000080,
			kColourDarkGreen    = 0xFF008000,
			kColourDarkRed      = 0xFF800000,
			kColourDarkCyan     = 0xFF008080,
			kColourDarkMagenta  = 0xFF800080,
			kColourDarkYellow   = 0xFF808000,
			kColourGrey         = 0xFF808080,
			kColourDarkGrey     = 0xFF404040,
			kColourLightGrey    = 0xFFD3D3D3,
			kColourBrown        = 0xFFA52A2A,
			kColourOrange       = 0xFFFFA500,
			kColourTransparent  = 0xFF000000,
			kColourWhite        = 0xFFFFFFFF,

			// Default console colours.
			kColourText       = kColourGrey,
			kColourError      = kColourDarkRed,
			kColourBackground = kColourDarkBlue
		};

		enum Font {
			kFont8,
			kFont12,
			kFont16,
			kFont20,
			kFont24
		};

		Gfx(const Rect &r);

		void setColourFg(Colour col);
		void setColourBg(Colour col);
		void setFont(Font font);
		int  getFontWidth() const;
		int  getFontHeight() const;
		void drawChar(int x, int y, char c);
		void drawText(int x, int y, const char *text);
		void drawPixel(int x, int y, Colour col);
		void fillRect(const Rect &r);
		void clearRect(const Rect &r);
		void clear();

		const Rect &getRect()        const { return _r; }
		int         getWidth()       const { return _r.getWidth(); }
		int         getHeight()      const { return _r.getHeight(); }
		PIXEL      *getFrameBuffer() const;

	private:
		const Rect _r;
		Font       _f;
		Colour     _fg;
		Colour     _bg;

		friend class Gui;

		void restore();
		void adjustCoords(int &x, int &y) const;
		Rect adjustCoords(const Rect &r) const;
	};

} // namespace Gui

#endif // GUI_GFX_H_
