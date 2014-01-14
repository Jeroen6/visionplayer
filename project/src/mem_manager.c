/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for memory menager
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "mem_manager.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
image_t pool[4] ; //__attribute__((section (".noinit")));
image_t * free_list = NULL;
uint8_t free_bloks  = 0;

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void mem_manager_init(void)
{
  uint8_t i;

  free_bloks = sizeof(pool) / sizeof(image_t);
  free_list = NULL;

  for(i = 0;i < free_bloks; i++)
  {
    *((image_t **)(&pool[i].data)) =  free_list;
    free_list = &pool[i];
  }
}

image_t * mem_manager_alloc(void)
{
  image_t * ret;
  if(free_list == NULL)
  {
    ret = NULL;
  }
  else
  {
    ret = free_list;
    free_list = *((image_t **)&(ret->data));

    ret->height = IMG_HEIGHT;
    ret->width = IMG_WIDTH;
    free_bloks--;
  }
  return ret;
}

void mem_manager_free(image_t * img)
{
  if(img != NULL)
  {
    *((image_t **)&(img->data)) =  free_list;
    free_list = img;
    free_bloks++;
  }
}

uint8_t mem_manager_free_bloks(void)
{
  return free_bloks;
}



