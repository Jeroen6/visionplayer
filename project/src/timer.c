/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for the benchmark timer
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "stm32f4xx.h"
#include "timer.h"
#include <string.h>

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
volatile uint32_t new_frame = 0;

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void timer_init(void)
{
  TIM_OCInitTypeDef       TIM_OCInitStruct;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

  TIM_Cmd(TIM2, ENABLE);

  TIM_SetCounter(TIM2 , 0);

  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStruct.TIM_Pulse = 100000000;
  TIM_OC3Init(TIM2, &TIM_OCInitStruct);

  TIM_CCxCmd(TIM2, TIM_Channel_3, TIM_CCx_Enable);

  TIM_ClearFlag(TIM2, TIM_IT_CC3);
  TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
  NVIC_EnableIRQ(TIM2_IRQn);

  timer_set_fps(10);
}

void TIM2_IRQHandler(void)
{
  new_frame = 1;
  TIM_ClearFlag(TIM2, TIM_IT_CC3);
}

void timer_reset(void)
{
  TIM_SetCounter(TIM2 , 0);
  new_frame = 0;
}

void timer_set_fps(uint32_t fps)
{
  TIM_SetCompare3(TIM2, 84000000 / fps);
}

void timer_sleep(void)
{
  while(new_frame == 0){;}
}

void benchmark_start(benchmark_t * var, const char * name)
{
  strncpy(var->name, name, 15);
  var->name[15]='\0';
  var->start = TIM_GetCounter(TIM2);
}

void benchmark_stop(benchmark_t * var)
{
  var->stop = TIM_GetCounter(TIM2);
}
