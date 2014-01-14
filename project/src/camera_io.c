/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for camera I/O
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "camera_io.h"
#include "stm32f4xx.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define CAM_IO_AHB1     RCC_AHB1Periph_GPIOD
#define CAM_IO_PORT     GPIOD
#define CAM_IO_FODD     GPIO_Pin_7
#define CAM_IO_RST      GPIO_Pin_10
#define CAM_IO_PWDN     GPIO_Pin_11

#define RESET_TIME      5000

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void camera_io_reset(void)
{
  uint32_t i = RESET_TIME;

  GPIO_SetBits(CAM_IO_PORT, CAM_IO_RST);

  // Short delay
  while(i!=0)
    i--;

  GPIO_ResetBits(CAM_IO_PORT, CAM_IO_RST);

  i = RESET_TIME;
  while(i!=0)
    i--;
}

void camera_io_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // IO pins
  RCC_AHB1PeriphClockCmd(CAM_IO_AHB1, ENABLE);

  // Set RST and PWDN as output
  GPIO_InitStructure.GPIO_Pin = CAM_IO_RST | CAM_IO_PWDN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CAM_IO_PORT, &GPIO_InitStructure);

  // Set RST low
  GPIO_ResetBits(CAM_IO_PORT, CAM_IO_RST);
  
  // Set PWDN low
  GPIO_ResetBits(CAM_IO_PORT, CAM_IO_PWDN);

  // Set FODD as input
  GPIO_InitStructure.GPIO_Pin = CAM_IO_FODD;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CAM_IO_PORT, &GPIO_InitStructure);
}
