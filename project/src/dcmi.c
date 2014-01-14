/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for Digital Camera Interface
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "dcmi.h"
#include "stm32f4xx.h"
#include "mem_manager.h"
#include "led.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define CAM_DCMI_AHB2             RCC_AHB2Periph_DCMI

#define CAM_DCMI_IO_A_AHB1        RCC_AHB1Periph_GPIOA
#define CAM_DCMI_IO_A_PORT        GPIOA
#define CAM_DCMI_IO_A_HSYNC       GPIO_Pin_4
#define CAM_DCMI_IO_A_PIXCLK      GPIO_Pin_6
#define CAM_DCMI_IO_A_D1          GPIO_Pin_10
#define CAM_DCMI_IO_A_HSYNC_NR    GPIO_PinSource4
#define CAM_DCMI_IO_A_PIXCLK_NR   GPIO_PinSource6
#define CAM_DCMI_IO_A_D1_NR       GPIO_PinSource10

#define CAM_DCMI_IO_B_AHB1        RCC_AHB1Periph_GPIOB
#define CAM_DCMI_IO_B_PORT        GPIOB
#define CAM_DCMI_IO_B_D5          GPIO_Pin_6
#define CAM_DCMI_IO_B_VSYNC       GPIO_Pin_7
#define CAM_DCMI_IO_B_D5_NR       GPIO_PinSource6
#define CAM_DCMI_IO_B_VSYNC_NR    GPIO_PinSource7

#define CAM_DCMI_IO_C_AHB1        RCC_AHB1Periph_GPIOC
#define CAM_DCMI_IO_C_PORT        GPIOC
#define CAM_DCMI_IO_C_D0          GPIO_Pin_6
#define CAM_DCMI_IO_C_D2          GPIO_Pin_8
#define CAM_DCMI_IO_C_D3          GPIO_Pin_9
#define CAM_DCMI_IO_C_D0_NR       GPIO_PinSource6
#define CAM_DCMI_IO_C_D2_NR       GPIO_PinSource8
#define CAM_DCMI_IO_C_D3_NR       GPIO_PinSource9

#define CAM_DCMI_IO_E_AHB1        RCC_AHB1Periph_GPIOE
#define CAM_DCMI_IO_E_PORT        GPIOE
#define CAM_DCMI_IO_E_D4          GPIO_Pin_4
#define CAM_DCMI_IO_E_D6          GPIO_Pin_5
#define CAM_DCMI_IO_E_D7          GPIO_Pin_6
#define CAM_DCMI_IO_E_D4_NR       GPIO_PinSource4
#define CAM_DCMI_IO_E_D6_NR       GPIO_PinSource5
#define CAM_DCMI_IO_E_D7_NR       GPIO_PinSource6

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
image_t * buffer0;
image_t * buffer1;

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void dcmi_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DCMI_InitTypeDef  DCMI_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;

  // DCMI - IO pins
  RCC_AHB1PeriphClockCmd(CAM_DCMI_IO_A_AHB1, ENABLE);
  RCC_AHB1PeriphClockCmd(CAM_DCMI_IO_B_AHB1, ENABLE);
  RCC_AHB1PeriphClockCmd(CAM_DCMI_IO_C_AHB1, ENABLE);
  RCC_AHB1PeriphClockCmd(CAM_DCMI_IO_E_AHB1, ENABLE);

  // DCMI - GPIOA
  GPIO_PinAFConfig(CAM_DCMI_IO_A_PORT, CAM_DCMI_IO_A_HSYNC_NR, GPIO_AF_DCMI);
  GPIO_PinAFConfig(CAM_DCMI_IO_A_PORT, CAM_DCMI_IO_A_PIXCLK_NR, GPIO_AF_DCMI);
  GPIO_PinAFConfig(CAM_DCMI_IO_A_PORT, CAM_DCMI_IO_A_D1_NR, GPIO_AF_DCMI);

  GPIO_InitStructure.GPIO_Pin = CAM_DCMI_IO_A_HSYNC | CAM_DCMI_IO_A_PIXCLK | CAM_DCMI_IO_A_D1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(CAM_DCMI_IO_A_PORT, &GPIO_InitStructure);

  // DCMI - GPIOB
  GPIO_PinAFConfig(CAM_DCMI_IO_B_PORT, CAM_DCMI_IO_B_VSYNC_NR, GPIO_AF_DCMI);
  GPIO_PinAFConfig(CAM_DCMI_IO_B_PORT, CAM_DCMI_IO_B_D5_NR, GPIO_AF_DCMI);

  GPIO_InitStructure.GPIO_Pin = CAM_DCMI_IO_B_D5 | CAM_DCMI_IO_B_VSYNC;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(CAM_DCMI_IO_B_PORT, &GPIO_InitStructure);

  // DCMI - GPIOC
  GPIO_PinAFConfig(CAM_DCMI_IO_C_PORT, CAM_DCMI_IO_C_D0_NR, GPIO_AF_DCMI);
  GPIO_PinAFConfig(CAM_DCMI_IO_C_PORT, CAM_DCMI_IO_C_D2_NR, GPIO_AF_DCMI);
  GPIO_PinAFConfig(CAM_DCMI_IO_C_PORT, CAM_DCMI_IO_C_D3_NR, GPIO_AF_DCMI);

  GPIO_InitStructure.GPIO_Pin = CAM_DCMI_IO_C_D0 | CAM_DCMI_IO_C_D2 | CAM_DCMI_IO_C_D3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(CAM_DCMI_IO_C_PORT, &GPIO_InitStructure);

  // DCMI - GPIOE
  GPIO_PinAFConfig(CAM_DCMI_IO_E_PORT, CAM_DCMI_IO_E_D4_NR, GPIO_AF_DCMI);
  GPIO_PinAFConfig(CAM_DCMI_IO_E_PORT, CAM_DCMI_IO_E_D6_NR, GPIO_AF_DCMI);
  GPIO_PinAFConfig(CAM_DCMI_IO_E_PORT, CAM_DCMI_IO_E_D7_NR, GPIO_AF_DCMI);

  GPIO_InitStructure.GPIO_Pin = CAM_DCMI_IO_E_D4 | CAM_DCMI_IO_E_D6 | CAM_DCMI_IO_E_D7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(CAM_DCMI_IO_E_PORT, &GPIO_InitStructure);

  // DCMI - Peripheral
  RCC_AHB2PeriphClockCmd(CAM_DCMI_AHB2, ENABLE);

  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;//DCMI_CaptureMode_SnapShot;
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;

  DCMI_Init(&DCMI_InitStructure);

  // 5. Configure the DMA2_Stream1 channel1 to transfer Data from DCMI DR
  // register to the destination memory buffer.

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  //RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA2, ENABLE);

  DMA_DeInit(DMA2_Stream1);

  DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(DCMI->DR);
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_InitStructure.DMA_Memory0BaseAddr = NULL; // No buffer yet
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;

  DMA_Init(DMA2_Stream1, &DMA_InitStructure);

  DMA_DoubleBufferModeConfig(DMA2_Stream1, NULL, DMA_Memory_0);
  DMA_DoubleBufferModeCmd(DMA2_Stream1, ENABLE);

  buffer0 = mem_manager_alloc();
  buffer1 = mem_manager_alloc();

  DMA_SetCurrDataCounter(DMA2_Stream1, (IMG_HEIGHT*IMG_WIDTH)/4);
  DMA_MemoryTargetConfig(DMA2_Stream1, (uint32_t)(buffer0->data),DMA_Memory_0);
  DMA_MemoryTargetConfig(DMA2_Stream1, (uint32_t)(buffer1->data),DMA_Memory_1);

  DCMI_ClearFlag(DCMI_FLAG_OVFRI);
  DCMI_ClearFlag(DCMI_FLAG_FRAMERI);

  DCMI_ITConfig(DCMI_IT_OVF, ENABLE);
  ///DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);

  NVIC_EnableIRQ(DCMI_IRQn);

  DCMI_Cmd(ENABLE);

  DMA_Cmd(DMA2_Stream1, ENABLE);
}

void DCMI_IRQHandler(void)
{
  uint16_t misr = DCMI->MISR;
  
  if(misr & DCMI_FLAG_OVFRI)
  {
    led_set(LED_ORANGE);

    DCMI_ClearFlag(DCMI_FLAG_OVFRI);
  }
}

void dcmi_start(void)
{
  DCMI_CaptureCmd(ENABLE);
}

image_t * dcmi_snapshot(image_t ** img)
{
  image_t * ret;
  
  if(DMA_GetCurrentMemoryTarget(DMA2_Stream1) != 0)
  {
    ret = buffer0;
    buffer0 = *img;
    DMA_MemoryTargetConfig(DMA2_Stream1, (uint32_t)(buffer0->data), DMA_Memory_0);
  }
  else
  {
    ret = buffer1;
    buffer1 = *img;
    DMA_MemoryTargetConfig(DMA2_Stream1, (uint32_t)(buffer1->data), DMA_Memory_1);
  }
  
  *img = ret;

  ret->height = IMG_HEIGHT;
  ret->width  = IMG_WIDTH;
  ret->lut    = LUT_STRETCH;
  
  return ret;
}
