/*
 * Console.h
 *
 *  Created on: 22 Mar 2020
 *      Author: adam
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>

class Console
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

	Console();

	static void init();
	static void setFont(Font fnt);
	static void setColour(Colour col);
	static void clearScreen();
	static void blinkCursor(bool on);
	static void setCursor(int x, int y);

	void write(const char *ptr, int len, Colour col = kColourText);

	static void drawPixel(int x, int y, Colour col = kColourWhite);
	static void fillRect(int x, int y, int width, int height);

private:
	uint16_t _cursX;
	uint16_t _cursY;
	uint16_t _charWidth;
	uint16_t _charHeight;

	void scroll(int x, int y);
	void cursNextChar();
	void cursNextLine();
	void clearLine();
};

#endif /* CONSOLE_H_ */
