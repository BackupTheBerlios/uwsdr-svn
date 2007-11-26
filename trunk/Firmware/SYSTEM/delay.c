//************************************************************************
// Projekt:	EBOX-USB
//
// File:	main.c
//
// Author:	Tobias Weber, Timo Riemer	
//
// Beschreibg: Firmware für EBOX USB
//
// Copyright (c) SEBA KMT 2006 Baunach, Germany
//***********************************************************************


//******************************* INCLUDES ***********************************
#include  "config.h"
#include  "delay.h"

//**************************************************************************
// NAME: delay_us
//
// DESC: Kalibierter Delay für Microsekunden
//      
// PARAM: u32 Factor
//      
// RETVAL: void
//         TRUE if successful, FALSE if failed.
//**************************************************************************
void delay_us(u32 Factor)
{
  //***** DECLARATION *****
  u32 i,j;
  //***** INITIALZATION *****
  //***** RANGE CHECK *****
  //***** PROGRAM *****
  Factor = Factor * _DELAY_CORRECTION; //_DELAY_CORRECTION;
  
  for (i = 0; i < Factor; i++)
    for (j = 0; j < _DELAY_COEFF; j++) {
      NOP();
      NOP();
      NOP();
   }
}

