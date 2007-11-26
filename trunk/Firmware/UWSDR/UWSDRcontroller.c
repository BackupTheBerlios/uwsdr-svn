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

//************************* DEFINES ******************************************

//************************* MODULE VARIABLES *********************************

int m_datasock;
u16 m_framecounter;
u32 hostIP;

//************************* FUNKTIONS ****************************************

//****************************************************************************
// USDR_init
//
// Parameters: void
// Return type: void
// Inits the UWSDR controller
//
//****************************************************************************
void USDR_init(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

  hostIP = 0;
  m_framecounter = 0;
  m_datasock = UDP_create(0, _UDP_DATA_PORT, _UDP_MODE_DUPLEX, UWSDR_dispatch);

}



//****************************************************************************
// UWSDR_dispatch
//
// Parameters: u8* pData
// Return type: void
// Processes incomming UWSDR packets
//
//****************************************************************************
void UWSDR_dispatch(u8* pData, u32 length)
{
  //*** DEFINITON ***
  int ip[4];
  
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  
  switch(pData[0]) {
    /******* D **********/
    case 'D': switch(pData[1]) {
      case 'T': {
        
      }
    }
    break;
    
    /******* E **********/
    case 'E': switch(pData[1]) {
      case 'R': 
        /***** ET ******/
        if(pData[2] == '1') {
          delay_us(200000);
          m_framecounter = 0;
          CODEC_startRX();
        }
        if(pData[2] == '0') {
          CODEC_stop();
        }
      }
      break;
    /******* S **********/
    case 'S': switch(pData[1]) {
      case 'D':
        sscanf((const char*)&pData[2], "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
        hostIP = (ip[3]<<24) + (ip[2]<<16) + (ip[1]<<8) + ip[0];
        break;
      }
      break;
    /******* T *********/
    case 'T': switch(pData[1]) {
      case 'X':
        if(pData[2] == '1') {
          CODEC_startTX();
        }
        if(pData[2] == '0') {
          CODEC_stop();
        }
    }
  }
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
  
  //*** INITIALIZATION ***
  _DBG_STATE_POS(_DBG_STATE_UDP);

  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  pData = RING_consume();

  
  if(pData == NULL)
    return;

  DBG_LED1_ON();

  // increase sample frame counter
  SET_LE16(pData+2, m_framecounter);
  m_framecounter++;

  UDP_sendto(hostIP, _UDP_DATA_PORT, pData, _CODEC_FRAME_SIZE);

  if(m_framecounter > 0xFFFF)
    m_framecounter = 0;
  
  DBG_LED1_OFF();
  
}

