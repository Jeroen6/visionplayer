/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Hugo
 *
 * Description: Header file for all the vision sets
 *
 ******************************************************************************
  Change History:

    Version 1.0 - September 2011
    > Initial revision

******************************************************************************/
#ifndef _VISIONSET_H_
#define _VISIONSET_H_

#include "operators.h"
#include "pc_interface.h"

#define ROI_MARGIN  0
#define ROI_TOP			45
#define ROI_BOTTOM  55
#define ROI_LEFT    64
#define ROI_RIGHT   116
#define START_LEFT  10
#define START_RIGHT 116
#define DOT_WIDTH   14

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void VisionSet1 (image_t *pSrc, image_t *pDst);
void VisionSet2 (image_t *pSrc, image_t *pDst);
#endif // _VISIONSET_H_
