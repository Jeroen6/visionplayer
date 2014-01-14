/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for Digital Camera Interface
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#ifndef DCMI_H_
#define DCMI_H_

#include <stdint.h>
#include "operators.h" // for image_t

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void dcmi_init(void);
void dcmi_start(void);

image_t * dcmi_snapshot(image_t ** img);

#endif /* DCMI_H_ */
