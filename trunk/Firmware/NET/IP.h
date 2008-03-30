//****************************************************************************
//
// IP.h
//
//****************************************************************************

#ifndef __IP_H__
#define __IP_H__

//************************* INCLUDES *****************************************

#include "types.h"

#define	_OS_IP_TOS		0
#define _IP_ADDR_SIZE           4
#define _IP_DEFAULT_TTL         8


//****************************************************************
//*
//*  Layer 3 
//*  IP Definitions
//*
//****************************************************************

#define	_IP_PROTTYPE_ICMP		1
#define _IP_PROTTYPE_IGMP               2
#define	_IP_PROTTYPE_TCP		6
#define	_IP_PROTTYPE_EGP		8
#define	_IP_PROTTYPE_UDP		17

#define _IP_GET_HEADER_LENGTH(versionfield)   ((versionfield & 0x0F) * 4)

//***** IP FRAME LAYOUT ****

#define _IP_HDR_VERSION           0
#define _IP_HDR_SERVICETYPE       1
#define _IP_HDR_TOTLEN            2
#define _IP_HDR_ID                4
#define _IP_HDR_FRAGMENT          6
#define _IP_HDR_TTL               8
#define _IP_HDR_PROTOCOL          9
#define _IP_HDR_HDRCKSUM          10
#define _IP_HDR_SRC_IP            12
#define _IP_HDR_DST_IP            16
#define _IP_HDR_OPTION            20

#define _IP_FRAGMENT_DONTFRAG     (1 << 14)
#define _IP_FRAGMENT_MOREFRAG     (1 << 13)

#define _IP_HDR_SIZE              _IP_HDR_OPTION

#define IP_GET_SEQ_NR()           (m_IPseqNr++)
#define IP_GET_SEQ_NR_NO_INCR()   (m_IPseqNr)


typedef struct s_socket {
  s32(*appcall)(u8* pData, u32 length);
  u16 localport;
  u16 destport;
  u8 address[_IP_ADDR_SIZE];
  u8 state;
} t_socket;

//****************************************************************************
// IP_dispatch
// Processes an incoming IP frame
//****************************************************************************
int IP_dispatch(u8* pRX);

//****************************************************************************
// IP_createReturnFrame
// returns the incomming IP frame to sender
//****************************************************************************
int IP_createReturnFrame(u8* pTX, u8* pRX, u16 newSize);

//****************************************************************************
// IP_createFrame
// creates an IP frame header
//****************************************************************************
int IP_createFrame(u8* pIPFrame, u32 IP, u16 port, u16 length, u8 proto, u16 fragment);

//****************************************************************************
// IP_init
// Initialises the IP layer
//****************************************************************************
void IP_init(void);

//****************************************************************************
// IP_getLocalIP
// Returns a byte pointer to the local IP address in big endian format
//****************************************************************************
u8* IP_getLocalIP(void);

//****************************************************************************
// IP_sendFrame
// sends out a prepared frame
//****************************************************************************
int IP_sendFrame(u8* pFrame, u16 len);


#endif //__IP_H__
