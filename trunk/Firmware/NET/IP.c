
//****************************************************************
//****************************************************************
//****************************************************************
//*								
//*  IP Implementation for OS8515				
//*								
//* Version 1.00						
//*								
//* created		 23.12.2001				  
//* last modified 05.09.2001					  
//*									
//* all source code by Tobias Weber (C) 2001,2002			
//****************************************************************
//****************************************************************
//****************************************************************

//***** includes ******

#include "config.h"
#include "net.h"
#include "IP.h"
#include "ICMP.h"
#include "UDP.h"
#include "ARP.h"
#include <string.h>

//***** defines ******



#define	_IP_versionmask			= 0b11110000
#define	_IP_headsizemask		= 0b00001111

#define	_IP_headerlen_NOOPT		= 20
#define	_IP_default_totlen		= 64
#define	_IP_Dont_Fragment		= 0x40
#define	_IP_TTL_startvalue		= 128
#define	_IP_Prottype_TCP		= 6


//#define	f_IP_header			= _DLC_headersize
//#define	f_IP_version			= 0x0000
//#define	f_IP_TOS			= 0x0001
//#define	f_IP_framesize			= 0x0002
//#define	f_IP_ID				= 0x0004
//#define	f_IP_Flags			= 0x0006
//#define	f_IP_frameoffset		= 0x0007
//#define	f_IP_TTL			= 0x0008
//#define	f_IP_protocol			= 0x0009
//#define	f_IP_header_CRC			= 0x000A
//#define	f_IP_sourceIP			= 0x000C
//#define	f_IP_destIP			= 0x0010


//************************** MODULE VARIABLES *******************************

u32 m_localIP; 
u16 m_IPseqNr;



//****************************************************************
//*	Sets length of IP-Header									*
//****************************************************************

//.macro	IP_set_TX_length
//	ldi	@1,high(@0)
//	mov	r_TX_framecountH,@1
//	sts	m_frameheader_TX+f_IP_header+f_IP_framesize+0,@1
//	ldi	@1,low(@0)
//	mov	r_TX_framecountL,@1
//	sts	m_frameheader_TX+f_IP_header+f_IP_framesize+1,@1
//.endmacro

//****************************************************************************
// IP_process
//
// Parameters: u32* pFrame
// Return type: s32
//
// Processes an incomming IP frame. We require 32 Bit pointer to aware 32
// Bit alignment.
//
//****************************************************************************
int IP_dispatch(u8* pRX)
{
  //*** DEFINITON ***
  volatile u32 ul;
  u16 us; //unsiged short (16 bit)
  s32 ret;
  u8  hdrLen; //unsigned chort (one byte)
  
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
 
  //******** IP version check *************************
  if((pRX[_IP_HDR_VERSION] & 0xF0) != (_NET_IP_VERSION << 4))
    return _NET_UNSUPP_PROTOCOL;
  
  //******** IP Header length calculation *************
  hdrLen = _IP_GET_HEADER_LENGTH(pRX[_IP_HDR_VERSION]); 

  //*** is this frame for my IP ? ***
  ul = GET_BE32(&pRX[_IP_HDR_DST_IP]);
  if(  ul != _IP_MYIPADDR )
    return _NET_WRONG_DESTINATION;
    
  if(pRX[_IP_HDR_TTL] == 0)
    return _NET_IP_TIMEOUT;
  

  switch(pRX[_IP_HDR_PROTOCOL]) {
    //***** Handling of ICMP frames *****
    case _IP_PROTTYPE_ICMP:
      ret = ICMP_process(
                         pRX + hdrLen,
                         (u8*)m_pTxBuf + _DLC_HDR_SIZE + hdrLen,
                         GET_BE16(&pRX[_IP_HDR_TOTLEN]) - hdrLen
                        );
      break;
    //***** Handling of UDP frames *****
    case _IP_PROTTYPE_UDP:
      ret = UDP_dispatch(pRX + hdrLen);
      break;
      
    default:
      return _NET_UNSUPP_PROTOCOL;
  }
  
  if(ret == _NET_RETURN_THIS_IPFRAME) {
    //**** calculate IP header Checksum ****  
    us = NET_CRC16((u8*)m_pTxBuf + _DLC_HDR_SIZE, _IP_HDR_SIZE);
    SET_BE16((u8*)m_pTxBuf + _DLC_HDR_SIZE + _IP_HDR_HDRCKSUM, us);
    return _NET_TX_REPLY;
  }
  
  return ret;
}

//****************************************************************************
// IP_createReturnFrame
//
// Parameters: u8* pTXFrame, u8* pRXFrame
// Return type: s32
// returns the incomming IP frame to sender
//
//****************************************************************************
int IP_createReturnFrame(u8* pTX, u8* pRX, u16 newSize)
{
  //*** DEFINITON ***
  u8  *pIPTXFrame, *pIPRXFrame;
  int  frameSize;
  
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  // ask the lower layer to create a return frame
  frameSize = ARP_createReturnFrame(pTX, pRX);

  pIPTXFrame = pTX + frameSize;
  pIPRXFrame = pRX + frameSize;
  
  memcpy(pIPTXFrame, pIPRXFrame, 12); // copy everything that doesnt change
  //readjust frame size
  SET_BE16(&pIPTXFrame[_IP_HDR_TOTLEN], frameSize + _IP_HDR_SIZE);
  // exchange IPs
  memcpy(&pIPTXFrame[_IP_HDR_DST_IP], &pIPRXFrame[_IP_HDR_SRC_IP], _IP_ADDR_SIZE);
  SET_LE32(&pIPTXFrame[_IP_HDR_SRC_IP], _IP_MYIPADDR);
  // reset old checksum
  SET_BE16(pIPTXFrame + _IP_HDR_HDRCKSUM, 0); // first reset old checksum

  m_TxLen += _IP_HDR_SIZE;
  frameSize += _IP_HDR_SIZE;
  
  return frameSize;
}

//****************************************************************************
// IP_createFrame
//
// Parameters: u8* pIPFrame, u32 IP, u16 port
// Return type: int
// creates an IP frame header
// returns negative, if error occured, or else, the length of the prepared header
//****************************************************************************
int IP_createFrame(u8* pFrame, u32 IP, u16 port, u16 length, u8 proto)
{
  //*** DEFINITON ***
  int result;
  u8* pIPFrame;
  
  //*** INITIALIZATION ***
  pIPFrame = pFrame + _DLC_HDR_SIZE;
  
  //*** PARAMETER CHECK ***
  if(pFrame == NULL)
    return _NET_NULL_POINTER;
  
  //*** PROGRAM CODE ***
  
  result = ARP_createFrameforIP((u8*)pFrame, IP);
  
  if(result < _NET_OK)
    return result;
  
  pIPFrame[_IP_HDR_VERSION] = (_NET_IP_VERSION<<4) + (_IP_HDR_SIZE>>2);
  pIPFrame[_IP_HDR_SERVICETYPE] = 0; //standard TOS
  //**** total length (incl. IP header ****
  SET_BE16(&pIPFrame[_IP_HDR_TOTLEN], length + _IP_HDR_SIZE);
  //**** sequential identification ****
  SET_BE16(&pIPFrame[_IP_HDR_ID], IP_GET_SEQ_NR());
  //**** Flags and offset for fragmentation ****
  SET_BE16(&pIPFrame[_IP_HDR_FLAGS], 0);
  //**** Time To Live TTL *****
  pIPFrame[_IP_HDR_TTL] = _IP_DEFAULT_TTL;
  //**** Protocol ****
  pIPFrame[_IP_HDR_PROTOCOL] = proto;
  //**** Checksum is calculated in IP_send ****
  pIPFrame[_IP_HDR_HDRCKSUM + 0] = 0;
  pIPFrame[_IP_HDR_HDRCKSUM + 1] = 0;

  //**** set my IP as sender ****  
  SET_LE32(pIPFrame + _IP_HDR_SRC_IP, _IP_MYIPADDR);

  //**** set sender IP address ****
  SET_BE32(pIPFrame + _IP_HDR_DST_IP, IP);
  
  return _DLC_HDR_SIZE + _IP_HDR_SIZE;
}


//****************************************************************************
// IP_init
//
// Parameters: void
// Return type: void
// Initialises the IP layer
//
//****************************************************************************
void IP_init(void)
{
  //*** DEFINITON ***
  u8* bp;
  
  //*** INITIALIZATION ***
  bp = (u8*)&m_localIP;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  m_IPseqNr = 0;
  
#ifdef _BIG_ENDIAN_MODE_
#error "Not Implemented"
#endif

#ifdef _LITTLE_ENDIAN_MODE_
 bp[0] = _IP_ADDR_0;

#endif
}


//****************************************************************************
// IP_getLocalIP
//
// Parameters: void
// Return type: u8*
// Returns a byte pointer to the local IP address in big endian format
//
//****************************************************************************
u8* IP_getLocalIP(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  return (u8*)&m_localIP;
}


//****************************************************************************
// IP_sendFrame
//
// Parameters: u8* pFrame
// Return type: int
// sends out a prepared frame
//
//****************************************************************************
int IP_sendFrame(u8* pFrame, u16 len)
{
  //*** DEFINITON ***
  u8* pIPHDR;
  u16 crc;
  
  //*** INITIALIZATION ***
  pIPHDR = pFrame + _DLC_HDR_SIZE;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

  //**** calculate IP header Checksum ****  
  crc = NET_CRC16(pIPHDR, _IP_HDR_SIZE);
  SET_BE16(pIPHDR + _IP_HDR_HDRCKSUM, crc);

  return NET_sendFrame(len);
}




//IP_TX_return_frame:
//
//	clr	a
//	lds	b,m_IP_RX_headersize	;add IP-header len
//	add	r_TX_frameCountL,b
//	adc	r_TX_frameCountH,a
//
//	moveY	r_IP_startH,r_IP_startL
//	stdwY	f_IP_framesize,r_TX_frameCountH,r_TX_frameCountL
//
//	cpy32yy	f_IP_destIP,f_IP_sourceIP,w,w,w,w
//
//	moveZ	r_IP_startH,r_IP_startL
//
//	adiw	ZL,f_IP_sourceIP
//	rcall	IP_set_my_IP_at_Zi
//
//	clr	a
//	moveY	r_IP_startH,r_IP_startL
//	stdwY	f_IP_header_CRC,a,a		;store CRC
//	
//	lds	w,m_IP_RX_headersize
//	sts	m_IP_TX_headersize,w
//
//	rcall	calc_crc32			;calc IP CRC32
//	
//	moveY	r_IP_startH,r_IP_startL
//	stdwY	f_IP_header_CRC,c,b		;store CRC
//
//	rjmp	DLC_TX_return_frame

//****************************************************************
//*																*
//* IP_TX_create_frame											*
//*																*
//****************************************************************
//*																*
//* PARAMETER:													*
//* a,b,c,d = IP-Number of Destination Node.						*
//*																*
//* creates a emtpy frame header. This prepares the frame to be 	*
//* transmitted by IP_TX_Frame later on.							*
//*																*
//* Class AA Routine												*
//****************************************************************

//;Requires: a,b,c,d as destination IP
//;offers:   r_TCP_start, Z points to Start of IP
//IP_TX_create_frame:
//
//	//************** set Z to start DLC Frametype ************
//	ldiZ	m_frameheader_TX+(_DLC_headersize-2)
//
//	//**************** set DLC Type to IP ********************
//	ldi	w,high(_DLC_type_IP)	
//	st	Z+,w
//	ldi	w,low(_DLC_type_IP)
//	st	Z+,w
//	
//	//*************** Store Dest-IP **************************
//	stdqZ	f_IP_destIP,a,b,c,d
//
//	//************* set Version & Header length **************
//
//	stdiZ	f_IP_version,(OS_IP_VERSION*0x10)|(_IP_headerlen_NOOPT/4),w
//	ldi	w,high(m_frameheader_TX+_IP_headerlen_NOOPT+_DLC_headersize)
//	mov	r_TCP_startH,w
//	ldi	w,low(m_frameheader_TX+_IP_headerlen_NOOPT+_DLC_headersize)
//	mov	r_TCP_startL,w
//
//	stsib	m_IP_TX_headersize,_IP_headerlen_NOOPT
//
//	//************* set Type of Service Flag *****************
//	stdiZ	f_IP_TOS,OS_IP_TOS,w
//	
//	//************* Set ID startvalue ************************
//	lds	w,m_IP_SN+1	;load current value (low part)
//	inc	w		;increment it
//	sts	m_IP_SN+1,w	;and store it back
//
//	std	Z+f_IP_ID+1,w	;store as start value in frame
//	
//	lds	w,m_IP_SN+0	;handles byte overflow
//	brne	IP_TX_create_frame_sn1
//	inc	w
//IP_TX_create_frame_sn1:
//	std	Z+f_IP_ID+0,w
//	sts	m_IP_SN+0,w
//
//	//************* clear fragmnent bit **********************
//	;we dont use fragments
//	stdiZ	f_IP_Flags,_IP_dont_fragment,w
//	stdiZ	f_IP_frameoffset,0,w
//
//	//*********** clear fragment offset **********************
//
//	//*********** set TTL startvalue *************************
//	stdiZ	f_IP_TTL,_IP_TTL_startvalue,w	;set Time To Live
//	
//	rcall	ARP_set_dest_MAC_from_IP
//
//	ldiZ	m_frameheader_TX+_DLC_headersize
//
//	ret

//****************************************************************
//*																*
//****************************************************************

//IP_TX_send_frame:
//	
//	clr	a
//	lds	b,m_IP_TX_headersize	;add IP-header len
//	add	r_TX_frameCountL,b
//	adc	r_TX_frameCountH,a
//
//	ldiZ	f_IP_header+m_frameheader_TX	;set Source-IP
//
//	std	Z+f_IP_framesize+0,r_TX_frameCountH ;set tot. Size
//	std	Z+f_IP_framesize+1,r_TX_frameCountL
//	
//	mov	r_IP_startH,ZH
//	mov	r_IP_startL,ZL
//	adiw	ZL,f_IP_sourceIP
//	rcall	IP_set_my_IP_at_Zi
//		
//	clr	a
//	ldiY	f_IP_header_CRC+f_IP_header+m_frameheader_TX	;delete old CRC
//	moveZ	YH,YL
//
//	std	Y+0,a
//	std	Y+1,a			
//	sbiw	YL,f_IP_header_CRC
//
//	//******** IP Header length calculation *************
//	ldd	w,Y+f_IP_version	
//	andi	w,_IP_headsizemask	;store IP-header size
//	lsl	w			;calculate the byte sum
//	lsl	w			;of IP-Header (*4)
//
//	rcall	calc_crc32			;calc IP CRC32
//	
//	st	Z+,c				;store CRC
//	st	Z+,b				;store CRC
//
//	rjmp	DLC_TX_send_frame

	
	
//****************************************************************
//*																*
//*  cmp_Z_locIP (Z) 											*
//*																*
//****************************************************************
//*  compares the 4 bytes at Z and compares them with local IP	*
//*  If IP is the same, ZF will be set.							*
//*--------------------------------------------------------------*
//*  Parameters	| Meaning			¦ used Bits					*
//*--------------------------------------------------------------*
//*  Register    = Z				   8							*
//*																*
//*  returns Zeroflag = 1 if same IP, otherwise ZF =0			*
//*  Z = Z + 4													*
//*																*
//* Class B routine												*
//****************************************************************

//cmp_Z_locIP:	ldsq	a,b,c,d,m_localIPNR
//cmp_Z_IP:
//		sez
//		ldd	w,Z+0
//		cp	w,a
//		brne	cmp_Z_notlocIP
//		ldd	w,Z+1
//		cp	w,b
//		brne	cmp_Z_notlocIP
//		ldd	w,Z+2
//		cp	w,c
//		brne	cmp_Z_notlocIP
//		ldd	w,Z+3
//		cp	w,d
//		breq	cmp_Z_locIPOK
//cmp_Z_notlocIP:
//		clz
//cmp_Z_locIPOK:
//		ret
//****************************************************************
//*								*
//****************************************************************
	
//cmp_Y_locIP:	ldsq	a,b,c,d,m_localIPNR
//cmp_Y_IP:
//		sez
//		ldd	w,Y+0
//		cp	w,a
//		brne	cmp_Z_notlocIP
//		ldd	w,Y+1
//		cp	w,b
//		brne	cmp_Z_notlocIP
//		ldd	w,Y+2
//		cp	w,c
//		brne	cmp_Z_notlocIP
//		ldd	w,Y+3
//		cp	w,d
//		brne	cmp_Z_notlocIP
//		ret	
//****************************************************************
//*								*
//****************************************************************
//;class	AA Routine
//
//IP_set_my_IP_at_Zi:
//		lds	w,m_localIPNR+0
//		st	Z+,w
//		lds	w,m_localIPNR+1
//		st	Z+,w
//		lds	w,m_localIPNR+2
//		st	Z+,w
//		lds	w,m_localIPNR+3
//		st	Z+,w
//		ret
//
//
