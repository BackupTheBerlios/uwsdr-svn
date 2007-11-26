
//////////////////////////////////////////////////////////////////////
//
// File: I2C.c
//
// Author: J F Main.
//
// Description:
//
//   I2C Software routines (single master mode only)
//   These routines do not use the built in I2C module at all.
//
// Compiler : mikroC, mikroElektronika C compiler
//            for Microchip PIC microcontrollers
//            Version: 5.0.0.3
//
// Note Testing:
//
//   Tested on 16F88
//
// Requirements:
//
//   I2C pins must be on the same port.
//
//   Target : 16F88
//
// Notes :
//
//   WARNING
//
//   Read-Modify_write
//   The latch output value is initialised to zero
//   only once.  Any other code that modifies the port
//   value must set the I2C port value pins zero at end.
//
//   R-M-W won't write to the I2C device as the bus is pulled
//   high - all that happens is that the latch output value
//   is set high - stopping I2C operation.
//
//   If you use (set/reset a port value) any
//   other output pins in the same port
//   Then you must set the SDA and SCL to zero  afterwards
//   as the read-modify-write instruction will read the
//   value of the port before setting it! causing
//   The I2C to fail (stop working).
//
//   Note : if using 16F88 then using the I2C module in
//   master mode will stop the R-M-W action so
//   you won't need to worry about it.
//
// Version:
// 1.00 - Initial release.
//
// Copyright : Copyright © John Main 2006
//   http://www.best-microcontroller-projects.com
//   Free for non commercial use as long as this entire copyright notice
//   is included in source code and any other documentation.
//
//////////////////////////////////////////////////////////////////////

#include "delay.h"
#include "config.h"
#include "bit.h"
#include "I2C.h"

//////////////////////////////////////////////////////////////////////
// Defines / Macros
//
#define I2C_PORT    PORTB   // User defined
#define I2C_PIN_SDA 1       // User defined
#define I2C_PIN_SCL 4       // User defined
#define I2C_TRIS    TRISB   // User defined

// Internal macros
#define I2C_SDA_0  CLR_PIN(PIOA, I2C_SDA_PIN) //Port = o/p (Drive low).
#define I2C_SCL_0  CLR_PIN(PIOA, I2C_SCL_PIN) //Port = o/p (Drive low).
#define I2C_SDA_1  SET_PIN(PIOA, I2C_SDA_PIN) //Port = i/p (Res pulls high).
#define I2C_SCL_1  SET_PIN(PIOA, I2C_SCL_PIN) //Port = i/p (Res pulls high).
#define I2C_SDA_IN GET_PIN(PIOA, I2C_SDA_PIN)
#define I2C_SCL_IN GET_PIN(PIOA, I2C_SCL_PIN)

//////////////////////////////////////////////////////////////////////
void I2C_Init(unsigned long speed) {
   // must do following in one go else port takes time to do it so R-M-F stops it.
   SET_PIN_OUTPUT(I2C_SDA_PIN|I2C_SCL_PIN);
   I2C_SDA_1; // release data  (high).
   I2C_SCL_1; // release clock (high).
}

//////////////////////////////////////////////////////////////////////
// 100kHz set here (very approximate). Will be slower  than 100kHz.
static void I2C_delay(void) {
   delay_us(2);
}

//////////////////////////////////////////////////////////////////////
// Start command
//
// SDA 1-0-0
// SCL 1-1-0
//
// Asumptions : coming from bus free state (d=1,ck=1)
void I2C_Start(void) {

   I2C_SDA_1;
   I2C_SCL_1;
   I2C_delay();
   
   I2C_SDA_0;
   I2C_delay();

   I2C_SCL_0;
   I2C_delay();
}

//////////////////////////////////////////////////////////////////////
// Stop command
//
// SDA 0-0-1
// SCL 0-1-1
//
void I2C_Stop(void) {

   I2C_SCL_0;   // Force to zero as can be sent at any time.
   I2C_SDA_0;   // Clock 1st so do not make a command.
   I2C_delay();

   I2C_SCL_1;
   I2C_delay();

   I2C_SDA_1;
   I2C_delay();
}

//////////////////////////////////////////////////////////////////////
unsigned short I2C_Rd(unsigned short ack) {

  unsigned short i;
  char data;

   I2C_SDA_1;  // Release the data pin

   // grab the data
   for(i=0;i<8;i++) {
      data <<=1;
      
      I2C_SCL_0;
      I2C_delay();  // Clock is low
      
      I2C_SCL_1;    // Clock released to high.
      
      while (I2C_SCL_IN==0) {;} // Wait here if clock is pulled low by slave
      if (I2C_SDA_IN) {data |= 1;} // Sample the data
      
      I2C_delay();  // Clock is high
   }
   
   // Send ACK or NACK
   I2C_SCL_0;
   
   if (ack) I2C_SDA_0;  // ACK
       else I2C_SDA_1;  // NACK

   I2C_delay();  // low T/w.
   I2C_SCL_1;
   I2C_delay();  // high T/2.
   
   I2C_SCL_0;    // Return clock to safe state.

   return data;
}

//////////////////////////////////////////////////////////////////////
// Returns 0 for no errors = the ACK bit from the slave
unsigned short I2C_Wr(unsigned short data) {
  unsigned short ret=0,i;

   // grab the data
   for(i = 0; i < 8; i++) {
   
      I2C_SCL_0; // Clock low
      if (data & 0x80) I2C_SDA_1; else I2C_SDA_0; // Data output
      data <<= 1;   // Next data bit.
      I2C_delay();  // low T/2.
      
      I2C_SCL_1;    // Clock released to high.
      I2C_delay();  // high T/2.
   }

   // get ACK or NACK
   I2C_SCL_0;
   I2C_SDA_1;    // Release the  data line
   SET_PIN_INPUT(I2C_SDA_PIN);
   I2C_delay();  // low T/2.
   
   I2C_SCL_1;
   // Sample the ACK
   ret = I2C_SDA_IN;
   I2C_delay();  // high T/2.
   
   I2C_SCL_0;    // Return clock to safe state
   SET_PIN_OUTPUT(I2C_SDA_PIN);

   return ret;
}

//********************************************************
//
// I2C_send_byte
//
// Parameters: void
// Returns: void
//
//********************************************************
void I2C_send_byte(u8 slave, u8 sub, u8 data)
{
  I2C_Start();
  I2C_Wr(slave);
  I2C_Wr(sub);
  I2C_Wr(data);
  I2C_Stop();
}

