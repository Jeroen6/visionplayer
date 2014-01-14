/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Oscar
 *
 * Description: Header file for I2C
 *
 ******************************************************************************
  Change History:

    Version 1.0 - December 2011
    > Initial revision

******************************************************************************/
#ifndef SCCB_H_
#define SCCB_H_

#include <stdint.h>

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define SCCB_CLKRC  0x11
#define SCCB_COMB   0x13
#define SCCB_COMC   0x14
#define SCCB_COMH   0x28
#define SCCB_COMK   0x38
#define SCCB_COML   0x39
#define SCCB_MIDH   0x1C
#define SCCB_MIDL   0x1D

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void sccb_init(void);
uint8_t sccb_read(uint8_t reg);
void sccb_write(uint8_t reg, uint8_t data);

#endif /* SCCB_H_ */
