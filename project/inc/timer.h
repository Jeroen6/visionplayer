/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for the benchmark timer
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include <stdint.h>

// ----------------------------------------------------------------------------
// Type definitions
// ----------------------------------------------------------------------------
typedef struct
{
  uint32_t start;
  uint32_t stop;
  char name[16];
} benchmark_t;

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void benchmark_start(benchmark_t *var, const char * name);
void benchmark_stop(benchmark_t *var);

void timer_init(void);
void timer_reset(void);
void timer_set_fps(uint32_t fps);
void timer_sleep(void);

#endif /* BENCHMARK_H_ */
