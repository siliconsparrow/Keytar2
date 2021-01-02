/*
 * USBMidi.h
 *
 *  Created on: 16 Nov 2020
 *      Author: adam
 */

#ifndef USBMIDI_USBMIDI_H_
#define USBMIDI_USBMIDI_H_

#include "usbh_def.h"

#define RX_BUFF_SIZE   64  /* Max Received data 64 bytes */

class USBMidi
{
public:

	static USBMidi *instance();

	USBMidi();

	void poll();

private:
	enum STATE {
		appStateIdle,
		appStateStart,
		appStateReady,
		appStateRunning,
		appStateDisconnect
	};

	USBH_HandleTypeDef _hUSBHost;
	STATE              _appState;
	uint8_t _midiRxBuffer[RX_BUFF_SIZE]; // MIDI reception buffer

	static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

	void setState(STATE newState);
	void applicationPoll();
	void processMidiMessage(uint32_t msg);

public:
	void recvdData();
};

#endif // USBMIDI_USBMIDI_H_
