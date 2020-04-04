/*
 * Gui.h
 *
 *  Created on: 29 Mar 2020
 *      Author: adam
 */

#ifndef GUI_H_
#define GUI_H_

#include <stdint.h>

namespace Gui
{
	typedef uint32_t PIXEL;

	// *******************************************
	// ** We use rectangles a lot.

	class Rect
	{
	public:
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


	// *******************************************
	// ** Graphical drawing context.

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

		const Rect &getRect()   const { return _r; }
		int         getWidth()  const { return _r.getWidth(); }
		int         getHeight() const { return _r.getHeight(); }

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


	// *******************************************
	// ** Base class for all GUI objects.

	class Obj
	{
	public:
		Obj(const Rect &r);
		virtual ~Obj();

		virtual void draw(Gfx &gfx) = 0;

		virtual void onTouch(int x, int y) { }
		virtual void onRelease() { }

//		int getWidth()  const { return _gfx.getRect().getWidth(); }
//		int getHeight() const { return _gfx.getRect().getHeight(); }

	protected:
		void dirty() { _dirty = true; }

	private:
		friend class Gui;

		Gfx  _gfx;
		bool _dirty;

		bool checkDirty() { bool r = _dirty; _dirty = false; return r; }
	};


	// *******************************************
	// ** Simple VU meter.

	class Meter
	    : public Obj
	{
	public:
		Meter(const Rect &pos);
		virtual ~Meter() { }

		virtual void draw(Gfx &gfx);

		void setValue(unsigned v);

	private:
		unsigned _value;
	};


	// *******************************************
	// ** Simple pressy button.

	class Button
		: public Obj
	{
	public:
		Button(const Rect &pos, const char *label, void (onClick)());
		virtual ~Button() { }

		virtual void onTouch(int x, int y);
		virtual void onRelease();
		virtual void draw(Gfx &gfx);

	private:
		const char  *_label;
		void       (*_onClick)();
		bool         _pressed;
	};


	// *******************************************
	// ** Scrolling text window.

	class TextWindow
		: public Obj
	{
	public:
		TextWindow(const Rect &r);
		virtual ~TextWindow() { }

		virtual void draw(Gfx &gfx);

		int  write(Gfx &gfx, const char *ptr, int len, Gfx::Colour col);
		void setCursor(Gfx &gfx, int x, int y);
		void blinkCursor(Gfx &gfx, bool on);

	private:
		uint16_t _cursX;
		uint16_t _cursY;

		void cursNextChar(Gfx &gfx);
		void cursNextLine(Gfx &gfx);
		void scroll(Gfx &gfx, int y);
		void clearLine(Gfx &gfx);
	};

	// *******************************************
	// ** Simple text label.

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

	// *******************************************
	// ** Manage screen and UI events.

	class Gui
	{
	public:
		Gui();

		static Gui *instance();

		void add(Obj *obj);
		void tick();
		void clearScreen();
		int  consoleWrite(const char *ptr, int len, Gfx::Colour col = Gfx::kColourText);

		void redrawAll();

	private:
		enum {
			kLcdWidth = 480,
			kLcdHeight = 272,
			kMaxObjects = 50,
			kBlinkTimeMs = 500
		};

		Rect        _screen;
		TextWindow  _console;
		unsigned    _nObj;
		Obj        *_obj[kMaxObjects];
		bool        _touch;
		Obj        *_touchObj;
		bool        _cursBlink;
		uint32_t    _tLastBlink;

		Obj  *objectAt(int x, int y);
		void  draw(Obj *obj);
	};

} // namespace Gui

#endif /* GUI_H_ */
