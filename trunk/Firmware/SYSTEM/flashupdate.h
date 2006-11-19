//****************************************************************************
// Projekt:	
//
// File:	flashupdate
//
// Author:	Tobias Weber
//
// Version: 1.00A
//
// Beschreibung:
//
// Copyright (c) SEBA KMT 2006 Baunach, Germany
//****************************************************************************
#ifndef _FLASHUPDATE_H_
#define _FLASHUPDATE_H_

// ************************** GOBAL CONFIG INCLUDE ***************************
#include  "config.h"
#include "flash.h"

//******************************* DEFINES ************************************

#define _FU_BULK_SIZE   16
#define	 ERASE_VALUE 		0xFFFFFFFF

//************************* FUNCTION PROTOTYPES ******************************

//**************************************************************************
// flash_update
// Updates program flash. If it failed, it returns, else it performs a reset.
//**************************************************************************
void FU_perform_update(u8* pData);



#endif //_FLASHUPDATE_H_
