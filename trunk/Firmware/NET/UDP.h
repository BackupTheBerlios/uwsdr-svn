//****************************************************************************
//*
//* UDP.h
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************

#ifndef __UDP_H__
#define __UDP_H__

//************************* INCLUDES *****************************************

#include "config.h"
#include "types.h"
#include "net.h"
#include "IP.h"
#include "UDP.h"

//************************* DEFINES ******************************************

#define _UDP_MAX_SOCKETS     2

#define _UDP_MODE_LISTENING  0
#define _UDP_MODE_OPEN_CONN  1
#define _UDP_MODE_DUPLEX     2

#define _UDP_HDR_SRC_PORT     0
#define _UDP_HDR_DST_PORT     2
#define _UDP_HDR_PACKET_LEN   4
#define _UDP_HDR_CKSUM        6
#define _UDP_HDR_PAYLOAD      8

#define _UDP_HDR_SIZE         _UDP_HDR_PAYLOAD


//****************************************************************************
// UDP_dispatch
// Processes incomming UDP frames
//****************************************************************************
s16 UDP_dispatch(u8* pRX);

//****************************************************************************
// UDP_sendto
// Sends a UDP Packet so the given address
//****************************************************************************
int UDP_sendto(u32 IP, u16 port, u8* pData, u32 size);

//****************************************************************************
// UDP_create
// creates a UDP socket
//****************************************************************************
int UDP_create(u32 ip, u16 port, u8 mode, void(*appcall)(u8* pData, u32 length));

//****************************************************************************
// UDP_init
// initialises the UDP part of the stack
//****************************************************************************
void UDP_init(void);

#endif //__UDP_H__
