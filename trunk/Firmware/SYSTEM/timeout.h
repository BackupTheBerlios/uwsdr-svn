//**************************************************************************
//
//  timeout.c
//    
//    Implements software timers. 
//    The user can regsister a custom function
//    for timeout callback functionality
//
// 
//  T.Weber 4.5.2006
//  SEBA-KMT Baunach, Gemany.
//**************************************************************************

#ifndef _TYPES_H_
#define _TYPES_H_

#include "types.h"

//****************************************************
// void TO_install
//****************************************************
u16 TO_register(u32 time, void(*CB)(void));

//****************************************************
// void TO_init
//****************************************************
void TO_init(void);

//****************************************************
// void TO_timerISR
//****************************************************
void TO_timerISR(void);

#endif //_TYPES_H_
