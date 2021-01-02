/*
 * USBMidi.h
 *
 *  Created on: 16 Nov 2020
 *      Author: adam
 */

#ifndef USBMIDI_USBMIDI_H_
#define USBMIDI_USBMIDI_H_

#include "MIDIMessage.h"
#include "usbh_def.h"

class USBMidi
{
public:
	class Delegate
	{
	public:
		virtual ~Delegate() { }

		virtual void usbMidiEvent(MIDIMessage msg) = 0;
	};

	static USBMidi *instance();

	USBMidi();

	void setDelegate(Delegate *delegate) { _delegate = delegate; }
	void poll();

private:
	enum STATE {
		appStateIdle,
		appStateStart,
		appStateReady,
		appStateRunning,
		appStateDisconnect
	};

	enum {
		kRxBufSize = 64
	};

	Delegate           *_delegate;
	USBH_HandleTypeDef  _hUSBHost;
	STATE               _appState;
	uint8_t             _midiRxBuffer[kRxBufSize]; // MIDI reception buffer

	static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

	void setState(STATE newState);
	void applicationPoll();
	void processMidiMessage(uint32_t msg);

public:
	void recvdData();
};

#endif // USBMIDI_USBMIDI_H_
