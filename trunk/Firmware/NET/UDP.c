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
// UDP_dispatch
//
// Parameters: 
// Return type: int
// Processes incomming UDP frames
//
//****************************************************************************
s16 UDP_dispatch(u8* pRX)
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
      m_sockets[i].appcall(pRX + _UDP_HDR_PAYLOAD, len);
      return _NET_OK;
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
int UDP_create(u32 ip, u16 port, u8 mode, void(*appcall)(u8* pData, u32 length))
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
int UDP_sendto(u32 IP, u16 port, u8* pData, u32 size)
{
  //*** DEFINITON ***
  int result;
  u8* pUDPHDR;
  
  //*** INITIALIZATION ***
  u8* pTX = (u8*)m_pTxBuf;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  result = IP_createFrame(
                          pTX,
                          IP,
                          port,
                          size + _UDP_HDR_SIZE,
                          _IP_PROTTYPE_UDP
                          );
  
  if(result < _NET_OK)
    return result;
  
  //**** prepare UDP header ****
  pUDPHDR = (u8*)m_pTxBuf + result;
    
  SET_BE16(pUDPHDR + _UDP_HDR_SRC_PORT, port);
  SET_BE16(pUDPHDR + _UDP_HDR_DST_PORT, port);
  SET_BE16(pUDPHDR + _UDP_HDR_PACKET_LEN, size);
  //**** no checksum is calculated ****
  SET_BE16(pUDPHDR + _UDP_HDR_CKSUM, 0);
   
  //**** copy payload **** 
  memcpy (pUDPHDR + _UDP_HDR_PAYLOAD, pData, size);
  
  
  return IP_sendFrame(pTX, size + _UDP_HDR_SIZE + result);
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

