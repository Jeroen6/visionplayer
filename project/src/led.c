/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar/Hugo
 *
 * Description: Implementation file for the LED's
 *
 ******************************************************************************
  Change History:

    Version 2.0 - October 2012
    > Removed compiler warning

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "stm32f4xx.h"
#include "led.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define MODER12_MASK   ((1U<<24) | (1U<<25))
#define MODER13_MASK   ((1U<<26) | (1U<<27))
#define MODER14_MASK   ((1U<<28) | (1U<<29))
#define MODER15_MASK   ((1U<<30) | (1U<<31))

#define MODER12_OUTPUT (1U<<24)
#define MODER13_OUTPUT (1U<<26)
#define MODER14_OUTPUT (1U<<28)
#define MODER15_OUTPUT (1U<<30)

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void led_init(void)
{
  RCC->AHB1ENR |= RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD;

  GPIOB->MODER &= ~(MODER12_MASK | MODER13_MASK | MODER14_MASK | MODER15_MASK);
  GPIOB->MODER |= MODER12_OUTPUT | MODER13_OUTPUT | MODER14_OUTPUT | MODER15_OUTPUT;

  GPIOB->BSRRL = (1<<15)|(1<<12)|(1<<13)|(1<<14);


  GPIOD->MODER &= ~(MODER12_MASK | MODER13_MASK | MODER14_MASK | MODER15_MASK);
  GPIOD->MODER |= MODER12_OUTPUT | MODER13_OUTPUT | MODER14_OUTPUT | MODER15_OUTPUT;

  GPIOD->BSRRH = (1<<15)|(1<<12)|(1<<13)|(1<<14);
}

void led_set(uint8_t led)
{
  if(led >= LED1 && led <= LED4)
  {
    led += 11;
    GPIOB->BSRRH = (1<<led);
  }
  else if(led >= LED_GREEN && led <= LED_BLUE)
  {
    led += 7;
    GPIOD->BSRRL = (1<<led);
  }
}

void led_reset(uint8_t led)
{
  if(led >= LED1 && led <= LED4)
  {
    led += 11;
    GPIOB->BSRRL = (1<<led);
  }
  else if(led >= LED_GREEN && led <= LED_BLUE)
  {
    led += 7;
    GPIOD->BSRRH = (1<<led);
  }
}

void led_toggle(uint8_t led)
{
  if(led >= LED1 && led <= LED4)
  {
    led += (12 - LED1);
    GPIOB->ODR ^= (1<<led);
  }
  else if(led >= LED_GREEN && led <= LED_BLUE)
  {
    led += (12 - LED_GREEN);
    GPIOD->ODR ^= (1<<led);
  }
}

void led_set_bus(uint8_t i)
{
  uint32_t ord = GPIOB->ODR;

  i = i & 0x0f;

  ord |= (0xf<<12);
  ord &= ~(i<<12);

  GPIOB->ODR = ord;
}
