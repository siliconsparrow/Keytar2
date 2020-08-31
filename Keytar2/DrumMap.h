/*
 * DrumMap.h
 *
 *  Created on: 30 Aug 2020
 *      Author: adam
 */

#ifndef DRUMMAP_H_
#define DRUMMAP_H_

// Standard General MIDI level 1 drum map.

enum {
	kDrumABass = 35,    // 35 B0  Acoustic Bass Drum
	kDrumBass,          // 36 C1  Bass Drum 1
	kDrumSideStick,     // 37 C#1 Side Stick
	kDrumSnare,         // 38 D1  Acoustic Snare
	kDrumClap,          // 39 Eb1 Hand Clap
	kDrumESnare,        // 40 E1  Electric Snare
	kDrumTomFloor,      // 41 F1  Low Floor Tom
	kDrumHatClosed,     // 42 F#1 Closed Hi Hat
	kDrumTomHigh,       // 43 G1  High Floor Tom
	kDrumHatPedal,      // 44 Ab1 Pedal Hi-Hat
	kDrumTomLow,        // 45 A1  Low Tom
	kDrumHatOpen,       // 46 Bb1 Open Hi-Hat
	kDrumTomMid,        // 47 B1  Low-Mid Tom
	kDrumTomHiMid,      // 48 C2  Hi Mid Tom
	kDrumCymbalCrash,   // 49 C#2 Crash Cymbal 1
	kDrumTomHigh,       // 50 D2  High Tom
	kDrumCymbalRide,    // 51 Eb2 Ride Cymbal 1
	kDrumCymbalChinese, // 52 E2  Chinese Cymbal
	kDrumBellRide,      // 53 F2  Ride Bell
	kDrumTambourine,    // 54 F#2 Tambourine
	kDrumCymbalSplash,  // 55 G2  Splash Cymbal
	kDrumCowBell,       // 56 Ab2 Cowbell
	kDrumCymbalCrash2,  // 57 A2  Crash Cymbal 2
	kDrumVibraSlap,     // 58 Bb2 Vibraslap
	kDrumCymbalRide,    // 59 B2  Ride Cymbal 2
	kDrumBongoHigh,     // 60 C3  Hi Bongo
	kDrumBongoLow,      // 61 C#3 Low Bongo
	kDrumCongaHigh,     // 62 D3  Mute Hi Conga
	kDrumCongaOpenHigh, // 63 Eb3 Open Hi Conga
	kDrumCongaLow,      // 64 E3  Low Conga
	kDrumTimbaleHigh,   // 65 F3  High Timbale
	kDrumTimbaleLow,    // 66 F#3 Low Timbale
	kDrumAgogoHigh,     // 67 G3  High Agogo
	kDrumAgogoLow,      // 68 Ab3 Low Agogo
	kDrumCabasa,        // 69 A3  Cabasa
	kDrumMaracas,       // 70 Bb3 Maracas
	kDrumWhistleShort,  // 71 B3  Short Whistle
	kDrumWhistleLong,   // 72 C4  Long Whistle
	kDrumGuiroShort,    // 73 C#4 Short Guiro
	kDrumGuiroLong,     // 74 D4  Long Guiro
	kDrumClaves,        // 75 Eb4 Claves
	kDrumWoodBlockHigh, // 76 E4  Hi Wood Block
	kDrumWoodBlockLow,  // 77 F4  Low Wood Block
	kDrumCuicaMute,     // 78 F#4 Mute Cuica
	kDrumCuicaOpen,     // 79 G4  Open Cuica
	kDrumTriangleMute,  // 80 Ab4 Mute Triangle
	kDrumTriangleOpen,  // 81 A4  Open Triangle
};

#endif /* DRUMMAP_H_ */
