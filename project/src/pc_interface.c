/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for the PC interface
 *
 ******************************************************************************
  Change History:

    Version 1.0 - January 2012
    > Initial revision

******************************************************************************/
#include <string.h>
#include "led.h"
#include "pc_interface.h"
#include "programma.h"
#include "stm32f4xx.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define FT245_RXF          ((uint16_t)0x0001)  /* Pin 0 selected */
#define FT245_TXE          ((uint16_t)0x0002)  /* Pin 1 selected */
#define FT245_RD           ((uint16_t)0x0004)  /* Pin 2 selected */
#define FT245_WR           ((uint16_t)0x0010)  /* Pin 4 selected */
#define FT245_USBEN        ((uint16_t)0x0020)  /* Pin 5 selected */

#define FT245_DATABUS      ((uint16_t)0xFF00)  /* DATABUS SELECTED */
#define FT245_CLEARDATA    ((uint16_t)0xFF00)  /* DATABUS SELECTED */

#define FT245_DATABUS_OUT  ((uint32_t)0x55550000)  /* DATABUS SELECTED AS OUT */
#define FT245_DATABUS_MASK ((uint32_t)0xFFFF0000)  /* DATABUS SELECTED AS MASK */
#define FT245_DATABUS_IN   ((uint32_t)0x00000000)  /* DATABUS SELECTED AS IN */

// HELLO is used as a version number that must match the version number in
// the PC software
#define HELLO        ('3')
#define EXIT         ('Q')
#define IMAGE        ('B')
#define BENCHMARK    ('C')
#define INFO         ('I')
#define SEND_IMAGE   ('S')

#define PROGRAMMA_NR ('D')
#define IMAGE_NR     ('E')
#define MODE         ('M')

#define INFO_STR_LEN (200)

// ----------------------------------------------------------------------------
// Type definitions
// ----------------------------------------------------------------------------
typedef enum
{
  Mode_None = 0,
  Mode_Single,
  Mode_Continuous,
  Mode_Inject
} d2xx_mode_t;

typedef enum
{
  read_start,
  read_img,
  read_img_nr,
  read_programma_nr,
  read_mode
} read_state_t;

typedef enum
{
  write_done,
  write_img_header,
  write_img_struct,
  write_string,
  write_benchmark,
  write_control
} write_state_t;

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
uint8_t image_counter;
uint8_t image_selected;
uint8_t pc_connection_active;

d2xx_mode_t mode_irq;
d2xx_mode_t mode;

volatile uint8_t image_busy;
uint8_t img_header[2];
image_t * img_to_send;
volatile uint8_t benchmark_busy;
uint8_t benchmark_buffer[1 + sizeof(benchmark_t)];
volatile uint8_t string_busy;
volatile uint8_t control_busy;
uint8_t control_buffer[1];
uint8_t control_size;
uint8_t string_buffer[1 + INFO_STR_LEN];
write_state_t write_state;
uint8_t * write_pointer;
uint16_t write_size;
read_state_t read_state;
uint16_t read_size;
volatile uint8_t img_ready;
uint8_t * img_to_receive;

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void hardware_init(void);
void pc_reset_connection(void);
void pc_enable_connection(void);
void pc_send_hello(void);

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
// Read
void EXTI0_IRQHandler(void)
{
  int i;
  uint16_t databuf;

  EXTI_ClearITPendingBit(EXTI_Line0);

  if((GPIOB->IDR & FT245_RXF) == 0)
  {
    if((GPIOE->MODER & FT245_DATABUS_MASK) != FT245_DATABUS_IN)
    {
      GPIOE->MODER &= ~FT245_DATABUS_MASK;
      GPIOE->MODER |= FT245_DATABUS_IN;
    }

    // STROBE RD down
    GPIOB->BSRRH = FT245_RD;
    
    // Wait 25 ns
    for(i=0; i<6; i++)
      __NOP();
      
    // READ D0...D7
    databuf = GPIOE->IDR;
    databuf = databuf>>8;

    // STROBE RD UP
    GPIOB->BSRRL = FT245_RD;

    switch(read_state)
    {
    case read_start:
      if(databuf == HELLO)
      {
        pc_reset_connection();
        pc_connection_active = 1;
        pc_send_hello();

        led_set(LED_BLUE);
      }
      else if(databuf == EXIT)
      {
        pc_reset_connection();
      }
      else if(databuf == SEND_IMAGE)
      {
        read_size = sizeof(image_t);
        read_state = read_img;
      }
      else if(databuf == PROGRAMMA_NR)
      {
        read_state = read_programma_nr;
      }
      else if(databuf == IMAGE_NR)
      {
        read_state = read_img_nr;
      }
      else if(databuf == MODE)
      {
        read_state = read_mode;
      }
      break;
    case read_img:
      if(img_to_receive != NULL)
      {
        read_size--;
        *img_to_receive = databuf;
        img_to_receive++;
        if(read_size == 0)
        {
          img_to_receive = NULL;
          img_ready = 1;
          read_state = read_start;
        }
      }
      break;
    case read_img_nr:
      image_selected = databuf;
      read_state = read_start;
      break;
    case read_programma_nr:
      programma_set(databuf);
      read_state = read_start;
      break;
    case read_mode:
      mode_irq = (d2xx_mode_t)databuf;
      read_state = read_start;
      break;
    }
  }
}

// Write
void EXTI1_IRQHandler(void)
{
  int i;
  uint16_t Data;

  EXTI_ClearITPendingBit(EXTI_Line1);
  if((GPIOB->IDR & FT245_TXE) == 0)
  {
    if(write_size == 0)
    {
      if(write_state == write_done)
      {
        if(control_busy == 1)
        {
          write_state = write_control;
          write_pointer = control_buffer;
          write_size = control_size;
        }
        else if(benchmark_busy == 1)
        {
          write_state = write_benchmark;
          write_pointer = (uint8_t *)&benchmark_buffer;
          write_size = sizeof(benchmark_buffer);
        }
        else if(string_busy == 1)
        {
          write_state = write_string;
          write_pointer = (uint8_t *)&string_buffer;
          write_size = sizeof(string_buffer);
        }
        else if(image_busy == 1)
        {
          write_state = write_img_header;
          write_pointer = img_header;
          write_size = sizeof(img_header);
        }
      }
      else if(write_state == write_img_header)
      {
        write_state = write_img_struct;
        write_pointer = (uint8_t *)img_to_send;
        write_size = sizeof(*img_to_send);
      }
    }

    // Send data?
    if(write_size != 0)
    {
      if((GPIOE->MODER & FT245_DATABUS_MASK) != FT245_DATABUS_OUT)
      {
        GPIOE->MODER &= ~FT245_DATABUS_MASK;
        GPIOE->MODER |= FT245_DATABUS_OUT;
      }

      // Clear data bits d0..d7
      GPIOE->BSRRH = FT245_CLEARDATA;
      
      // Prepare data
      Data= (*write_pointer) <<8;
      
      // Strobe wr high
      GPIOB->BSRRL = FT245_WR;
      
      // Write data to d0...d7
      GPIOE->BSRRL=Data;
      
      // 20 ns WR high and valid data
      for(i=0; i<2; i++)
        __NOP();
      
      // Strobe wr down
      GPIOB->BSRRH = FT245_WR;
      write_pointer++;
      write_size--;
      
      // wait 40 ns for txe to get high
      for(i=0; i<4; i++)
        __NOP();
    }

    if(write_size == 0)
    {
      switch(write_state)
      {
        case write_img_struct:
          write_state = write_done;
          image_busy = 0;
          break;
        case write_benchmark:
          write_state = write_done;
          benchmark_busy = 0;
          break;
        case write_string:
          write_state = write_done;
          string_busy = 0;
        case write_control:
          write_state = write_done;
          control_busy = 0;
          break;
        default:
          break;
      }
    }
  }
}

void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
    EXTI_ClearITPendingBit(EXTI_Line5);

    if(GPIO_ReadInputDataBit(GPIOB, FT245_USBEN) == Bit_RESET)
    {
      pc_reset_connection();
    }
  }
}


void pc_interface_init(void)
{
  hardware_init();
  pc_reset_connection();
}

void pc_send_image(image_t * img)
{
  if(mode != Mode_None && image_selected == image_counter)
  {
    // Wait while an other image is being sent
    while(image_busy != 0){;}

    image_busy = 1;
    img_header[0] = IMAGE;
    img_header[1] = image_counter;
    img_to_send = img;

    EXTI_GenerateSWInterrupt(EXTI_Line1);

    // Wait until the image has been sent
    while(image_busy != 0){;}

  }
  if(image_counter < 254)
  {
    image_counter++;
  }
}

void pc_reset_image_counter(void)
{
  image_counter = 1;
  mode = mode_irq;

  if(mode_irq == Mode_Single || mode == Mode_Inject)
  {
    mode_irq = Mode_None;
  }
}

void pc_send_benchmark(benchmark_t * benchmark)
{
  if(mode != Mode_None)
  {
    // Wait while an other benchmark is being sent
    while(benchmark_busy != 0){;}

    benchmark_busy = 1;
    benchmark_buffer[0] = BENCHMARK;
    memcpy(&benchmark_buffer[1], benchmark, sizeof(benchmark_t));

    // Enable send interrupt
    EXTI_GenerateSWInterrupt(EXTI_Line1);
  }
}

void pc_send_string(char * str)
{
  if(mode != Mode_None)
  {
    // Wait while an other string is being sent
    while(string_busy != 0){;}

    string_busy = 1;
    string_buffer[0] = INFO;
    strncpy((char *)&string_buffer[1], str, INFO_STR_LEN);
    string_buffer[INFO_STR_LEN] = '\0';

    // Enable send interrupt
    EXTI_GenerateSWInterrupt(EXTI_Line1);
  }
}

void pc_send_hello(void)
{
  if(pc_connection_active != 0)
  {
    while(control_busy != 0);
    control_busy = 1;
    control_buffer[0] = HELLO;
    control_size = 1;

    // Enable send interrupt
    EXTI_GenerateSWInterrupt(EXTI_Line1);
  }
}


void pc_receive_image(image_t * img)
{
  if(pc_connection_active != 0)
  {
    while(control_busy != 0){;}

    control_busy = 1;
    control_buffer[0] = SEND_IMAGE;
    control_size = 1;

    // Enable send interrupt
    EXTI_GenerateSWInterrupt(EXTI_Line1);

    img_ready = 0;
    img_to_receive = (uint8_t *)img;

    // Wait until image is received
    while(img_ready == 0){;}
  }

}

void pc_reset_connection(void)
{
  image_selected = 0;
  pc_connection_active = 0;

  write_state = write_done;
  write_size = 0;

  read_state = read_start;
  read_size = 0;

  programma_set(1);

  image_busy = 0;
  string_busy = 0;
  benchmark_busy = 0;
  control_busy = 0;

  mode = Mode_None;
  mode_irq = Mode_None;

  led_reset(LED_BLUE);
}

void hardware_init(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef FT_245_GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  // SET PB2==RD AND PB4==WR AS OUTPUT
  FT_245_GPIO_InitStructure.GPIO_Pin= FT245_RD | FT245_WR;
  FT_245_GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
  FT_245_GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
  FT_245_GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_NOPULL;
  FT_245_GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;
  GPIO_Init(GPIOB, &FT_245_GPIO_InitStructure);

  GPIO_SetBits(GPIOB, FT245_RD);
  //GPIO_SetBits(GPIOB, FT245_WR);

  // SET PB0==RXF AND PB1==TXE AND PB5==USBEN AS INPUT
  FT_245_GPIO_InitStructure.GPIO_Pin= FT245_RXF | FT245_TXE | FT245_USBEN;
  FT_245_GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
  FT_245_GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &FT_245_GPIO_InitStructure);

  // Set D0...D7 AS OUTPUT
  FT_245_GPIO_InitStructure.GPIO_Pin= FT245_DATABUS;
  FT_245_GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
  FT_245_GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
  FT_245_GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_NOPULL;
  FT_245_GPIO_InitStructure.GPIO_Speed=GPIO_Speed_25MHz;
  GPIO_Init(GPIOE, &FT_245_GPIO_InitStructure);

  // Connect EXTI Line0 to PA0 pin
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);

  // Configure EXTI Line0
  EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_EnableIRQ(EXTI9_5_IRQn);

  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_EnableIRQ(EXTI1_IRQn);
}

int pc_mode_from_pc(void)
{
  return (mode == Mode_Inject);
}
