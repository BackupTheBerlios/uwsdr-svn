//****************************************************************************
//* ARP.h
//*
//* Address Resolution Protocol					
//*
//* Version 1.00						
//*
//* all source code by Tobias Weber (C) 2001,2002		
//****************************************************************

#ifndef __ARP_H__
#define __ARP_H__

//************************* INCLUDES *****************************************

#include "types.h"

//************************* DEFINES ******************************************

#define	ARP_OPCODE_REQUEST		0x0001
#define	ARP_OPCODE_REPLY		0x0002
#define ARP_IP_ADDR_SIZE                4

#define	ARP_PROTO_TYPE_ETH              0x0001

//#define	ARP_packetsize			46
#define	_ARP_FRAMESIZE			8*4

#define	ARP_PROTO_TYPE  		0
#define	ARP_PROTO_TYPE_H  		0
#define	ARP_PROTO_TYPE_L		1
#define	ARP_HW_TYPE			2
#define	ARP_HWtypeH			2
#define	ARP_HWtypeL			3
#define	ARP_HW_ADDR_LEN			4
#define	ARP_SW_ADDR_LEN                 5
#define	ARP_HDR_OPCODE    		6
#define	ARP_OPCODE_H    		6
#define	ARP_OPCODE_L			7
#define	ARP_SENDER_MAC             	8
#define	ARP_SENDER_IP			14
#define	ARP_TARGET_MAC		        18
#define	ARP_TARGET_IP			24

#define	_ARP_ERROR_NO_MAC		99
#define	_ARP_Timeout			20
#define _ARP_REQUEST_RETRIES            3

//;****************************************************************
//;*    definitions for the ARP cache space and location
#define	s_ARP_cache_IP			0
#define	s_ARP_cache_MAC			_IP_address_length
#define	_ARP_cache_line_length	        _IP_address_length + _DLC_address_length

#define	_ARP_CACHE_ENTRIES		2
#define	_ARP_cache_size			10*_ARP_cache_entries


//****************************************************************************
// ARP_init
// Initialises the ARP stack
//****************************************************************************
void ARP_init(void);

//****************************************************************************
// ARP_createMACforIP
// Creates a ethernet frame for a specified IP
//****************************************************************************
s16 ARP_createFrameforIP(u8* frame, u32 IP);

//****************************************************************************
// ARP_dispatch
// Processes incomming ARP requests
//****************************************************************************
s16 ARP_dispatch(u8* pData);

//****************************************************************************
// ARP_createReturnFrame
// 
//****************************************************************************
int ARP_createReturnFrame(u8* pRXFrame, u8* pTXFrame);

//****************************************************************************
// ARP_getMAC
// Gets a MAC for a IP
//****************************************************************************
u8* ARP_getMAC(u32 IP);


#endif //__ARP_H__
