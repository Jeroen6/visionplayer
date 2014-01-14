/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for the memory manager
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#ifndef MEM_MANAGER_H_
#define MEM_MANAGER_H_

#include <stdint.h>
#include <stdlib.h> //for NULL
#include "operators.h" // for image_t

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void mem_manager_init(void);
void mem_manager_free(image_t * img);
image_t * mem_manager_alloc(void);
uint8_t mem_manager_free_bloks(void);

#endif /* MEM_MANAGER_H_ */
