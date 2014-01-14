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
    > Added FPU
    > Added comment

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "camera.h"
#include "dcmi.h"
#include "led.h"
#include "mem_manager.h"
#include "pc_interface.h"
#include "programma.h"
#include "stm32f4xx.h"
#include "timer.h"
#include "audio_driver.h"
#include "stm32f4_discovery_audio_codec.h"

// ----------------------------------------------------------------------------
// Main program
// ----------------------------------------------------------------------------
int main(void)
{
  // Declare variables
  image_t *src;
  image_t *dst;
  benchmark_t bench;

  // FPU: Set CP10 and CP11 Full Access
  SCB->CPACR |= ((3U << 10*2)|(3U << 11*2));

  // Initialize hardware
  led_init();
  mem_manager_init();
  camera_init();
  timer_init();
  pc_interface_init();
	Project_Init();		// Initialiseer project

  // Allocate memory for images
  src = mem_manager_alloc();
  dst = mem_manager_alloc();

  while(1)
  {
		led_toggle(LED_GREEN);
		
    // Set the maximum frame rate and reset the time
    timer_set_fps(10);
    timer_reset();

    // Reset image counter to make sure it starts at zero
    pc_reset_image_counter();

    // Start a benchmark
    benchmark_start(&bench,"Frame");

    // Receive image from PC?
    if(pc_mode_from_pc())
    {
      // Yes, get image from the PC interface
      pc_receive_image(src);
    }
    else
    {
      // No, get image from the camera module
      camera_snapshot(&src);

      // The camera module is mounted upside down, therefore the image
      // is rotated
      vRotate180(src);
    }

    // Execute selected vision set
    programma_start(src, dst);

    // Sleep until timer expires
    timer_sleep();

    // Stop and send the benchmark
    benchmark_stop(&bench);
    pc_send_benchmark(&bench);
  }
}
