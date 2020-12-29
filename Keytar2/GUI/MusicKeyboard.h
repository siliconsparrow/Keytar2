/*
 * MusicKeyboard.h - Play notes on screen.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#ifndef GUI_MUSICKEYBOARD_H_
#define GUI_MUSICKEYBOARD_H_

#include "Obj.h"

namespace Gui
{

	class MusicKeyboard
	    : public Obj
	{
	public:
		MusicKeyboard(const Rect &r, void (*fnNoteOn)(uint8_t) = 0, void (*fnNoteOff)(uint8_t) = 0);
		virtual ~MusicKeyboard();

		virtual void onTouch(int x, int y);
		virtual void onRelease();
		virtual void draw(Gfx &gfx);

		void noteOn(uint8_t noteNum);
		void noteOff(uint8_t noteNum);

	private:
		enum {
			kNotesPerOctave = 12,
			kNoteStart = 60, // C4
			kNoteEnd = kNoteStart + kNotesPerOctave,
			kTotalNotes = kNoteEnd - kNoteStart + 1,
			kNoNote = 255,
		};

		struct KBKey {
			uint8_t noteNum;
			Rect    r;
		};

		uint8_t   _currentNote;
		void    (*_fnNoteOn)(uint8_t);
		void    (*_fnNoteOff)(uint8_t);
		KBKey    *_rWhite;
		KBKey    *_rBlack;
		unsigned  _nWhite;
		unsigned  _nBlack;
		uint8_t   _keyMap[16];

		void notePress(uint8_t noteNum);
		bool isPressed(uint8_t noteNum);
	};

} // namespace Gui

#endif // GUI_MUSICKEYBOARD_H_
