/*
**  Beep Generator
**
**  The beep generator delivers tones at select frequencies across approximately two octave major scales.
**  With independent volume control, beeps may be configured to occur continuously, periodically, or at single time intervals. 
**
**  The Beep Generator generates audio frequencies across approximately two octave major scales. It offers
**  three modes of operation: Continuous, multiple and single (one-shot) beeps. Sixteen on and eight off
**  times are available. 
*/

#ifndef __AUDIO_DRIVER
#define __AUDIO_DRIVER

/* Headers */
#include <stdint.h>

/* Definities */														  //   7      |    6     |     5      |    4    |     3     |    2     |     1    |   0
#define AUDIODRIVER_REG_BEEP_FREQ      0x1C   // FREQ3    | FREQ2    | FREQ1      | FREQ0   | ONTIME3   | ONTIME2  | ONTIME1  | ONTIME0
#define AUDIODRIVER_REG_BEEP_OFF_VOL   0x1D	  // OFFTIME2 | OFFTIME1 | OFFTIME0   | BPVOL4  | BPVOL3    | BPVOL2   | BPVOL1   | BPVOL0
#define AUDIODRIVER_REG_BEEP_CONF_TONE 0x1E   // BEEP1    | BEEP0    | BEEPMIXDIS | TREBCF1 |  TREBCF0  |  BASSCF1 |  BASSCF0 | TCEN


/* Enums */
typedef enum
{
	Tone_Off = -1, 
	Tone_C4  = 0,				// 260.87 Hz
	Tone_C5  = 1,  			// 521.74 Hz
	Tone_D5  = 2,  			// 585.37 Hz
	Tone_E5  = 3, 			// 666.67 Hz
	Tone_F5  = 4,  			// 705.88 Hz
	Tone_G5  = 5,  			// 774.19 Hz
	Tone_A5  = 6,  			// 888.89 Hz
	Tone_B5  = 7,  			// 1000.00 Hz
	Tone_C6  = 8,  			// 1043.48 Hz
	Tone_D6  = 9,  			// 1200.00 Hz
	Tone_E6  = 10,			// 1333.33 Hz
	Tone_F6  = 11,  		// 1411.76 Hz
	Tone_G6  = 12,  		// 1600.00 Hz
	Tone_A6  = 13,  		// 1714.29 Hz
	Tone_B6  = 14,  		// 2000.00 Hz
	Tone_C7  = 15,			// 2181.82 Hz
} AudioDriver_Tone;		// Sets the frequency of the beep signal. 


typedef enum	
{
	Beep_Off        = 0x00,			// Off
	Beep_Single     = 0x40,			// Beep turns on at a configurable frequency (FREQ) and volume (BPVOL) for the duration of ONTIME.
	Beep_Multiple   = 0x80,			// Beep turns on at a configurable frequency (FREQ) and volume (BPVOL) for the duration of ONTIME and turns off for the duration of OFFTIME.
	Beep_Continuous = 0xC0,			// Beep turns on at a configurable frequency (FREQ) and volume (BPVOL) and remains on until BEEP is cleared.
} AudioDriver_BeepType;	// Configures a beep mixed with the HP/Line and SPK output. 


/* Function definitions */
void    Project_Init       (void);
uint8_t Project_PlayTone   (const AudioDriver_Tone);
void    Project_StartMotor (void);

/* End-of-file */
#endif
