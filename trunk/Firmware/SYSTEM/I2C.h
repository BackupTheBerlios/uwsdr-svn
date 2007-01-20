
//////////////////////////////////////////////////////////////////////
//
// File: I2C.h
//
// Author: J F Main.
//
// Description:
//
//   I2C header file
//
// Compiler : mikroC, mikroElektronika C compiler
//            for Microchip PIC microcontrollers
//            Version: 5.0.0.3
//
// Copyright : Copyright © John Main 2006
//   http://www.best-microcontroller-projects.com
//   Free for non commercial use as long as this entire copyright notice
//   is included in source code and any other documentation.
//
//////////////////////////////////////////////////////////////////////

void I2C_Init(unsigned long speed);
void I2C_Start(void);
unsigned short I2C_Is_Idle(void);
unsigned short I2C_Rd(unsigned short ack);
unsigned short I2C_Wr(unsigned short data);
void I2C_Stop(void);
void I2C_send_byte(u8 slave, u8 sub, u8 data);

