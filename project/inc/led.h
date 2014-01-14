/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for the LED's
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#ifndef LED_H_
#define LED_H_

#include <stdint.h>

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define LED1       (1)
#define LED2       (2)
#define LED3       (3)
#define LED4       (4)

#define LED_GREEN  (5)
#define LED_ORANGE (6)
#define LED_RED    (7)
#define LED_BLUE   (8)

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void led_init(void);
void led_set(uint8_t led);
void led_reset(uint8_t led);
void led_toggle(uint8_t led);
void led_set_bus(uint8_t i);


#endif /* LED_H_ */
