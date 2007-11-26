//************************************************
//
//  types.h
//    
//    This file is used as an abstraction of
//    the variable type definitions. Every user
//    file should include this file instead of any
//    device specific type definition.
//
//************************************************

#define __inline inline

#include    "AT91SAM_type.h"

#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif

#ifndef NULL
  #define NULL (void*)0
#endif

#define low(x) ((u8)x & 0xFF)
#define high(x) ((u8)(x >> 8) & 0xFF)

//asm order
#define HTONL(x, y)   *(u8*)(x) = y & 0xFF; \
                      *((u8*)(x)+1) = y >> 8; \
                      *((u8*)(x)+2) = y >> 16; \
                      *((u8*)(x)+3) = y >> 24

#define SET_LE32(x, y)  *((u8*)(x)+3) = (y) & 0xFF; \
                        *((u8*)(x)+2) = (y >> 8) & 0xFF; \
                        *((u8*)(x)+1) = (y >> 16) & 0xFF; \
                        *((u8*)(x)+0) = (y >> 24) & 0xFF

#define SET_BE32(x, y)  *((u8*)(x)+0) = y; \
                        *((u8*)(x)+1) = y >> 8; \
                        *((u8*)(x)+2) = y >> 16; \
                        *((u8*)(x)+3) = y >> 24

#define SET_LE16(x, y)  *((u8*)(x)+0) = y; \
                        *((u8*)(x)+1) = y >> 8

#define GET_LE16(x)   ((*(((u8*)x)+0)) | \
                      ((*(((u8*)x)+1)) << 8) )

#define SET_BE16(x, y)  *((u8*)(x)+1) = y; \
                        *((u8*)(x)+0) = y >> 8

#define GET_BE16(x)   ((*(((u8*)x)+1)) | \
                      ((*(((u8*)x)+0)) << 8) )

#define GET_BE32(x)   ((*(((u8*)x)+3)) | \
                      ((*(((u8*)x)+2)) << 8) | \
                      ((*(((u8*)x)+1)) << 16) | \
                      ((*(((u8*)x)+0)) << 24) )                         

#define GET_LE32(x)   ((*(((u8*)x)+0)) | \
                      ((*(((u8*)x)+1)) << 8) | \
                      ((*(((u8*)x)+2)) << 16) | \
                      ((*(((u8*)x)+0)) << 24) )



//#define u8_t u8
//#define u16_t u16
//#define u32_t u32
