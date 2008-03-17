
//****************************************************************
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//****************************************************************
//*									
//*  OSI Layer 2 universal driver for uwSDR                       
//*									
//*  DLC Data Link Layer	 Version 1.00				
//*									
//****************************************************************
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
//****************************************************************

#include "config.h"
#include "Emac.h"
#include "net.h"
#include "ARP.h"
#include "IP.h"
#include "UDP.h"

//************************ Defines *******************************************


//************************ Module Variables **********************************
u32* m_pRxBuf;
u16 m_RxLen;
u32* m_pTxBuf;
u16 m_TxLen;

//****************************************************************************


//****************************************************************************
// NET_init
//
// Parameters: u8* pRxBuf, u8* pTxBuf
// Return type: void
// Inits the network stack
//
//****************************************************************************
void NET_init(u32* pRxBuf, u32* pTxBuf)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  if(pRxBuf == NULL)
    return;
  if(pTxBuf == NULL)
    return;
  //*** PROGRAM CODE ***
  m_pRxBuf = pRxBuf;
  m_pTxBuf = pTxBuf;
  
  ARP_init();
  IP_init();
  UDP_init();
}


//****************************************************************************
// NET_process
//
// Parameters: u8 *inbuf
// Return type: s16
// Processes incomming ethernet frames on Data Link Level 
//
//****************************************************************************
s32 NET_process()
{
  //*** DEFINITON ***
  u16     us;
  s32     ret;
  u8      *pRX8, *pTX8;
  t_DLC   *pDLC;
  
  //*** PARAMETER CHECK ***
  if(m_pRxBuf == NULL)
    return _NET_NULL_POINTER;

  //*** INITIALIZATION ***
  ret = 0;
  pDLC = (t_DLC*)m_pRxBuf;
  pRX8 = (u8*)m_pRxBuf;
  pTX8 = (u8*)m_pTxBuf;

  _DBG_STATE_POS(_DBG_STATE_UDP);

  //*** PROGRAM CODE ***
  
  us = pDLC->frameType;
  
  switch(us)
  {
  case _DLC_TYPE_IP:
    ret = IP_dispatch(pRX8 + _DLC_HDR_SIZE);
    break;
  case _DLC_TYPE_ARP:
    ret = ARP_dispatch(pRX8 + _DLC_HDR_SIZE);
    break;
  default:
    return _NET_INVALID_FRAME;
  }
  
  // do we need to send anything ?
  if (ret > 0 && m_TxLen > 0)
    EMACSend(m_TxLen);
  
  return 0;
}

//****************************************************************************
// NET_CRC16
//
// Parameters: u16* pData, u16 length
// Return type: u16
// calculates a CRC16 checksum
//
//****************************************************************************
u16 NET_CRC16(u8* pData, u16 count)
{
  //*** DEFINITON ***
  register u32 checksum;
  int i;
    
  //*** INITIALIZATION ***
  checksum = 0;
  i = 0;
  
  //*** PARAMETER CHECK ***
  
  //*** PROGRAM CODE ***

  /* add 16-bit words */

  /* this is the inner loop */
  while(i < count) {
    checksum += (pData[i]*256 + pData[i+1]);
    i += 2;
  }

  /* add leftover byte, if any */
//  if (!(i % 2))
//    checksum += *(pData++);

  /* Fold 32-bit sum to 16-bit */
  while (checksum >> 16)
    checksum = (checksum & 0xffff) + (checksum >> 16);
  
  checksum = ~checksum;
  /*
  * Return one's compliment of final sum.
  */

  return checksum;
}


//****************************************************************************
// EMAC_copyRxPayload
//
// Parameters: u8* pTX, u8* pRX, u16 length
// Return type: void
// copies RX payload from Eth-Device to memory
//
//****************************************************************************
void NET_copyRxPayload(u8* pTX, u8* pRX, u16 length)
{
  //*** DEFINITON ***
  u32 offset;
  
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  offset = (u8*)m_pRxBuf - pRX;
  
  //EMAC_copyRxPayload(pTX, offset, length);
}

//****************************************************************************
// NET_sendFrame
//
// Parameters: u16 len
// Return type: int
// sends out a ethernet frame
//
//****************************************************************************
int NET_sendFrame(u16 len)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  EMACSend(len);

}

