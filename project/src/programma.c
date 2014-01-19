/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for the selected program
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "programma.h"
#include "pc_interface.h"
#include "VisionSet.h"

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void WebcamMode(image_t *pSrc, image_t *pDst);

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
void (*programma_table[])(image_t *, image_t *) =
{
  WebcamMode,
  VisionSet1,
};

uint8_t programma_nummer = 0;

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void programma_start(image_t * in, image_t * out)
{
  if(programma_nummer >= sizeof(programma_table) / sizeof(void (*)(image_t *, image_t *)))
  {
    programma_nummer = 0;
  }

  programma_table[programma_nummer](in,out);
}

void programma_set(uint8_t nummer)
{
  programma_nummer = nummer;
}

void WebcamMode(image_t *pSrc, image_t *pDst)
{
  image_t **p;
  p = (image_t **)pSrc->data;
  *p = pSrc;
  pc_send_image(pSrc);
}



