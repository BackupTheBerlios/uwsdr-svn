//////////////////////////////////////////////////////////////////////
// File: bit.h
//
// Description: Bit manipulation macros
//
//    These macros preserve the current value of the 'PORT' or register.
//
//    WRITEPORT is the same as writing to a port
//    but preserves the keepmask bits.
//
//////////////////////////////////////////////////////////////////////

// See below for detailed description of WRITEPORT(port,newval,keepMask)
#define WRITEPORT(port,newval,keepMask) \
    (port) =  ((newval) & (~keepMask)) | ((port) & (keepMask));

#define setBit(var, bitnum)   (var)|=(1<<(bitnum))
#define resBit(var, bitnum)   (var)&=~(1<<(bitnum))
#define clearBit(var, bitnum) (var)&=~(1<<(bitnum))
#define testBit(var, bitnum)  (var)&(1<<(bitnum))

///////////////////////////////////////////////////////////////////////
//
// Macro: WRITEPORT(port,newval,keepMask)
//
// Description: A macro to write only to specific bits of a port.
//
// Author: John Main.
//
//  The PORT is read, keepMask bits are preserved when the
//  data value is output to the port.
//
// Note this macro reads the specified port
//
// Beware of port loading (too much current) e.g. if drive an LED with
//   larger current then the input high voltage will not be met
//   (see data sheet graphs of Voh vs Io) so reading back a zero.
// If this is a problem the use a variable to hold the port state
//    and only read/update this, the putput to port.
//
// Breakdown of action of macro WRITEPORT.
//
//  1. Read the port and only keep the desired 'keepMask' bits.
//
//      e.g. if the current value of the port is 0x6b
//      and the mask is 0x0c (keep bits b3..2) from the port read.
//
//       ((port) & (keepMask)) results in 0x6b & 0x0c
//            0110-1011 port
//            0000-1100 keepMask
//            000001000 Result 1
//
//  2. Create the output value and send only to the used bits
//     i.e. to the non 'keepMask' bits of the port.
//     e.g. if newval is 0xa7 (1010-0111)
//
//       ((newval) & (~keepMask))
//            1010-0111 newval
//            1111-0011 ~keepMask
//            1010-0011 Result 2
//
//  3. 'OR' these together to get the final result.
//            0000-1000 Result 1
//            1010-0011 Result 2
//            1010-1011 Ored Result 3
//
//////////////////////////////////////////////////////////////////////


