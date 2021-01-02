/*
 * USBMidi.cpp
 *
 *  Created on: 16 Nov 2020
 *      Author: adam
 */

#include "USBMidi.h"
extern "C"
{
	#include "usbh_core.h"
	#include "usbh_MIDI.h"
	//#include "usb_core.h"
}

#ifdef OLD

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;
#endif // OLD


USBMidi *USBMidi::instance()
{
	static USBMidi *g_usbMidi = 0;

	if(g_usbMidi == 0) {
		g_usbMidi = new USBMidi();
	}

	return g_usbMidi;
}

USBMidi::USBMidi()
	: _appState(appStateIdle)
{
#ifdef OLD
	USBH_Init(&USB_OTG_Core_dev,
			USB_OTG_FS_CORE_ID,
			&USB_Host,
			&MIDI_cb,
			&USR_Callbacks);
#endif // OLD

    // Init Host Library
    USBH_Init(&_hUSBHost, USBH_UserProcess, 0);

    // Add Supported Class
    USBH_RegisterClass(&_hUSBHost, USBH_MIDI_CLASS);

    // Start Host Process
    USBH_Start(&_hUSBHost);
}

void USBMidi::poll()
{
	// Process MIDI.
	applicationPoll();

    // USB Host Background task.
    USBH_Process(&_hUSBHost);
}

void USBMidi::USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
	switch (id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		break;

	case HOST_USER_DISCONNECTION:
		instance()->setState(appStateDisconnect);
		printf("Application connected.\n");
		break;

	case HOST_USER_CLASS_ACTIVE:
		instance()->setState(appStateReady);
		printf("Application ready\n");
		break;

	case HOST_USER_CONNECTION:
		instance()->setState(appStateStart);
		printf("Application start\n");
		break;

	default:
		break;
	}
}

void USBMidi::setState(STATE newState)
{
	_appState = newState;
}

void USBMidi::applicationPoll()
{
	switch(_appState)
	{
	case appStateReady:
		USBH_MIDI_Receive(&_hUSBHost, _midiRxBuffer, RX_BUFF_SIZE); // just once at the beginning, start the first reception
		_appState = appStateRunning;
		printf("Application running.\n");
		break;

	case appStateDisconnect:
		USBH_MIDI_Stop(&_hUSBHost);
		_appState = appStateIdle;
		printf("Application idle.\n");
		break;
	}
}

void USBMidi::recvdData()
{
	unsigned numberOfPackets = USBH_MIDI_GetLastReceivedDataSize(&_hUSBHost) / sizeof(uint32_t); //each USB midi package is 4 bytes long

	const uint32_t *p = (const uint32_t *)_midiRxBuffer;
	for(unsigned i = 0; i < numberOfPackets; i++) {
		processMidiMessage(p[i]);
	}

	USBH_MIDI_Receive(&_hUSBHost, _midiRxBuffer, RX_BUFF_SIZE); // start a new reception
}

extern "C" void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost)
{
	USBMidi::instance()->recvdData();
}

void USBMidi::processMidiMessage(uint32_t msg)
{
	printf("MSG %02X %02X %02X\n", (int)(msg & 0xFF), (int)((msg >> 8) & 0xFF), (int)((msg >> 16) & 0xFF));
	switch(0xF0 & msg)
	{
	case 0x80:
		printf("Note off.\n");
		break;

	case 0x90:
		printf("Note on.\n");
		break;

	}
}
