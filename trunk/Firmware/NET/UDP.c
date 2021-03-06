//****************************************************************************
//*
//* UDP.c
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************


//************************* INCLUDES *****************************************

#include "UDP.h"
#include "IP.h"
#include "ARP.h"
#include <string.h>

//************************* DEFINES ******************************************



//************************** MODULE VARIABLES *******************************

t_socket m_sockets[_UDP_MAX_SOCKETS];


//************************* FUNCTIONS ****************************************

//****************************************************************************
// UDP_reply
//
// Parameters: u8* pData, u32 len
// Return type: void
// Returns data to a incoming UDP sender
//
//****************************************************************************
s32 UDP_reply(u8* pRXUDP, u8* pData, u32 len)
{
  //*** DEFINITON ***
  //int   frameSize;
  u16   destport, crc;
  u8*   pTX;
  
  //*** INITIALIZATION ***
  pTX = (u8*)m_pTxBuf;
    
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

  destport = GET_BE16(pRXUDP + _UDP_HDR_DST_PORT);
 
  pTX += IP_createReturnFrame(pTX, (u8*)m_pRxBuf, len + _UDP_HDR_SIZE);

  SET_BE16(pTX + _UDP_HDR_SRC_PORT, destport);
  SET_BE16(pTX + _UDP_HDR_DST_PORT, destport);
  SET_BE16(pTX + _UDP_HDR_PACKET_LEN, len + _UDP_HDR_SIZE);
  SET_BE16(pTX + _UDP_HDR_CKSUM, 0);
  
  pTX += _UDP_HDR_PAYLOAD;
  
  // add payload  
  memcpy(pTX, pData, len);
  m_TxLen += (len + _UDP_HDR_SIZE);
  
  return _NET_RETURN_THIS_IPFRAME;
}


//****************************************************************************
// UDP_dispatch
//
// Parameters: 
// Return type: int
// Processes incomming UDP frames
//
//****************************************************************************
s32 UDP_dispatch(u8* pRX)
{
  //*** DEFINITON ***
  int len, i;
  u16 destport;
  
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  destport = GET_BE16(pRX + _UDP_HDR_DST_PORT);
 
  for(i = 0; i < _UDP_MAX_SOCKETS; i++) {
    if(m_sockets[i].localport == destport) {
      len = GET_BE16(pRX + _UDP_HDR_PACKET_LEN);
      return m_sockets[i].appcall(pRX, len);
    }
  }
  if(i == _UDP_MAX_SOCKETS)
    return _NET_RESOURCE_EXCESS;

  
  return _NET_OK;
}



//****************************************************************************
// UDP_create
//
// Parameters: u32 ip, u16 port, u16 mode
// Return type: t_socket
// creates a UDP socket, returns a socket struct
//
//****************************************************************************
int UDP_create(u32 ip, u16 port, u8 mode, s32(*appcall)(u8* pData, u32 length))
{
  //*** DEFINITON ***
  int i;
  //*** INITIALIZATION ***
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  // search for free socket
  for(i = 0; i < _UDP_MAX_SOCKETS; i++) {
    if((m_sockets[i].localport == 0) &&
       (m_sockets[i].destport == 0)) {
         break;
    }
  }
  if(i == _UDP_MAX_SOCKETS)
    return _NET_RESOURCE_EXCESS;
  
  if(mode == _UDP_MODE_LISTENING || mode == _UDP_MODE_DUPLEX) {
    m_sockets[i].appcall = appcall;
    m_sockets[i].state = (u8)mode;
    m_sockets[i].localport = port;
    return _NET_OK;
  }
  
  if(mode == _UDP_MODE_DUPLEX){
    m_sockets[i].destport = port;
  }


  return _NET_UNSUPP_MODE;
}

//****************************************************************************
// UDP_sendto
//
// Parameters: t_socket* sock, u8* pTX, u16 size
// Return type: int
// Sends a UDP Packet so the given address
//
//****************************************************************************
int UDP_sendto(u32 IP, u16 port, u8* pData, u32 totalSize, u32 frameSize, u32 offset)
{
  //*** DEFINITON ***
  int result;
  u8* pUDPHDR;
  u16 fragment;  
  
  //*** INITIALIZATION ***
  u8* pTX = (u8*)m_pTxBuf;
  
  //*** PARAMETER CHECK ***
  if(offset > totalSize - frameSize)
    return _NET_INVALID_FRAME;
     
  //*** PROGRAM CODE ***
  
  // are there more fragments to come
  if(offset + frameSize < totalSize)
    fragment = (offset >> 3) + _IP_FRAGMENT_MOREFRAG;
  else {
    fragment = (offset >> 3);
  }

  if(offset == 0) {
    //the first frame is 8 bytes longer because of the UDP header
    frameSize += _UDP_HDR_SIZE;
  }
  else {
    //the later packets must be shifted by 8 because the caller 
    //is not aware of the UDP header length
    fragment += _UDP_HDR_SIZE/8;
  }

  result = IP_createFrame(
                          pTX,
                          IP,
                          port,
                          frameSize,
                          _IP_PROTTYPE_UDP,
                          fragment
                          );
  
  //the first frame is a bit greater because the UDP header is included
  if(offset == 0) {

    //**** prepare UDP header ****
    pUDPHDR = pTX + result;
      
    SET_BE16(pUDPHDR + _UDP_HDR_SRC_PORT, port);
    SET_BE16(pUDPHDR + _UDP_HDR_DST_PORT, port);
    SET_BE16(pUDPHDR + _UDP_HDR_PACKET_LEN, totalSize + _UDP_HDR_SIZE);
    //**** no checksum is calculated ****
    SET_BE16(pUDPHDR + _UDP_HDR_CKSUM, 0);
   
    //**** copy payload **** 
    memcpy (pUDPHDR + _UDP_HDR_PAYLOAD, pData, frameSize);
  }
  else {
    memcpy (pTX + result, pData, frameSize);
  }
  
  
  if(result < _NET_OK)
    return result;
  
  return IP_sendFrame(pTX, frameSize + _UDP_HDR_SIZE + result);
}


//****************************************************************************
// UDP_init
//
// Parameters: void
// Return type: void
// initialises the UDP part of the stack
//
//****************************************************************************
void UDP_init(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  memset(m_sockets, 0, sizeof(m_sockets));
}

