/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Implementation file for I2C
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#include "sccb.h"
#include "stm32f4xx.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define CAM_I2C_IO_AHB1    RCC_AHB1Periph_GPIOB
#define CAM_I2C_IO_PORT    GPIOB
#define CAM_I2C_IO_SCL     GPIO_Pin_10
#define CAM_I2C_IO_SDA     GPIO_Pin_11
#define CAM_I2C_IO_SCL_NR  GPIO_PinSource10
#define CAM_I2C_IO_SDA_NR  GPIO_PinSource11

#define CAM_I2C_APB1       RCC_APB1Periph_I2C2
#define CAM_I2C_PORT       I2C2

#define CAM_I2C_ADDRESS    0xC0
#define CAM_I2C_SPEED      100000  // 100Khz ?
#define CAM_I2C_TIMEOUT    1000

#define TIMEOUT_MAX        10000
#define WRITE_DELAY        5000

// ----------------------------------------------------------------------------
// Function implementations
// ----------------------------------------------------------------------------
void sccb_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;

  // I2C - IO pins
  RCC_AHB1PeriphClockCmd(CAM_I2C_IO_AHB1, ENABLE);

  // Set SCL and SDA to Alternate Function
  GPIO_InitStructure.GPIO_Pin = CAM_I2C_IO_SCL | CAM_I2C_IO_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CAM_I2C_IO_PORT, &GPIO_InitStructure);

  // Connect pins to I2C peripheral
  GPIO_PinAFConfig(CAM_I2C_IO_PORT, CAM_I2C_IO_SCL_NR, GPIO_AF_I2C2);
  GPIO_PinAFConfig(CAM_I2C_IO_PORT, CAM_I2C_IO_SDA_NR, GPIO_AF_I2C2);

  // I2C - Peripheral
  RCC_APB1PeriphClockCmd(CAM_I2C_APB1, ENABLE);

  I2C_DeInit(CAM_I2C_PORT);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = CAM_I2C_SPEED;

  I2C_Init(CAM_I2C_PORT, &I2C_InitStructure);
  I2C_Cmd(CAM_I2C_PORT, ENABLE);
}

void sccb_write(uint8_t reg, uint8_t data)
{
  uint32_t timeout;
  uint32_t i = WRITE_DELAY;

  timeout = TIMEOUT_MAX;
  while(I2C_GetFlagStatus(CAM_I2C_PORT, I2C_FLAG_BUSY))
  {
    if ((timeout--) == 0) return;
  }

  // Start the config sequence
  I2C_GenerateSTART(CAM_I2C_PORT, ENABLE);

  timeout = TIMEOUT_MAX;
  while (!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((timeout--) == 0) return;
  }

  // Transmit the slave address and enable writing operation
  I2C_Send7bitAddress(CAM_I2C_PORT, CAM_I2C_ADDRESS, I2C_Direction_Transmitter);

  // Test on EV6 and clear it
  timeout = TIMEOUT_MAX;
  while (!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if ((timeout--) == 0) return;
  }

  I2C_SendData(CAM_I2C_PORT, reg);

  // Test on EV8 and clear it
  timeout = TIMEOUT_MAX;
  while (!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if ((timeout--) == 0) return;
  }

  // Prepare the register value to be sent
  I2C_SendData(CAM_I2C_PORT, data);

  // Wait till all data have been physically transferred on the bus
  timeout = TIMEOUT_MAX;
  while (!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if ((timeout--) == 0) return;
  }

  // End the configuration sequence
  I2C_GenerateSTOP(CAM_I2C_PORT, ENABLE);

  // Short delay
  // It seems like the chip needs some time to perform a write operation
  while(i!=0)
    i--;
}

uint8_t sccb_read(uint8_t reg)
{
  uint8_t result = 0;
  uint32_t timeout;

  timeout = TIMEOUT_MAX;
  while(I2C_GetFlagStatus(CAM_I2C_PORT, I2C_FLAG_BUSY))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  // Start the config sequence
  I2C_GenerateSTART(CAM_I2C_PORT, ENABLE);

  timeout = TIMEOUT_MAX;
  while (!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  // Transmit the slave address and enable writing operation
  I2C_Send7bitAddress(CAM_I2C_PORT, CAM_I2C_ADDRESS, I2C_Direction_Transmitter);

  // Test on EV6 and clear it
  timeout = TIMEOUT_MAX;
  while (!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  I2C_SendData(CAM_I2C_PORT, reg);

  // Test on EV8 and clear it
  timeout = TIMEOUT_MAX;
  while (!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  // End the configuration sequence
  I2C_GenerateSTOP(CAM_I2C_PORT, ENABLE);

  timeout = TIMEOUT_MAX;
  while(I2C_GetFlagStatus(CAM_I2C_PORT, I2C_FLAG_BUSY))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  // Send START condition a second time
  I2C_GenerateSTART(CAM_I2C_PORT, ENABLE);

  // Test on EV5 and clear it (cleared by reading SR1 then writing to DR)
  timeout = TIMEOUT_MAX;
  while(!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  // Send Codec address for read
  I2C_Send7bitAddress(CAM_I2C_PORT, CAM_I2C_ADDRESS, I2C_Direction_Receiver);

  // Wait on ADDR flag to be set (ADDR is still not cleared at this level
  timeout = TIMEOUT_MAX;
  while(!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  // Disable Acknowledgment
  I2C_AcknowledgeConfig(CAM_I2C_PORT, DISABLE);

  // Send STOP Condition
  I2C_GenerateSTOP(CAM_I2C_PORT, ENABLE);

  // Wait for the byte to be received
  timeout = TIMEOUT_MAX;
  while(!I2C_CheckEvent(CAM_I2C_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED))
  {
    if ((timeout--) == 0) return 0xFF;
  }

  // Read the byte received from the Codec
  result = I2C_ReceiveData(CAM_I2C_PORT);

  // Re-Enable Acknowledgment to be ready for another reception
  I2C_AcknowledgeConfig(CAM_I2C_PORT, ENABLE);

  // Clear AF flag for next communication
  I2C_ClearFlag(CAM_I2C_PORT, I2C_FLAG_AF);

  return result;
}
