
//****************************************************************************
//*
//* net.h
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************

#ifndef __NET_H__
#define __NET_H__

//************************* INCLUDES *****************************************

#include "types.h"

//************************* CONFIGURATION ************************************

#define	_NET_IP_VERSION		4 //we are using IP v4 only

//************************* DEFINES ******************************************

#define _NET_GOT_RX_DATA         2
#define _NET_TX_REPLY            1
#define _NET_OK                  0
#define _NET_INVALID_FRAME       -1
#define _NET_WRONG_DESTINATION   -2
#define _NET_ARP_ERROR           -3
#define _NET_ARP_ERROR_TRY_AGN   -4
#define _NET_NULL_POINTER        -5
#define _NET_UNSUPP_PROTOCOL     -6
#define _NET_IP_TIMEOUT          -7
#define _NET_UNSUPP_MODE         -8
#define _NET_DEVICE_BUSY         -9
#define _NET_RESOURCE_EXCESS     -10
#define _NET_RETURN_THIS_FRAME   -32767
#define _NET_RETURN_THIS_IPFRAME -32766

#define	_DLC_ADDR_SIZE	       6
#define _DLC_HDR_SIZE          sizeof(t_DLC)

#define	_DLC_DST_ADDR		0
#define	_DLC_SRC_ADDR		6
#define	_DLC_ETHERTYPE		12

#define _NET_MIN_PACKET_SIZE    64

typedef struct {
  u8 dstMAC[_DLC_ADDR_SIZE];
  u8 srcMAC[_DLC_ADDR_SIZE];
  u16 frameType;
} t_DLC;

#define _NET_ETHERDEVICE        SAM7_EMAC

#define	_DLC_TYPE_IP		0x0008
#define	_DLC_TYPE_ARP		0x0608



#if (_NET_ETHERDEVICE == SAM7_EMAC)
#include "sam7_emac.h"
#endif

extern u32* m_pRxBuf;
extern u32* m_pTxBuf;
extern u16 m_RxLen;
extern u16 m_TxLen;

//****************************************************************************
// NET_init
// Inits the network stack
//****************************************************************************
void NET_init(u32* pRxBuf, u32* pTxBuf);

//****************************************************************************
// NET_process
// Processes incomming ethernet frames
//****************************************************************************
s32 NET_process(void);

//****************************************************************************
// NET_CRC16
// calculates a CRC16 checksum
//****************************************************************************
u16 NET_CRC16(u8* pData, u16 length);

//****************************************************************************
// EMAC_copyRxPayload
// copies RX payload from Eth-Device to memory
//****************************************************************************
void NET_copyRxPayload(u8* pTX, u8* pRX, u16 length);

//****************************************************************************
// NET_sendFrame
// sends out a ethernet frame
//****************************************************************************
int NET_sendFrame(u16 len);


#endif //__NET_H__
