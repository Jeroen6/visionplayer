/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for camera
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "stm32f4xx.h"
#include "camera.h"
#include "led.h"

#include "camera_io.h"
#include "sccb.h"
#include "dcmi.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
uint8_t error = 1;

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void camera_init(void)
{
  camera_io_init();
  sccb_init();
  dcmi_init();

  camera_io_reset();

  error = 0;

  if(sccb_read(SCCB_MIDH) != 0x7F){ error = 1; }
  if(sccb_read(SCCB_MIDL) != 0xA2){ error = 1; }

  if(error == 1){ led_set(LED_ORANGE); }
  else
  {
    led_reset(LED_ORANGE);

    sccb_write(SCCB_COMC, 0x20);
    //camera_write(CAMR_COMB, 0x21);
    //camera_write(CAMR_COMH, 0x41);

    dcmi_start();
  }
}

image_t * camera_snapshot(image_t ** img)
{
  return dcmi_snapshot(img);
}


