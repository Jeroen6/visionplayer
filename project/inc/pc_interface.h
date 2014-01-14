/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for the PC interface
 *
 ******************************************************************************
  Change History:

    Version 1.0 - January 2012
    > Initial revision

******************************************************************************/
#ifndef PC_INTERFACE_H_
#define PC_INTERFACE_H_

#include "timer.h"     // for benchmark_t
#include "operators.h" // for image_t

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void pc_send_image(image_t *);
void pc_send_benchmark(benchmark_t *);
void pc_send_string(char *);
void pc_reset_image_counter(void);
void pc_interface_init(void);
void pc_receive_image(image_t *);
int pc_mode_from_pc(void);

#endif /* PC_INTERFACE_H_ */
