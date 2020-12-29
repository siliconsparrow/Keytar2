/*
 * Screen.cpp - Driver/wrapper for the Disco board's LCD touch screen.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

// I'm using the Discovery-provided functions a lot, eventually I'd like to replace
// them with tighter functions using the DMA2D blitter.

#include "stm32746g_discovery_ts.h"  // Touch screen
#include "stm32746g_discovery_lcd.h" // LCD
#include "chipAlloc.h"
#include "Screen.h"
#include <string.h>

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

	void Screen::init()
	{
		// Initialize the LCD.
		//_frameBuffer = (PIXEL *)allocSDRam(kLcdWidth * kLcdHeight * sizeof(PIXEL));
		//_frameBuffer = (PIXEL *)FRAMEBUFFER_ADDR;
		BSP_LCD_Init();
		BSP_LCD_LayerDefaultInit(0, (uint32_t)frameBuffer());
		BSP_LCD_SelectLayer(0);
		BSP_LCD_DisplayOn();

		// Initialize the touch screen.
		BSP_TS_Init(kWidth, kHeight);

		// Clear the screen.
		BSP_LCD_Clear(LCD_COLOR_DARKBLUE);
	}

	// Hopefully won't need this much if at all.
	PIXEL *Screen::frameBuffer()
	{
		return (PIXEL *)FRAMEBUFFER_ADDR;
	}

	// Set the current drawing colour for text, fillRect, draw line etc.
	void Screen::setTextColour(uint32_t col)
	{
		BSP_LCD_SetTextColor(col);
	}

	// Set the current background colour.
	void Screen::setBackColour(uint32_t col)
	{
		BSP_LCD_SetBackColor((uint32_t)col);
	}

	// Set the current font.
	void Screen::setFont(unsigned fontIndex)
	{
		if(fontIndex < kNumFonts) {
			BSP_LCD_SetFont(g_consoleFonts[fontIndex]);
		}
	}

	// Return the cell width of the current font.
	int Screen::getFontWidth()
	{
		return BSP_LCD_GetFont()->Width;
	}

	// Return the cell height of the current font.
	int Screen::getFontHeight()
	{
		return BSP_LCD_GetFont()->Height;
	}

	// Render a single character.
	void Screen::drawChar(int x, int y, char c)
	{
		BSP_LCD_DisplayChar(x, y, c);
	}

	void Screen::drawPixel(int x, int y, uint32_t colour)
	{
		BSP_LCD_DrawPixel(x, y, colour);
	}

	void Screen::fillRect(int x, int y, int w, int h)
	{
		BSP_LCD_FillRect(x, y, w, h);
	}

	// Block image transfer - Move a rectangle of the screen somewhere else.
	// This is not very well tested.
	// TODO: Use DMA2D to make this faster.
	void Screen::blt(int dstX, int dstY, int srcX, int srcY, int width, int height)
	{
		// Calculate buffer start position.
		uint8_t *buf = (uint8_t *)frameBuffer()
					 + (sizeof(PIXEL) * srcX)
					 + (sizeof(PIXEL) * BSP_LCD_GetXSize() * srcY);

		// Calculate number of bytes per scan line.
		int lineBytes = sizeof(PIXEL) * BSP_LCD_GetXSize();
		int windowLineBytes = sizeof(PIXEL) * width;
		int offset = (lineBytes * (dstY - srcY)) + (sizeof(PIXEL) * (dstX - srcX));

		if(offset < 0) {
			for(int i = 0; i < height; i++) {
				memcpy(buf + offset, buf, windowLineBytes);
				buf += lineBytes;
			}
		} else {
			buf += lineBytes * (height - 1);
			for(int i = height - 1; i >= 0; i++) {
				memmove(buf + offset, buf, windowLineBytes);
				buf -= lineBytes;
			}
		}
	}

	// Returns true if there is a touch (and puts the position in x,y).
	// Returns false if no touch is happening.
	// TODO: Lots more touch features are available like multi-touch, pressure and gestures.
	bool Screen::getSingleTouch(int &x, int &y)
	{
		TS_StateTypeDef ts;
		BSP_TS_GetState(&ts);
		if(ts.touchDetected > 0) {
			x = ts.touchX[0];
			y = ts.touchY[0];
			return true;
		}

		return false;
	}

} // namespace Gui
