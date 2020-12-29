/*
 * MusicKeyboard.cpp - Play notes on screen.
 *
 *  Created on: 29 Dec 2020
 *      Author: adam
 */

#include "MusicKeyboard.h"
#include <string.h>

#define KB_BWMAP 0x54A // Bitmap of black and white notes (0=white, 1=black).

namespace Gui
{
	MusicKeyboard::MusicKeyboard(const Rect &r, void (*fnNoteOn)(uint8_t), void (*fnNoteOff)(uint8_t))
		: Obj(r)
		, _currentNote(kNoNote)
		, _fnNoteOn(fnNoteOn)
		, _fnNoteOff(fnNoteOff)
	{
		memset(_keyMap, 0, sizeof(_keyMap));

		// Count the number of white notes to display (determines the scaling).
		unsigned shift = kNoteStart % kNotesPerOctave;
		unsigned bw = KB_BWMAP >> shift;
		_nWhite = 0;
		for(unsigned n = 0; n < kTotalNotes; n++) {
			if(0 == (bw & 1)) {
				_nWhite++;
			}

			if(++shift >= kNotesPerOctave) {
				shift = 0;
				bw = KB_BWMAP;
			} else {
				bw >>= 1;
			}
		}

		// Pre-calculate the screen rectangles for each key.
		_rWhite = new KBKey[_nWhite];
		_rBlack = new KBKey[_nBlack = kTotalNotes - _nWhite];
		shift = kNoteStart % kNotesPerOctave;
		bw = KB_BWMAP >> shift;
		unsigned ixWhite = 0;
		unsigned ixBlack = 0;
		unsigned wnw = (getWidth() / _nWhite) - 2;
		unsigned wnx = 0;
		unsigned bndx = (3 * wnw) / 4;
		unsigned bnw = wnw / 2;
		unsigned bnh = getHeight() / 2;
		uint8_t nn = kNoteStart;
		for(unsigned n = 0; n < kTotalNotes; n++) {
			if(0 == (bw & 1)) {
				// White note.
				wnx = (getWidth() * ixWhite) / _nWhite;
				KBKey &k = _rWhite[ixWhite++];
				k.noteNum = nn++;
				k.r = Rect(wnx, 0, wnw, getHeight());
			} else {
				// Black note.
				KBKey &k = _rBlack[ixBlack++];
				k.noteNum = nn++;
				k.r = Rect(wnx + bndx, 0, bnw, bnh);
			}

			if(++shift >= kNotesPerOctave) {
				shift = 0;
				bw = KB_BWMAP;
			} else {
				bw >>= 1;
			}
		}
	}

	MusicKeyboard::~MusicKeyboard()
	{
		delete[] _rBlack;
		delete[] _rWhite;
	}

	void MusicKeyboard::onTouch(int x, int y)
	{
		// Compare with black keys first.
		for(int i = 0; i < _nBlack; i++) {
			KBKey &k = _rBlack[i];
			if(k.r.contains(x, y)) {
				notePress(k.noteNum);
				return;
			}
		}

		// Then the white keys.
		for(int i = 0; i < _nWhite; i++) {
			KBKey &k = _rWhite[i];
			if(k.r.contains(x, y)) {
				notePress(k.noteNum);
				return;
			}
		}
	}

	void MusicKeyboard::onRelease()
	{
		if(_currentNote != kNoNote) {
			noteOff(_currentNote);
			if(_fnNoteOff != 0) {
				_fnNoteOff(_currentNote);
			}
			_currentNote = kNoNote;
		}
	}

	void MusicKeyboard::notePress(uint8_t noteNum)
	{
		noteOn(noteNum);
		_currentNote = noteNum;
		if(_fnNoteOn != 0) {
			_fnNoteOn(noteNum);
		}
	}

	void MusicKeyboard::draw(Gfx &gfx)
	{
		// Render white notes first.
		for(int i = 0; i < _nWhite; i++) {
			KBKey &k = _rWhite[i];
			if(isPressed(k.noteNum)) {
				gfx.setColourFg(Gfx::kColourOrange);
			} else {
				gfx.setColourFg(Gfx::kColourWhite);
			}
			gfx.fillRect(k.r);
		}

		// Then the black notes.
		for(int i = 0; i < _nBlack; i++) {
			KBKey &k = _rBlack[i];
			if(isPressed(k.noteNum)) {
				gfx.setColourFg(Gfx::kColourOrange);
			} else {
				gfx.setColourFg(Gfx::kColourBlack);
			}
			gfx.fillRect(k.r);
		}
	}

	void MusicKeyboard::noteOn(uint8_t noteNum)
	{
		_keyMap[noteNum >> 3] |= (1 << (noteNum & 7));
		dirty();
	}

	void MusicKeyboard::noteOff(uint8_t noteNum)
	{
		_keyMap[noteNum >> 3] &= ~(1 << (noteNum & 7));
		dirty();
	}

	bool MusicKeyboard::isPressed(uint8_t noteNum)
	{
		return 0 != (_keyMap[noteNum >> 3] & (1 << (noteNum & 7)));
	}

} // namespace Gui
