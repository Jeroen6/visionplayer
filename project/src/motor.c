 /**
 * @file    motor.c
 * @brief   Api layer between Uart and Stepper Motor Board
 *
 * @author	Jeroen Lodder
 * @date	Januari 2014
 *
 * @note	This uart driver is write only
 *
 * @{
 */
#include <stdint.h>
#include "stm32f4xx.h"
#include "motor.h"
#include "stm32f4xx_usart.h"

static uint8_t outputBuffer[16];  		/** @brief: output buffer for transmit */
static USART_InitTypeDef uartInit;		/** @brief: init structure for uart */
static GPIO_InitTypeDef uartGpioInit;	/** @brief: init structure for gpio of uart */

/**
 * @brief   motor_init initialises io and motor driver	 
 */
void motor_init(void){
// Initialiseer USART3

	// Enable clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	// Set IO
	GPIO_StructInit(&uartGpioInit);
	uartGpioInit.GPIO_Pin 	= GPIO_Pin_8 | GPIO_Pin_9; 	// Pins 8 (TX) and 9 (RX) are used
	uartGpioInit.GPIO_Mode 	= GPIO_Mode_AF;           	// the pins are configured as alternate function so the USART peripheral has access to them
	uartGpioInit.GPIO_Speed = GPIO_Speed_50MHz;      		// this defines the IO speed and has nothing to do with the baudrate!
	uartGpioInit.GPIO_OType = GPIO_OType_PP;         		// this defines the output type as push pull mode (as opposed to open drain)
	uartGpioInit.GPIO_PuPd 	= GPIO_PuPd_UP;           	// this activates the pullup resistors on the IO pins
	// Set io alternate function
	GPIO_PinAFConfig(GPIOD,8,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD,9,GPIO_AF_USART3);
	// Set USART3
	USART_StructInit(&uartInit);
	uartInit.USART_BaudRate   = 115200;
	uartInit.USART_Parity     = USART_Parity_No;
	uartInit.USART_StopBits   = USART_StopBits_1;
	uartInit.USART_WordLength = USART_WordLength_8b;
	uartInit.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
	uartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// Write peripherals
	GPIO_Init(GPIOD, &uartGpioInit);
	USART_Init(USART3, &uartInit); 	
	USART_Init(USART3, &uartInit);
	// Enable uart
	USART_Cmd(USART3, ENABLE);
	

}

/**
 * @brief   Stepper motor control main command
 *
 * @param[in] 	steps	Number of steps to take
 * @param[in] 	dir		Direction to step in, 1 or 0
 * @param[in] 	time_ms	Time to take for these steps
 * @param[in] 	free	Free or lock motor when done 1 or 0 respectively
 * @return		-1 when failed to send to USB	 
 */
int SMC_step(int steps, uint8_t dir, uint32_t time_ms, uint8_t free){
	// Not allowed to initiate new commands when dirty
	//if( isDirty() )
		//return -1;
	/* Prepare datapacket */
	int i = 1; // Start at one, since 0 has command
	// Write arguments to buffer, low index == lsb
	write_32_to_8(&i,outputBuffer,(uint32_t)steps);
	outputBuffer[i++]	=	dir;
	write_32_to_8(&i,outputBuffer,(uint32_t)time_ms);
	outputBuffer[i++]	=	free;
		
	/* Send packet */
	return sendCommand( CMD_SMC_STEP );
}

/**
 * @brief   Pause any active motor movement
 * @return	-1 when failed
 */
int SMC_pauze(void){
	return sendCommand( CMD_SMC_PAUSE );
}

/**
 * @brief   Continue from paused state
 * @return	-1 when failed
 */
int SMC_continue(void){
	return sendCommand( CMD_SMC_CONTINUE );
}

/**
 * @brief   Stop motor and brake or lock
 * @return		-1 when failed
 */
int SMC_stop(uint8_t brake){
	outputBuffer[1] = brake;
	return sendCommand( CMD_SMC_STOP );
}

/**
 * @brief		Return remaining steps, negative if paused
 * @param[out]	Pointer to variable to put state in
 * @return		-1 when failed
 */
int SMC_stepstate(int *state){
	// Extract data
	return 0;
	/*
	uint8_t data[4];
	int i;
	for(i=0; i<4; i++){
		data[i] = USBDevice.readFromTheInputBuffer(i+2);
	}
	// Decode
	*state = read_8_to_32(&i,data);
	return 0;
	*/
}

/**
 * @brief		Set Auxilary channel
 * @param[in]	State to set
 * @return		-1 when failed
 */
int setAux(uint8_t state){
	if(state){
		return sendCommand( CMD_AUX_ON );
	}else{
		return sendCommand( CMD_AUX_OFF );
	}
}

/**
 * @brief   Configure PWM on auxilary channel
 *
 * @param[in] 	dutycycle	PWM Duty cycle, should be lower than period
 * @param[in] 	period		PWM Period
 * @param[in] 	prescaler	Pre divider of 48 MHz (0 == 48 MHz)
 * @return		-1 when failed to send to USB	 
 *
 * @note	All arguments are in timer registers	
 */
int setAux_pwm(uint16_t dutycycle, uint16_t period, uint16_t prescaler ){
	/* Prepare data */
	int i=1;
	write_16_to_8(&i,outputBuffer,dutycycle);
	write_16_to_8(&i,outputBuffer,period);
	write_16_to_8(&i,outputBuffer,prescaler);
			
	/* Send USB packet */
	return sendCommand( CMD_AUX_PWM );
}

/**
 * @brief		Returns button history to pointer
 * @param[out]	Button history as (left shift) register with 20 ms per bit
 * @return		-1 when failed
 */
int getButton(uint32_t *button_history){
	return 0;
	/*
	int i=0;
	uint8_t data[4];
	if( sendCommand( CMD_GET_BUTTON ) ){
		return -1;
	}
	// Extract data
	for(i=2; i<6; i++){
		data[i-2] = USBDevice.readFromTheInputBuffer(i);
	}
	// Decode
	*button_history = read_8_to_32(&i,data);
	return 0;
	*/
}

/**
 * @brief		Set on board LED
 * @param[in]	State to set to
 * @return		-1 when failed
 */
int setLed(uint8_t state){
	if(state){
		return sendCommand( CMD_LED_ON );
	}else{
		return sendCommand( CMD_LED_OFF );
	}
}

/**
 * @brief		Set on board LED to blink
 * @return		-1 when failed
 */
int setLedBlink(void){
	return sendCommand( CMD_LED_BLINK );
}

/**
 * @brief		Get firmware version of motor controller
 * @param[out]	Pointer to store version in
 * @return		-1 when failed
 */
int check(int *version){
	return 0xFF;
	/*
	if( sendCommand( CMD_SYS_CHECK ) )
		return -1;
	*version = USBDevice.readFromTheInputBuffer(2);
	return 0;
	*/
}

/**
 * @brief		Random feature
 * @return		-1 when failed
 */
int egg(void){
	if( sendCommand( 0xEE ) )
		return -1;
	return 1;
}

/**
 * @brief		Soft reset device current mode
 * @return		-1 when failed
 */
int reset(void){
	return sendCommand( CMD_SYS_RESET );
}

/**
 * @brief Private functions 
 * @{
 */
 
/**
 * @brief			Send UART Command
 * @param[in]	Command byte
 * @return		-1 when failed
 */
int sendCommand(uint8_t cmd){
	uint32_t i=0;
	// Send buffer
	outputBuffer[0] = cmd;
	
	for(i=0; i<16; i++){
		// Check for ready
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TC));
		USART_ClearFlag(USART3,USART_FLAG_TC);
		
		USART_SendData(USART3, outputBuffer[i]);
		outputBuffer[i] = 0;
	}
	
	return 0;
}

/**
 * @brief   Write 32 Bit word to 4 bytes
 * @note	Lower index equals least significant byte 
 *
 * @param[in,out]	&i	Index variable pointer, will incement 4
 * @param[out] 		dst	Destination array
 * @param[in] 		src	Input variable
 */
void write_32_to_8(int *i, uint8_t dst[], uint32_t src){
	dst[(*i)++] = (src >> 0)&0xFF ;
	dst[(*i)++] = (src >> 8)&0xFF ;
	dst[(*i)++] = (src >> 16)&0xFF;
	dst[(*i)++] = (src >> 24)&0xFF;
}

/**
 * @brief   Write 16 Bit word to 2 bytes
 * @note	Lower index equals least significant byte 
 *
 * @param[in,out]	&i	Index variable pointer will incement 2
 * @param[out] 		dst	Destination array
 * @param[in] 		src	Input variable
 */
void write_16_to_8(int *i, uint8_t dst[], uint16_t src){
	dst[(*i)++] = (src >> 0)&0xFF ;
	dst[(*i)++] = (src >> 8)&0xFF ;
}

/**
 * @brief   Read 32 Bit word from 4 bytes
 * @note	Lower index equals least significant byte 
 *
 * @param[in,out] &i	Index variable pointer will incement 4
 * @param[in] 	  src	Source array
 * @return	Read value
 */
uint32_t read_8_to_32(int *i, uint8_t *src){
	uint32_t data = 0;
	data |= (src[(*i)++] << 0) ; 
	data |= (src[(*i)++] << 8) ; 
	data |= (src[(*i)++] << 16); 
	data |= (src[(*i)++] << 24); 
	return data;
}

/**
 * @brief   Read 16 Bit word from 2 bytes
 * @note	Lower index equals least significant byte 
 *
 * @param[in,out]	&i	Index variable pointer, will incement 2
 * @param[in] 		src	Direction to step in, 1 or 0
 * @return	Read value
 */
uint16_t read_8_to_16(int *i, uint8_t *src){
	uint16_t data = 0;
	data |= (src[(*i)++] << 0) ;
	data |= (src[(*i)++] << 8) ;
	return data;
}
/**
 * @}
 */
/**
 * @}
 */
