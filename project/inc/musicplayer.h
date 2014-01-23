#ifndef __AUDIO_DRIVER
#define __AUDIO_DRIVER

//-------------------------------------------------------------------
// Headers
//-------------------------------------------------------------------
#include <stdint.h>
#include "operators.h"


//-------------------------------------------------------------------
// Definities
//-------------------------------------------------------------------
                                //   7      |    6     |     5      |    4    |     3     |    2     |     1    |   0
#define REG_BEEPFREQ     0x1C   // FREQ3    | FREQ2    | FREQ1      | FREQ0   | ONTIME3   | ONTIME2  | ONTIME1  | ONTIME0
#define REG_BEEPOFFVOL   0x1D    // OFFTIME2 | OFFTIME1 | OFFTIME0   | BPVOL4  | BPVOL3    | BPVOL2   | BPVOL1   | BPVOL0
#define REG_BEEPCONFTONE 0x1E    // BEEP1    | BEEP0    | BEEPMIXDIS | TREBCF1 |  TREBCF0  |  BASSCF1 |  BASSCF0 | TCEN

#define SEVENSEG_START_X       65              // Begin van horizontale 7-segment gebied in pixels
#define SEVENSEG_END_X         105            // Einde van horizontale 7-segment gebied in pixels
#define LEFT_SEGMENT_START_X   6              // Begin van horizontale linker  deel van rol in pixels
#define RIGHT_SEGMENT_START_X  SEVENSEG_END_X  // Begin van horizontale rechter deel van rol in pixels
#define ROI_START_Y             60              // Start van verticale ROI in pixels
#define ROI_END_Y               64              // Einde van verticale ROI in pixels
#define TONE_HOR_PX            8              // Maximale horizontale aantal pixels per toon
#define TONE_MIN_PX             3              // Minimale aantal pixels in een toon blob totaal

typedef enum
{
  Tone_XX = 0,      // Uit
  
  // Octaaf 5 (Linker deel van rol)
  Tone_C5 = 1,      // 521.74 Hz
  Tone_D5 = 2,      // 585.37 Hz
  Tone_E5 = 3,       // 666.67 Hz
  Tone_F5 = 4,      // 705.88 Hz
  Tone_G5 = 5,      // 774.19 Hz
  Tone_A5 = 6,      // 888.89 Hz
  Tone_B5 = 7,      // 1000.00 Hz
  
  // Octaaf 6 (Rechter deel van rol)
  Tone_C6 = 8,      // 1043.48 Hz
  Tone_D6 = 9,      // 1200.00 Hz
  Tone_E6 = 10,      // 1333.33 Hz
  Tone_F6 = 11,      // 1411.76 Hz
  Tone_G6 = 12,      // 1600.00 Hz
  Tone_A6 = 13,      // 1714.29 Hz
  Tone_B6 = 14,      // 2000.00 Hz
  
} BeepTone;          // Toon frequenties


typedef enum  
{
  Beep_Off        = 0x00,      // Uit
  Beep_Single     = 0x40,      // Beep turns on at a configurable frequency (FREQ) and volume (BPVOL) for the duration of ONTIME.
  Beep_Multiple   = 0x80,      // Beep turns on at a configurable frequency (FREQ) and volume (BPVOL) for the duration of ONTIME and turns off for the duration of OFFTIME.
  Beep_Continuous = 0xC0,      // Beep turns on at a configurable frequency (FREQ) and volume (BPVOL) and remains on until BEEP is cleared.
} BeepType;                    // Toon type


//-------------------------------------------------------------------
// Functie declaraties
//-------------------------------------------------------------------
void     vInitMusicplayer (void);
BeepTone vRecognizeTone   (image_t*, const blobinfo_t* blobinfo, const uint8_t);
void     vPlayTone          (const BeepTone);


//-------------------------------------------------------------------
// End-of-file 
//-------------------------------------------------------------------
#endif
