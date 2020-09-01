// *****************************************************************************
// **
// ** Definitions of General MIDI standard parameters.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 4-Mar-2018
// **
// *****************************************************************************

#ifndef GENERALMIDI_H_
#define GENERALMIDI_H_

enum GeneralMidiProgram
{
// Piano
	GM_PIANO_ACOUSTIC_GRAND,     // 1 Acoustic Grand Piano
    GM_PIANO_ACOUSTIC_BRIGHT,    // 2 Bright Acoustic Piano
    GM_PIANO_ELECTRIC_GRAND,     // 3 Electric Grand Piano
    GM_PIANO_HONKY_TONK,         // 4 Honky-tonk Piano
    GM_PIANO_ELECTRIC_1,         // 5 Electric Piano 1
    GM_PIANO_ELECTRIC_2,         // 6 Electric Piano 2
    GM_HARPSICHORD,              // 7 Harpsichord
    GM_CLAVINET,                 // 8 Clavinet

// Chromatic Percussion
	GM_CELESTA,                  // 9 Celesta
    GM_GLOCKENSPIEL,             // 10 Glockenspiel
    GM_MUSIC_BOX,                // 11 Music Box
    GM_VIBRAPHONE,               // 12 Vibraphone
    GM_MARIMBA,                  // 13 Marimba
    GM_XYLOPHONE,                // 14 Xylophone
    GM_TUBULAR_BELLS,            // 15 Tubular Bells
    GM_DULCIMER,                 // 16 Dulcimer

// Organ
    GM_ORGAN_DRAWBAR,            // 17 Drawbar Organ
    GM_ORGAN_PERCISSIVE,         // 18 Percussive Organ
    GM_ORGAN_ROCK,               // 19 Rock Organ
    GM_ORGAN_CHURCH,             // 20 Church Organ
    GM_ORGAN_REED,               // 21 Reed Organ
    GM_ACCORDION,                // 22 Accordion
    GM_HARMONICA,                // 23 Harmonica
    GM_ACCORDION_TANGO,          // 24 Tango Accordion

// Guitar
    GM_GUITAR_ACOUSTIC,          // 25 Acoustic Guitar (nylon)
    GM_GUITAR_STEEL,             // 26 Acoustic Guitar (steel)
    GM_GUITAR_JAZZ,              // 27 Electric Guitar (jazz)
    GM_GUITAR_ELECTRIC_CLEAN,    // 28 Electric Guitar (clean)
    GM_GUITAR_ELECTRIC_MUTED,    // 29 Electric Guitar (muted)
    GM_GUITAR_OVERDRIVE,         // 30 Overdriven Guitar
    GM_GUITAR_DISTORTION,        // 31 Distortion Guitar
    GM_GUITAR_HARMONICS,         // 32 Guitar Harmonics

// Bass
    GM_BASS_ACOUSTIC,            // 33 Acoustic Bass
    GM_BASS_FINGER,              // 34 Electric Bass (finger)
    GM_BASS_ELECTRIC,            // 35 Electric Bass (pick)
    GM_BASS_FRETLESS,            // 36 Fretless Bass
    GM_BASS_SLAP,                // 37 Slap Bass 1
    GM_BASS_SLAP_2,              // 38 Slap Bass 2
    GM_BASS_SYNTH_1,             // 39 Synth Bass 1
    GM_BASS_SYNTH_2,             // 40 Synth Bass 2

// Strings
    GM_VIOLIN,                   // 41 Violin
    GM_VIOLA,                    // 42 Viola
    GM_CELLO,                    // 43 Cello
    GM_CONTRABASS,               // 44 Contrabass
    GM_STRINGS_TREMOLO,          // 45 Tremolo Strings
    GM_STRINGS_PIZZICATOR,       // 46 Pizzicato Strings
    GM_HARP_ORCHESTRAL,          // 47 Orchestral Harp
    GM_TIMPANI,                  // 48 Timpani

// Ensemble
    GM_STRING_ENSEMBLE_1,        // 49 String Ensemble 1
    GM_STRING_ENSEMBLE_2,        // 50 String Ensemble 2
    GM_STRINGS_SYNTH_1,          // 51 Synth Strings 1
    GM_STRINGS_SYNTH_2,          // 52 Synth Strings 2
    GM_CHOIR_AAH,                // 53 Choir Aahs
    GM_VOICE_OOH,                // 54 Voice Oohs
    GM_CHOIR_SYNTH,              // 55 Synth Choir
    GM_ORCHESTRA_HIT,            // 56 Orchestra Hit

// Brass
    GM_TRUMPET,                  // 57 Trumpet
    GM_TROMBONE,                 // 58 Trombone
    GM_TUBA,                     // 59 Tuba
    GM_TRUMPET_MUTED,            // 60 Muted Trumpet
    GM_FRENCH_HORN,              // 61 French Horn
    GM_BRASS_SECTION,            // 62 Brass Section
    GM_SYNTH_BRASS,              // 63 Synth Brass 1
    GM_SYNTH_BRASS_2,            // 64 Synth Brass 2

// Reed
    GM_SAXOPHONE_SOPRANO,        // 65 Soprano Sax
    GM_SAXOPHONE_ALTO,           // 66 Alto Sax
    GM_SAXOPHONE_TENOR,          // 67 Tenor Sax
    GM_SAXOPHONE_BARITONE,       // 68 Baritone Sax
    GM_OBOE,                     // 69 Oboe
    GM_ENGLISH_HORN,             // 70 English Horn
    GM_BASSOON,                  // 71 Bassoon
    GM_CLARINET,                 // 72 Clarinet

// Pipe
    GM_PICCOLO,                  // 73 Piccolo
    GM_FLUTE,                    // 74 Flute
    GM_RECORDER,                 // 75 Recorder
    GM_PAN_FLUTE,                // 76 Pan Flute
    GM_BLOWN_BOTTLE,             // 77 Blown bottle
    GM_SHAKUHACHI,               // 78 Shakuhachi
    GM_WHISTLE,                  // 79 Whistle
    GM_OCARINA,                  // 80 Ocarina

// Synth Lead
    GM_SYNTH_LEAD_SQUARE,        // 81 Lead 1 (square)
    GM_SYNTH_LEAD_SAWTOOTH,      // 82 Lead 2 (sawtooth)
    GM_SYNTH_LEAD_CALLIOPE,      // 83 Lead 3 (calliope)
    GM_SYNTH_LEAD_CHIFF,         // 84 Lead 4 (chiff)
    GM_SYNTH_LEAD_CHARANG,       // 85 Lead 5 (charang)
    GM_SYNTH_LEAD_VOICE,         // 86 Lead 6 (voice)
    GM_SYNTH_LEAD_FIFTHS,        // 87 Lead 7 (fifths)
    GM_SYNTH_LEAD_AND_BASS,      // 88 Lead 8 (bass + lead)

// Synth Pad
    GM_PAD_NEW_AGE,              // 89 Pad 1 (new age)
    GM_PAD_WARM,                 // 90 Pad 2 (warm)
    GM_PAD_POLYSYNTH,            // 91 Pad 3 (polysynth)
    GM_PAD_CHOIR,                // 92 Pad 4 (choir)
    GM_PAD_BOWED,                // 93 Pad 5 (bowed)
    GM_PAD_METALLIC,             // 94 Pad 6 (metallic)
    GM_PAD_HALO,                 // 95 Pad 7 (halo)
    GM_PAD_SWEEP,                // 96 Pad 8 (sweep)

// Synth Effects
    GM_SYNTH_FX_RAIN,            // 97 FX 1 (rain)
    GM_SYNTH_FX_SOUNDTRACK,      // 98 FX 2 (soundtrack)
    GM_SYNTH_FX_CRYSTAL,         // 99 FX 3 (crystal)
    GM_SYNTH_FX_ATMOSPHERE,      // 100 FX 4 (atmosphere)
    GM_SYNTH_FX_BRIGHTNESS,      // 101 FX 5 (brightness)
    GM_SYNTH_FX_GOBLINS,         // 102 FX 6 (goblins)
    GM_SYNTH_FX_ECHOES,          // 103 FX 7 (echoes)
    GM_SYNTH_FX_SCI_FI,          // 104 FX 8 (sci-fi)

// Ethnic
    GM_SITAR,                    // 105 Sitar
    GM_BANJO,                    // 106 Banjo
    GM_SHAMISEN,                 // 107 Shamisen
    GM_KOTO,                     // 108 Koto
    GM_KALIMBA,                  // 109 Kalimba
    GM_BAGPIPE,                  // 110 Bagpipe
    GM_FIDDLE,                   // 111 Fiddle
    GM_SHANAI,                   // 112 Shanai

// Percussive
    GM_TINKLE_BELL,              // 113 Tinkle Bell
    GM_AGOGO,                    // 114 Agogo
    GM_STEEL_DRUMS,              // 115 Steel Drums
    GM_WOODBLOCK,                // 116 Woodblock
    GM_TAIKO_DRUM,               // 117 Taiko Drum
    GM_MELODIC_TOM,              // 118 Melodic Tom
    GM_SYNTH_DRUM,               // 119 Synth Drum
    GM_REVERSE_CYMBAL,           // 120 Reverse Cymbal

// Sound effects
    GM_GUITAR_FRET_NOISE,        // 121 Guitar Fret Noise
    GM_BREATH_NOISE,             // 122 Breath Noise
    GM_SEASHORE,                 // 123 Seashore
    GM_BIRD_TWEET,               // 124 Bird Tweet
    GM_TELEPHONE_RING,           // 125 Telephone Ring
    GM_HELICOPTER,               // 126 Helicopter
    GM_APPLAUSE,                 // 127 Applause
    GM_GUNSHOT                   // 128 Gunshot
};

// GM Standard Drum Map
#define GM_DRUM_CHANNEL 9
#define GM_DRUM_BANK 120
enum GeneralMidiDrum
{
	GM_DRUM_BASS_2 = 34,     // 35 Bass Drum 2
	GM_DRUM_BASS_1,          // 36 Bass Drum 1
	GM_DRUM_RIMSHOT,         // 37 Side Stick/Rimshot
	GM_DRUM_SNARE_1,         // 38 Snare Drum 1
    GM_DRUM_HANDCLAP,        // 39 Hand Clap
    GM_DRUM_SNARE_2,         // 40 Snare Drum 2
    GM_DRUM_TOM_2_LOW,       // 41 Low Tom 2
	GM_DRUM_HIHAT_CLOSED,    // 42 Closed Hi-hat
    GM_DRUM_TOM_1_LOW,       // 43 Low Tom 1
    GM_DRUM_HIHAT_PEDAL,     // 44 Pedal Hi-hat
    GM_DRUM_TOM_2_MID,       // 45 Mid Tom 2
    GM_DRUM_HIHAT_OPEN,      // 46 Open Hi-hat
	GM_DRUM_TOM_1_MID,       // 47 Mid Tom 1
    GM_DRUM_TOM_2_HIGH,      // 48 High Tom 2
    GM_DRUM_CYMBAL_CRASH,    // 49 Crash Cymbal 1
    GM_DRUM_TOM_1_HIGH,      // 50 High Tom 1
    GM_DRUM_CYMBAL_1_RIDE,   // 51 Ride Cymbal 1
    GM_DRUM_CYMBAL_CHINESE,  // 52 Chinese Cymbal
    GM_DRUM_BELL_RIDE,       // 53 Ride Bell
    GM_DRUM_TAMBOURINE,      // 54 Tambourine
    GM_DRUM_CYMBAL_SPLASH,   // 55 Splash Cymbal
    GM_DRUM_COWBELL,         // 56 Cowbell
    GM_DRUM_CYMBAL_2_CRASH,  // 57 Crash Cymbal 2
    GM_DRUM_VIBRA_SLAP,      // 58 Vibra Slap
    GM_DRUM_CYMBAL_2_RIDE,   // 59 Ride Cymbal 2
	GM_DRUM_BONGO_HIGH,      // 60 High Bongo
    GM_DRUM_BONGO_LOW,       // 61 Low Bongo
    GM_DRUM_CONGA_HIGH_MUTE, // 62 Mute High Conga
    GM_DRUM_CONGA_HIGH_OPEN, // 63 Open High Conga
    GM_DRUM_CONGA_LOW,       // 64 Low Conga
    GM_DRUM_TIMBALE_HIGH,    // 65 High Timbale
    GM_DRUM_TIMBALE_LOW,     // 66 Low Timbale
    GM_DRUM_AGOGO_HIGH,      // 67 High Agogô
    GM_DRUM_AGOGO_LOW,       // 68 Low Agogô
    GM_DRUM_CABASA,          // 69 Cabasa
    GM_DRUM_MARACAS,         // 70 Maracas
    GM_DRUM_WHISTLE_SHORT,   // 71 Short Whistle
    GM_DRUM_WHISTLE_LONG,    // 72 Long Whistle
    GM_DRUM_GUIRO_SHORT,     // 73 Short Güiro
    GM_DRUM_GUIRO_LONG,      // 74 Long Güiro
    GM_DRUM_CLAVES,          // 75 Claves
    GM_DRUM_WOODBLOCK_HIGH,  // 76 High Wood Block
    GM_DRUM_WOODBLOCK_LOW,   // 77 Low Wood Block
    GM_DRUM_CUICA_MUTE,      // 78 Mute Cuíca
    GM_DRUM_CUICA_OPEN,      // 79 Open Cuíca
    GM_DRUM_TRIANGLE_MUTE,   // 80 Mute Triangle
    GM_DRUM_TRIANGLE_OPEN    // 81 Open Triangle
};

// Standard controller channels.
enum GeneralMidiControlChannel
{
	GM_CC_MODWHEEL            =   1, // 1 Modulation wheel
	GM_CC_VOLUME              =   7, // 7 Volume
	GM_CC_PAN                 =  10, // 10 Pan
	GM_CC_EXPRESSION          =  11, // 11 Expression
	GM_CC_SUSTAIN             =  64, // 64 Sustain pedal
	GM_CC_PARAM_LSB           = 100, // 100 Registered Parameter Number LSB
	GM_CC_PARAM_MSB           = 101, // Registered Parameter Number MSB
	GM_CC_ALL_CONTROLLERS_OFF = 121, // 121 All controllers off
	GM_CC_ALL_NOTES_OFF       = 123, // 123 All notes off

	// Additional control channels defined in General MIDI 2 spec.
    GM_CC_RESONANCE           =  71, // Filter Resonance (Timbre/Harmonic Intensity) (cc#71)
    GM_CC_RELEASE_TIME        =  72, // Release Time (cc#72)
    GM_CC_ATTACK_TIME         =  73, // Attack time (cc#73)
    GM_CC_CUTOFF_FREQUENCY    =  74, // Brightness/Cutoff Frequency (cc#74)
    GM_CC_DECAY_TIME          =  75, // Decay Time (cc#75)
    GM_CC_VIBRATO_RATE        =  76, // Vibrato Rate (cc#76)
    GM_CC_VIBRATO_DEPTH       =  77, // Vibrato Depth (cc#77)
    GM_CC_VIBRATO_DELAY       =  78  // Vibrato Delay (cc#78)
};

#endif /* GENERALMIDI_H_ */
