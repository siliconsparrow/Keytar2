/*
 * USBStorage.cpp - Manage USB access to the SD card.
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#include "USBStorage.h"
#include "FileSystem.h"
#include "usbd_conf.h"
#include "usbd_def.h"

// Called by the USB driver to obtain possession of the SD card.
// Closes all files and unmounts the card.
extern "C" void sdCardToUsb()
{
	USBStorage::instance()->sdCardToUsb();
}

USBStorage *USBStorage::instance()
{
	static USBStorage *g_usbStorage = 0;

	if(g_usbStorage == 0) {
		g_usbStorage = new USBStorage();
	}

	return g_usbStorage;
}

USBStorage::USBStorage()
	: _sdToUsb(false)
{
	// Start up USB Mass storage device to access the SD card.
	 // TODO: There must be a better way to detect USB connection!
	 if(USBD_OK == USB_MSC_Init()) {
	 	printf("USB init OK\n");
	 } else {
	 	printf("USB init failed!\n");
	 }

	 FileSystem::instance(); // Mount the file system.
}

void USBStorage::poll()
{
	if(_sdToUsb) {
		if(0 == usbIsConnected()) {
			FileSystem::instance()->resume();
			_sdToUsb = false;
		}
	}
}

void USBStorage::sdCardToUsb()
{
	_sdToUsb = true;
	FileSystem::instance()->suspend();
}

#ifdef OLD

// The SD card cannot safely be used by the USB and the audio
// system at the same time. This must be called every now and
// then to handle switching between the two devices.
bool _sdToUsb = false;
void sdCardPoll()
{
	if(_sdToUsb) {
		if(0 == usbIsConnected()) {
			FileSystem::instance()->resume();
			_sdToUsb = false;
		}
	}
}

// Called by the USB driver to obtain possession of the SD card.
// Closes all files and unmounts the card.
extern "C" void sdCardToUsb()
{
	_sdToUsb = true;
	FileSystem::instance()->suspend();
}

//main...

// Start up USB Mass storage device to access the SD card.
 // TODO: There must be a better way to detect USB connection!
 if(USBD_OK == USB_MSC_Init()) {
 	printf("USB init OK\n");
 } else {
 	printf("USB init failed!\n");
 }

 FileSystem::instance(); // Mount the file system.

 //loop...
 // There doesn't seem to be any kind of USB disconnect event
 		// so I have to poll for USB disconnect.
 		sdCardPoll();

#endif
