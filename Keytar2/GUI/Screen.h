/*
 * Screen.h - Driver/wrapper for the Disco board's LCD touch screen.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_SCREEN_H_
#define GUI_SCREEN_H_

#include <stdint.h>

namespace Gui
{
	typedef uint32_t PIXEL;

	class Screen
	{
	public:
		enum Font {
			kFont8,
			kFont12,
			kFont16,
			kFont20,
			kFont24,

			kNumFonts
		};

		static void   init();
		static PIXEL *frameBuffer();
		static int    width()  { return kWidth; }
		static int    height() { return kHeight; }

		static void   setTextColour(uint32_t col);
		static void   setBackColour(uint32_t col);
		static void   setFont(unsigned fontIndex);

		static int    getFontWidth();
		static int    getFontHeight();

		static void   drawChar(int x, int y, char c);
		static void   drawPixel(int x, int y, uint32_t colour);
		static void   fillRect(int x, int y, int w, int h);

		static void   blt(int dstX, int dstY, int srcX, int srcY, int width, int height);

		static bool   getSingleTouch(int &x, int &y);

	private:
		enum {
			kWidth = 480,
			kHeight = 272,
		};


	};

} // namespace Gui

#endif // GUI_SCREEN_H_
