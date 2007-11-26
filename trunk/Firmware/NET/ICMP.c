//*																*
//* all source code by Tobias Weber (C) 2001,2002				*
//****************************************************************
//****************************************************************
//****************************************************************

#include "ICMP.h"
#include "net.h"
#include <string.h>

//****************************************************************************
// ICMP_process
//
// Parameters: u8* pFrame
// Return type: s16
// Processes an incomming ICMP packet
//
//****************************************************************************
int ICMP_process(u8* pRX, u8* pTX, int length)
{
  //*** DEFINITON ***
  u16 checksum;
  
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  m_TxLen = length;
  
  // we only answer to echo requests
  if(pRX[_ICMP_HDR_TYPE] != _ICMP_ECHO)
    return _NET_WRONG_DESTINATION;
  
  memcpy(pTX, pRX, length);
  
  //NET_copyPayload(pTX, pRX, length);
  
  // set to reply type
  pTX[_ICMP_HDR_TYPE] = _ICMP_ECHO_REPLY;

  // calc checksum
  SET_BE16(&pTX[_ICMP_HDR_CKSUM], 0);
  checksum = NET_CRC16(pTX, length);
  SET_BE16(&pTX[_ICMP_HDR_CKSUM], checksum);
  
  return _NET_RETURN_THIS_IPFRAME;
}




//ICMP_process_request:
//
//	rcall	LAN_RX_16Yi
//
//	cpi	a,_ICMP_echo
//	breq	ICMP_process_echo
//	rjmp	IP_process_end
//
//ICMP_process_echo:
//	mov	d,r_RX_framecountH
//	mov	c,r_RX_framecountL
//	lsr	d
//	ror	c
//	rcall	LAN_RX_16Yic
//
//	moveY	r_TCP_startH,r_TCP_startL
//
//	ldi	b,_ICMP_echo_reply
//	st	Y,b			;Y point to start of ICMP frame
//
//	;clr	b			;this one we can drop, because Echo Reply Code is already 0
//	std	Y+2,b
//	std	Y+3,b
//
//	mov16rr	a,w,r_RX_frameCountH,r_RX_frameCountL	
//	mov16rr	r_TX_frameCountH,r_TX_frameCountL,a,w
//	clr_RX_Framecount
//
//	mov	ZL,YL		;calculate ICMP Checksum
//	mov	ZH,YH
//	rcall	calc_crc32
//	mov	YL,ZL
//	mov	YH,ZH
//	adiw	YL,_ICMP_CRC_offset
//	st	Y+,c		;and store it into reply frame
//	st	Y,b
//
//	;inform lower layer, that packet can be deleted
//	rjmp	IP_TX_return_frame
