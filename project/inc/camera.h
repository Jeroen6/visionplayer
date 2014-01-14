/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for camera
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#ifndef CAMERA_H_
#define CAMERA_H_

#include "mem_manager.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void camera_init(void);
image_t * camera_snapshot(image_t ** img);

#endif /* CAMERA_H_ */
