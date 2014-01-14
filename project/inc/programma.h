/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for the selected program
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#ifndef PROGRAMMA_H_
#define PROGRAMMA_H_

#include <stdint.h>
#include "operators.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void programma_start(image_t * in, image_t * out);
void programma_set(uint8_t nummer);

#endif /* PROGRAMMA_H_ */
