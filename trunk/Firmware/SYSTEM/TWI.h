//****************************************************************************
//*
//* TWI.h
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************

#ifndef __TWI_H__
#define __TWI_H__

//************************* INCLUDES *****************************************


//************************* DEFINES ******************************************


#include "Types.h"

//---------------------------------------------------------------------------
//----------------------------Konstanten-------------------------------------
//---------------------------------------------------------------------------
#define TWI_INTERRUPT_LEVEL 4



//---------------------------------------------------------------------------
//----------------------------Funktionen-------------------------------------
//---------------------------------------------------------------------------
//TWI initialisiern
void TWI_init(void);
//ini TWI initialisiern
void ini_TWI_Int(void);
//TWI WRITE Ready
bool TWI_WR_RD(void);
//TWI READ Ready
bool TWI_RD_RD(void);
//TWI WR Data
bool TWI_WR_DAT(const char *Dat, u16 Len, u16 TWI_ADR, void (*TxClbFunc)(void));
//TWI RD Data
bool TWI_RD_DAT_FIX(char *Dat, u16 Len, u16 TWI_ADR, void (*RxClbFunc)(void));
bool TWI_RD_DAT_VAR(char *Dat, u8 *Len, u8 LenMax, u16 TWI_ADR, void (*RxClbFunc)(void));
//Interrupt Routine
__arm void TWI_c_irq_handler(void);
//Datenbyte Senden
void TWI_WR_BYTE(void);
//Datenbyte Lesen
void TWI_RD_BYTE(void);

void TWI_c_irq_handler(void);

#endif //__TWI_H__
