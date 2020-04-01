/*
 * Console.cpp
 *
 *  Created on: 22 Mar 2020
 *      Author: adam
 */

#include "Console.h"
#include "stm32746g_discovery_lcd.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>

static Console g_console;

static sFONT *g_consoleFonts[] = {
	&Font8,
	&Font12,
	&Font16,
	&Font20,
	&Font24
};

Console::Console()
	: _cursX(0)
	, _cursY(0)
	, _charWidth(0)
	, _charHeight(0)
{
}

void Console::init()
{
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_DisplayOn();

    setFont(kFont12);
    clearScreen();
}

// Clear screen
void Console::clearScreen()
{
	BSP_LCD_SetBackColor(kColourBackground);
	BSP_LCD_Clear(kColourBackground);
}

void Console::setColour(Colour col)
{
	::BSP_LCD_SetTextColor((uint32_t)col);
}

void Console::drawPixel(int x, int y, Colour col)
{
	BSP_LCD_DrawPixel(x, y, col);
}

void Console::fillRect(int x, int y, int width, int height)
{
	::BSP_LCD_FillRect(x, y, width, height);
}

void Console::setFont(Font fnt)
{
	sFONT *pFont = g_consoleFonts[fnt];

    BSP_LCD_SetFont(pFont);
    g_console._charWidth = pFont->Width;
    g_console._charHeight = pFont->Height;
}

#define ABS(x) ((x)<0?(-(x)):(x))

void Console::scroll(int x, int y)
{
	// Get screen size.
	int scrX = (int)BSP_LCD_GetXSize();
	int scrY = (int)BSP_LCD_GetYSize();

	// Calculate offset (assume 32bpp).
	int offset = 4 * (y * scrX + x);

	// Calculate number of pixels to move.
	unsigned nBytes = sizeof(uint32_t) * ((scrX - ABS(x)) * (scrY - ABS(y)));

	// Move the pixels.
	if(offset < 0)
		memmove((uint8_t *)LCD_FB_START_ADDRESS, (uint8_t *)(LCD_FB_START_ADDRESS - offset), nBytes);
	else
		memmove((uint8_t *)(LCD_FB_START_ADDRESS + offset), (uint8_t *)LCD_FB_START_ADDRESS, nBytes);
}

void Console::setCursor(int x, int y)
{
	blinkCursor(false);
	g_console._cursX = x;
	g_console._cursY = y;
}

void Console::cursNextChar()
{
	_cursX += _charWidth;

	if(_cursX + _charWidth > BSP_LCD_GetXSize()) {
		cursNextLine();
	}
}

void Console::cursNextLine()
{
	_cursX = 0;
	uint16_t y = _cursY + _charHeight;
	if(y + _charHeight > BSP_LCD_GetYSize()) {
		scroll(0, -_charHeight);
		clearLine();
	} else {
		_cursY = y;
	}
}

// Erase the current line of the screen.
void Console::clearLine()
{
	uint32_t col = BSP_LCD_GetTextColor();
	BSP_LCD_SetTextColor(kColourBackground);
	BSP_LCD_FillRect(0, _cursY, BSP_LCD_GetXSize(), _charHeight);
	BSP_LCD_SetTextColor(col);
}

void Console::blinkCursor(bool on)
{
	BSP_LCD_DisplayChar(g_console._cursX, g_console._cursY, on ? '_' : ' ');
}

// Write some characters to the console.
void Console::write(const char *ptr, int len, Colour col)
{
    BSP_LCD_SetTextColor(col);

    for(int i = 0; i < len; i++) {
    	char c = ptr[i];

		if(c >= 32 && c < 127) {
			// Regular character.
			BSP_LCD_DisplayChar(_cursX, _cursY, c);
			cursNextChar();
		} else switch(c) {
		// Special characters.
		case '\n':
			cursNextLine();
			break;

		case '\f':
			clearScreen();
			_cursX = _cursY = 0;
		}
    }
}

// Write to disc or console.
extern "C" int _write(int fd, char *ptr, int len)
{
	if(fd == STDOUT_FILENO) {
		g_console.write(ptr, len);
		return len;
	}
	else if(fd == STDERR_FILENO) {
		g_console.write(ptr, len, Console::kColourError);
		return len;
	}
	errno = EBADF;
	return -1;
}
