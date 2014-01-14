#ifndef _MOTOR_H_
#define _MOTOR_H_
/**
 * @file    USBHID.h
 *
 * @author	Jeroen Lodder
 * @date	Oktober 2013
 *
 * @{
 */
#include <stdint.h>

/* UART Motor sturing 
 * 
 * Deze klasse voorzien in een vertaallaag tussen
 * uart en de functies die het bord moet bieden
 *
 *	DEVICE IS POLLING ONLY
 *
 * Het bord is voorzien van:
 * 1x stappenmotor besturing software module (SMC)
 * 1x Auxilary low side FET switch
 * 1x Blauwe status LED
 * 1x Drukkknop 
 *
*/

/* Command list */
// Devices verifies with same CMD and optional data

// System commands
	#define CMD_SYS_CHECK		0x00	// Args: version
	#define CMD_SYS_RESET		0xFF	// Args: bool softreset

// Miscellaneous
	#define CMD_LED_OFF			0x10	// On board led on
	#define CMD_LED_ON			0x11	// On board led off
	#define CMD_LED_BLINK		0x12	// On board led blink
	#define CMD_LED_BREATH		0x13	// Not implemented
	#define CMD_GET_BUTTON		0x14	// Returns button history word with 20 ms interval per bit

// SMC commands
	/* Motor has 3 states :
		Locked	-	Current flowing, motor is powered
		Brake	-	Motor terminals short circuit
		Free	-	Motor terminals open
	*/
	#define CMD_SMC_STEP		0x20	// Args, int number of steps, bool direction, finish this in .. (ms), bool free motor (or keep locked)
	#define CMD_SMC_STATE		0x21	// Returns remaining steps, negative if paused

	#define CMD_SMC_STOP		0x22	// Aborts all motor control
	#define CMD_SMC_PAUSE		0x23	// Pauzes motor control, keeps locked (overheating!)
	#define CMD_SMC_CONTINUE	0x24	// Continues motor control

// Motor States (ineffective if stepping)
	#define CMD_SMC_FREE		0x25	// Free motor
	#define CMD_SMC_BRAKE		0x26	// Brake motor (different from lock)

// AUX channel
	#define CMD_AUX_OFF			0x30	// Auxilary channel ON	(discards pwm)
	#define CMD_AUX_ON			0x31	// Auxilary channel OFF (discards pwm)
	#define CMD_AUX_PWM			0x32	// Auxilary channel PWM. Args: duty cycle, period, timer prescaler (from 48 MHz)

	// Api
	void motor_init(void);
	int SMC_step(int steps, uint8_t dir, uint32_t time_ms, uint8_t free);
	int SMC_pauze(void);
	int SMC_continue(void);
	int SMC_stop(uint8_t);
	int SMC_stepstate(int *);
	int setAux(uint8_t);
	int setAux_pwm(uint16_t dutycycle, uint16_t period, uint16_t prescaler );
	int getButton(uint32_t *);
	int setLed(uint8_t );
	int setLedBlink(void);
	int check(int *);
	int reset(void);

// Functions
	int sendCommand(uint8_t );

//Byte operations
	void write_32_to_8(int *i, uint8_t *dst, uint32_t src);
	void write_16_to_8(int *i, uint8_t *dst, uint16_t src);
	uint32_t read_8_to_32(int *i, uint8_t *src);
	uint16_t read_8_to_16(int *i, uint8_t *src);
/**
 * @}
 */

#endif
