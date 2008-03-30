//****************************************************************************
//*
//* UWSDRcontroller.c
//*
//* (c) 2007, Tobias Weber, tw@suprafluid.com 
//****************************************************************************


//************************* INCLUDES *****************************************

#include "UWSDRcontroller.h"
#include "types.h"
#include "codec.h"
#include "UDP.h"
#include "ring.h"
#include "delay.h"
#include <stdio.h>
#include <string.h>

//************************* DEFINES ******************************************

#define _UWSDR_RX_OK      0
#define _UWSDR_RX_ON      1

//************************* MODULE VARIABLES *********************************

int m_datasock;
//int m_ctrlsock;

u16 m_framecounter;
u16 m_fragmentcounter;
u32 hostIP;
u32 m_state;

//************************* FUNKTIONS ****************************************

//****************************************************************************
// UWSDR_init
//
// Parameters: void
// Return type: void
// Inits the UWSDR controller
//
//****************************************************************************
void UWSDR_init(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

  hostIP = 0;
  m_framecounter = 0;
  m_fragmentcounter = 0;
  m_state = 0;
  //m_ctrlsock = UDP_create(0, _UDP_CTRL_PORT, _UDP_MODE_LISTENING, UWSDR_dispatch);
  m_datasock = UDP_create(0, _UDP_CTRL_PORT, _UDP_MODE_DUPLEX, UWSDR_dispatch);

}

static s32 USWDR_ackn(u8* pFrame, u8* pCmd)
{
  static u8 cmdBuf[6] = {'A', 'K', 'x', 'x', ';', 0};
  
  memcpy(cmdBuf + 2, pCmd, 2);
  return UDP_reply(pFrame, cmdBuf, sizeof(cmdBuf));
}


//****************************************************************************
// UWSDR_dispatch
//
// Parameters: u8* pData
// Return type: void
// Processes incomming UWSDR packets
//
//****************************************************************************
s32 UWSDR_dispatch(u8* pFrame, u32 length)
{
  //*** DEFINITON ***
  int ip[4];
  u16 us;
  u8* pNext;
  u8* pData;
  
  //*** INITIALIZATION ***
  pData = pFrame + _UDP_HDR_PAYLOAD;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  switch(pData[0]) {
    /******* C **********/
    case 'C': switch(pData[1]) { 
      /***** CF ******/
      case 'F':
        return USWDR_ackn(pFrame, pData);
        
    }
    /******* D **********/
    case 'D': switch(pData[1]) {
      /***** DT ******/
      case 'T': {
        // get sequence number
        us = GET_LE16(&pData[2]);
        if(us < m_framecounter - 4) {
          m_framecounter = us;
        }
        else if(us < 4) {
          m_framecounter = us;
        }
        else break;
        
        pNext = RING_produce();
        if(pNext != NULL) {
          memcpy(pNext, &pData[6], length - 6);
        }
      }
    }
    break;
    
    /******* E **********/
    case 'E': switch(pData[1]) {
      case 'R': 
        /***** ER ******/
        if(pData[2] == '1') {
          hostIP = GET_LE32(pFrame - _IP_HDR_SIZE + _IP_HDR_SRC_IP);
          SET_FLAG(m_state, _UWSDR_RX_OK);
          m_framecounter = 0;
          CODEC_startRX();
          return USWDR_ackn(pFrame, pData);
        }
        if(pData[2] == '0') {
          CODEC_stop();
          return USWDR_ackn(pFrame, pData);
        }
      break;
      case 'T': 
        /***** ET ******/
        if(pData[2] == '1') {
          return USWDR_ackn(pFrame, pData);
        }
      break;
    }
    /******* F **********/
    case 'F': switch(pData[1]) {
      case 'R': 
        /***** FR ******/
        if(TEST_FLAG(m_state, _UWSDR_RX_OK) && !TEST_FLAG(m_state, _UWSDR_RX_ON)) {
          SET_FLAG(m_state, _UWSDR_RX_ON);
        }
        return USWDR_ackn(pFrame, pData);
    }
    /******* S **********/
    case 'S': switch(pData[1]) {
      case 'D':
//        sscanf((const char*)&pData[2], "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
//        hostIP = (ip[3]<<24) + (ip[2]<<16) + (ip[1]<<8) + ip[0];
        break;
      }
      break;
    /******* T *********/
    case 'T': switch(pData[1]) {
      case 'X':
        if(pData[2] == '1') {
          CODEC_startTX();
          return USWDR_ackn(pFrame, pData);
        }
        if(pData[2] == '0') {
          CODEC_stop();
          return USWDR_ackn(pFrame, pData);
        }
    }
  }
  return _NET_ARP_ERROR;
}



//****************************************************************************
// UWSDR_upload
//
// Parameters: 
// Return type: 
// 
//
//****************************************************************************
void UWSDR_upload()
{
  //*** DEFINITON ***
  u8    *pData;
  t_codec_hdr*  pHdr;
  int   TXlen;

  //*** INITIALIZATION ***
  _DBG_STATE_POS(_DBG_STATE_UDP);
  TXlen = _CODEC_DATABLOCK_SIZE;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  //get the oldest data from the ring
  pData = RING_consume();

  // is there data ?
  if(pData == NULL)
    return;

  DBG_LED1_ON();

  //if this is the first frame within the fragment
  if(m_fragmentcounter == 0) {
    //create the uWSDR data header for this frame in front of the data
    pHdr = (t_codec_hdr*) (pData - _CODEC_HEADER_SIZE);
    pHdr->type_char0 = 'D';
    pHdr->type_char1 = 'R';
    pHdr->seqNr = m_framecounter;
    pHdr->seqLen = _CODEC_SAMPLES_PER_BLOCK * _CODEC_BLOCKS_PER_FRAME / 2;
    pHdr->agc = 0x01;
    pHdr->dummy = 0x02;
    //correct buffer sizes and position for the header inclusion
    pData = (u8*)pHdr;
    TXlen += _CODEC_HEADER_SIZE;
  // increase sample frame counter
    m_framecounter += 2;
  }

  UDP_sendto(
             hostIP,
             _UDP_CTRL_PORT,
             pData,
             _CODEC_FRAME_SIZE + _CODEC_HEADER_SIZE,
             TXlen,
             m_fragmentcounter
             );
  
  //increase the fragment pointer
  m_fragmentcounter += TXlen;
  
  if(m_fragmentcounter == (_CODEC_FRAME_SIZE + _CODEC_HEADER_SIZE)) {
    //reset fragment counter
    m_fragmentcounter = 0;
  }

  if (m_framecounter > 0xFFFF) {
    if ((m_framecounter % 2) == 0)
      m_framecounter = 1;
    else
      m_framecounter = 0;
  }
//  if(m_framecounter > 0xFFFF)
//    m_framecounter = 0;
  
  DBG_LED1_OFF();
  
}

