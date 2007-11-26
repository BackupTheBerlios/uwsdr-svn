//****************************************************************************
//*
//* ICMP.h
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************

#ifndef __ICMP_H__
#define __ICMP_H__

//************************* INCLUDES *****************************************

#include "types.h"

//************************* DEFINES ******************************************


#define _ICMP_HDR_TYPE    0
#define _ICMP_HDR_CODE    1
#define _ICMP_HDR_CKSUM   2
#define _ICMP_HDR_ID      4
#define _ICMP_HDR_SEQNR   6
#define _ICMP_HDR_PAYLOAD 8
#define _ICMP_HDRSIZE     8


#define	_ICMP_ECHO			8
#define	_ICMP_ECHO_REPLY		0

//****************************************************************************
// ICMP_process
// Processes an incomming ICMP packet
//****************************************************************************
int ICMP_process(u8* pRX, u8* pTX, int length);


#endif //__ICMP_H__

