
//****************************************************************
//****************************************************************
//****************************************************************
//*					
//* Address Resolution Protocol		
//*					
//* Version 1.00			
//*							
//* all source code by Tobias Weber (C) 2001,2002	
//****************************************************************
//****************************************************************
//****************************************************************

#include "config.h"
#include "arp.h"
#include "net.h"
#include "ip.h"
#include <string.h>

//.dseg
//	m_ARP_cache:				.byte _ARP_cache_size
//.cseg

//*** ARP cache *** 

typedef struct {
  u8 MAC[_DLC_ADDR_SIZE];
  u32 IP;
} t_ARPentry;

t_ARPentry ARPcache[ _ARP_CACHE_ENTRIES ];

//****************************************************************************
// ARP_init
//
// Parameters: void
// Return type: void
// Initialises the ARP stack
//
//****************************************************************************
void ARP_init(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  memset(ARPcache, 0, sizeof(ARPcache));
}


//;****************************************************************
//;*
//;*	ARP_send_request(a,b,c,d) Sends Request out to Broadcast
//;*
//;*--------------------------------------------------------------*
//;*
//;* Parameters: Registers a,b,c,d
//;*
//;* IP Adress to request MAC from.
//;* 
//;****************************************************************
//

//ARP_TX_send_request:
//
//	rcall	DLC_TX_create_broadcast
//	ldiZ	m_frameheader_TX	;goto start of Frame
//	stdiwZ	f_DLC_ethertype,_DLC_type_ARP,w
//	ldiZ	m_frameheader_TX+_DLC_headersize	;goto start of ARP-Frame
//
//	stdiwZ	ARP_HWtypeL,ARP_opcode_HWType,w
//	stdiwZ	ARP_prottypeL,ARP_opcode_IP,w
//	stdiZ	ARP_HWadrlen,_DLC_address_length,w
//	stdiZ	ARP_Protadrlen,_IP_address_length,w
//	stdiwZ	ARP_opcodeL,ARP_opcode_Request,w
//	ldiY	m_localMAC
//	cpy32zy	ARP_SenderHWadr,0,w,w,w,w
//	cpy16zy	ARP_SenderHWadr+4,4,w,w
//	stdqZ	ARP_TargetIPadr,a,b,c,d
//	ldiY	m_localIPNR
//	cpy32zy	ARP_SenderIPadr,0,w,w,w,w
//
//	ldiW	a,b,ARP_packetsize-_DLC_headersize
//	mov		r_TX_frameCountH,a
//	mov		r_TX_frameCountL,b
//	
//	rjmp	DLC_TX_send_frame
//
//
//;****************************************************************
//;*																*
//;*	ARP_TX_reply()												*
//;*																*
//;*--------------------------------------------------------------*
//;*																*
//;*	receives ARP-Frame if IP is in ARP-Cache					*
//;*																*
//;* Class AAYZ routine											*
//;****************************************************************
//
//ARP_RX_process_reply:
//
//	ldi		w,_ARP_cache_entries
//	ldiY	_ARP_RX_framestart+ARP_SenderIPadr
//	ldiZ	m_ARP_cache
//
//ARP_RX_reply_cknx:	;search corresponding ARP-Entry
//	rcall	cmp32eq ;compare IP
//	breq	ARP_RX_reply_fnd_en ;found corresponding entry
//	
//	;jump to next APR-entry
//	adiw	ZL,_DLC_address_length+_IP_address_length
//	dec		w
//	brne	ARP_RX_reply_cknx
//	ret
//	
//ARP_RX_reply_fnd_en:	;copy MAC-Adr from Frame to Cache
//	sbiw	YL,_DLC_address_length
//	adiw	ZL,_IP_address_length
//
//	rjmp	move48ZY
//
//
//;****************************************************************
//;*								*
//;* ARP_set_MAC_from_IP_at_Z					*
//;*								*
//;*--------------------------------------------------------------*
//;*								*
//;* Searches IP (a,b,c,d) in cache and writes it at Z		*
//;*								*
//;* Class AAE Routine						*
//;****************************************************************
//
//
//ARP_set_dest_MAC_from_IP:
//	push	YH
//	push	YL
//	
//	ldiY	m_ARP_cache
//	ldi	e,_ARP_cache_entries
//
//ARP_ck_next_cache:
//	rcall	cmp_Y_IP
//	breq	ARP_MAC_found
//	adiw	YL,_ARP_cache_entries
//	dec	e
//	brne	ARP_ck_next_cache
//
//ARP_MAC_not_found:
//	ldi	w,_ARP_ERROR_NO_MAC
//	rjmp	ARP_set_MAC_end
//ARP_MAC_found:
//	adiw	YL,s_ARP_cache_MAC
//	ldiZ	f_DLC_dst_adr+m_frameheader_TX
//	rcall	move48ZY
//
//ARP_set_MAC_end:
//	pop	YL
//	pop	YH
//ARP_process_end:
//	ret
//
//;****************************************************************
//;*
//;*	ARP_TX_reply()
//;*
//;*--------------------------------------------------------------*
//;*
//;*	forms a ARP-REPLY
//;*
//;****************************************************************
//
//ARP_TX_reply:
//	
//	;************ Its my packet *********************
//
//	;set opcode to Reply type of message
//
//
//	ldiY	m_frameheader_RX+_DLC_headersize
//	ldiZ	m_frameheader_RX+_DLC_headersize
//	
//	stdiY	ARP_opcodeH,low(ARP_opcode_Reply),w
//
//	;exchange Adresses
//	adiw	YL,ARP_SenderHWadr
//	adiw	ZL,ARP_TargetHWadr
//	rcall	move48ZY
//
//	ldiZ	_ARP_RX_framestart+ARP_SenderHWadr
//	ldiY	m_localMAC
//	rcall	move48ZY
//
//	ldiY	_ARP_RX_framestart+ARP_SenderIPadr
//	ldiZ	_ARP_RX_framestart+ARP_TargetIPadr
//	rcall	move32ZY
//	
//	ldiZ	_ARP_RX_framestart+ARP_SenderIPadr
//	rcall	IP_set_my_IP_at_Zi
//
//	ldiw	a,b,ARP_packetsize
//	mov16rr r_TX_frameCountH,r_TX_frameCountL,a,b
//	
//	;inform lower layer, that packet will be returned
//	rjmp	DLC_TX_return_frame



//****************************************************************************
// ARP_process_request
//
// Parameters: u8* pData
// Return type: s16
// process a incomming request
//
//****************************************************************************
static int ARP_process_request(u8* pData)
{
  //*** DEFINITON ***
  u32 ip_addr, i;
  int res;
  u8* pDest8; 
  
  //*** INITIALIZATION ***
  pDest8 = (u8*)m_pTxBuf;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  //check if it is for my IP

  if(pData[ARP_TARGET_IP + 0] != _IP_ADDR_0 ||
     pData[ARP_TARGET_IP + 1] != _IP_ADDR_1 ||
     pData[ARP_TARGET_IP + 2] != _IP_ADDR_2 ||
     pData[ARP_TARGET_IP + 3] != _IP_ADDR_3 )
  {
    return _NET_WRONG_DESTINATION;
  }
  // it's for me

//  ip_addr = (pData[ARP_SENDER_IP + 0] << 24) +
//            (pData[ARP_SENDER_IP + 1] << 16) +
//            (pData[ARP_SENDER_IP + 2] << 8) +
//            pData[ARP_SENDER_IP + 3];

  for(i = 0; i < _ARP_CACHE_ENTRIES; i++) {
    if(memcmp(&ARPcache[i].IP, IP_getLocalIP(), _IP_ADDR_SIZE) == 0) {
      // save his "new" mac address
      memcpy(ARPcache[i].MAC, &pData[ARP_SENDER_MAC], _DLC_ADDR_SIZE);
      break;
    }
  }
  // if we didnt find it, check for empty places
  if(i == _ARP_CACHE_ENTRIES) {
    for(i = 0; i < _ARP_CACHE_ENTRIES; i++) {
      if(ARPcache[i].IP == 0) {
        memcpy(&ARPcache[i].IP, &pData[ARP_SENDER_IP], _IP_ADDR_SIZE);
        memcpy(ARPcache[i].MAC, &pData[ARP_SENDER_MAC], _DLC_ADDR_SIZE);
        break;
      }
    }  
  }
  // we have to send him an answer
  //ARP_createReturnFrame(pDest, pData);

  // MAC is ok, now the data
  pDest8 += _DLC_HDR_SIZE;
  
  memcpy(pDest8, pData, ARP_OPCODE_L); //copy everthing from source untill opcode
  // the reply opcode
  pDest8[ARP_OPCODE_L] = low(ARP_OPCODE_REPLY);
  pDest8[ARP_OPCODE_H] = high(ARP_OPCODE_REPLY);
  // my MAC adress
  pDest8[ARP_SENDER_MAC + 0] = _MAC_ADDR_0;
  pDest8[ARP_SENDER_MAC + 1] = _MAC_ADDR_1;
  pDest8[ARP_SENDER_MAC + 2] = _MAC_ADDR_2;
  pDest8[ARP_SENDER_MAC + 3] = _MAC_ADDR_3;
  pDest8[ARP_SENDER_MAC + 4] = _MAC_ADDR_4;
  pDest8[ARP_SENDER_MAC + 5] = _MAC_ADDR_5;
  // my IP
  memcpy(pDest8 + ARP_SENDER_IP, pData + ARP_TARGET_IP, _IP_ADDR_SIZE);
  // his IP
  memcpy(pDest8 + ARP_TARGET_IP, pData + ARP_SENDER_IP, _IP_ADDR_SIZE);
  // his MAC
  memcpy(pDest8 + ARP_TARGET_MAC, pData + ARP_SENDER_MAC, _DLC_ADDR_SIZE);
  // set TX length
  m_TxLen = _ARP_FRAMESIZE;
    
  return _NET_TX_REPLY;
}


//****************************************************************************
// ARP_dispatch
//
// Parameters: u16* pInFrame
// Return type: s16
// Processes incomming ARP requests
//
//****************************************************************************
s16 ARP_dispatch(u8* pInFrame)
{
  //*** DEFINITON ***
  u16 us; //unsiged short (16 bit)
  u32 ul;
  u8* pRX8;
  
  //*** INITIALIZATION ***
  
  pRX8 = (u8*)pInFrame;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  ul = pRX8[ARP_PROTO_TYPE_L] | pRX8[ARP_PROTO_TYPE_H]<<8;

  if(ul != ARP_PROTO_TYPE_ETH)
    return _NET_UNSUPP_PROTOCOL;

  ul = pRX8[ARP_OPCODE_L] | (pRX8[ARP_OPCODE_H]<<8);
 
  switch(ul) {
   case ARP_OPCODE_REQUEST:
     return ARP_process_request(pInFrame);
   default:
     return _NET_ARP_ERROR;
  }
}


//****************************************************************************
// ARP_createReturnFrame
//
// Parameters: u8* pRXFrame, u8* pTXFrame
// Return type: void
// 
//
//****************************************************************************
void ARP_createReturnFrame(u8* pTXFrame, u8* pRXFrame)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //t_DLC* pRX = (t_DLC*)(pRXFrame - _DLC_HDR_SIZE);
  t_DLC* pRX = (t_DLC*)(pRXFrame);
  t_DLC* pTX = (t_DLC*)pTXFrame;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  m_TxLen += sizeof(t_DLC);

  memcpy(pTX->dstMAC, pRX->srcMAC, _DLC_ADDR_SIZE); // copy source to dest
  // set source (my MAC)
  pTX->srcMAC[0] = _MAC_ADDR_0;
  pTX->srcMAC[1] = _MAC_ADDR_1;
  pTX->srcMAC[2] = _MAC_ADDR_2;
  pTX->srcMAC[3] = _MAC_ADDR_3;
  pTX->srcMAC[4] = _MAC_ADDR_4;
  pTX->srcMAC[5] = _MAC_ADDR_5;
  pTX->frameType = pRX->frameType;

}


//****************************************************************************
// ARP_createMACforIP
//
// Parameters: u8* frame, u32 IP
// Return type: s16
// Creates a ethernet frame for a specified IP
//
//****************************************************************************
s16 ARP_createFrameforIP(u8* frame, u32 IP)
{
  //*** DEFINITON ***
  int i;
  u8* pDLC;
  
  //*** INITIALIZATION ***
  pDLC = frame;
  
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

  // do we have an entry for this IP ?
  for(i = 0; i < _ARP_CACHE_ENTRIES; i++) {
    if(ARPcache[i].IP == IP) {
      memcpy(pDLC + _DLC_DST_ADDR, ARPcache[i].MAC, _DLC_ADDR_SIZE);
      break;
    }
  }
  // if not, we need to get one via ARP request, prepare frame
  // TBD
  if(i == _ARP_CACHE_ENTRIES)
    return _NET_ARP_ERROR_TRY_AGN;
  
  //*** set my MAC as the sender
  pDLC[_DLC_SRC_ADDR + 0] = _MAC_ADDR_0;
  pDLC[_DLC_SRC_ADDR + 1] = _MAC_ADDR_1;
  pDLC[_DLC_SRC_ADDR + 2] = _MAC_ADDR_2;
  pDLC[_DLC_SRC_ADDR + 3] = _MAC_ADDR_3;
  pDLC[_DLC_SRC_ADDR + 4] = _MAC_ADDR_4;
  pDLC[_DLC_SRC_ADDR + 5] = _MAC_ADDR_5;

  // frame type IP
  pDLC[_DLC_ETHERTYPE + 0] = 0x08;
  pDLC[_DLC_ETHERTYPE + 1] = 0x00;

  return _NET_OK;


}



//****************************************************************************
// ARP_getMAC
//
// Parameters: u8 IP[]
// Return type: u8*
// Gets a MAC for a IP. If return value is NULL, the IP is currently unknown
// but a request has been started. Try again in a few milliceonds.
//
//****************************************************************************
u8* ARP_getMAC(u32 IP)
{
  //*** DEFINITON ***
  int i;
  
  u8* pDLC; 
  u8* pARP;
  
  //*** INITIALIZATION ***
  pDLC = (u8*)m_pTxBuf;
  pARP = (u8*)m_pTxBuf + _DLC_HDR_SIZE;

  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  // do we have an entry for this IP ?
  for(i = 0; i < _ARP_CACHE_ENTRIES; i++) {
    if(memcmp(&ARPcache[i].IP, IP_getLocalIP(), _IP_ADDR_SIZE) == 0) {
      return ARPcache[i].MAC;
    }
  }
  // if not, we need to get one via ARP request, prepare frame
 
  // destination
  memset(&pDLC[_DLC_DST_ADDR], 0xFF, _DLC_ADDR_SIZE);
  // source
  pDLC[_DLC_DST_ADDR + 0] = _MAC_ADDR_0;
  pDLC[_DLC_DST_ADDR + 1] = _MAC_ADDR_1;
  pDLC[_DLC_DST_ADDR + 2] = _MAC_ADDR_2;
  pDLC[_DLC_DST_ADDR + 3] = _MAC_ADDR_3;
  pDLC[_DLC_DST_ADDR + 4] = _MAC_ADDR_4;
  pDLC[_DLC_DST_ADDR + 5] = _MAC_ADDR_5;
  // frame type
  pDLC[_DLC_ETHERTYPE + 0] = 0x08;
  pDLC[_DLC_ETHERTYPE + 1] = 0x06;

  // **** ARP protocol ****
  
  // Protokol Type
  SET_BE16(pARP + ARP_PROTO_TYPE, ARP_PROTO_TYPE_ETH);
  // Hardware Type
  SET_BE16(pARP + ARP_HW_TYPE, ARP_PROTO_TYPE_ETH);
  // HW length (MAC)
  pARP[ARP_HW_ADDR_LEN] = _DLC_ADDR_SIZE;
  // SW length (IP)
  pARP[ARP_SW_ADDR_LEN] = ARP_IP_ADDR_SIZE;
  // ARP command opcode
  SET_BE16(pARP + ARP_HDR_OPCODE, ARP_OPCODE_REQUEST);
  // local MAC
  memcpy(pARP + ARP_SENDER_MAC, pDLC + _DLC_DST_ADDR, _DLC_ADDR_SIZE);
  // local IP
  memcpy(pARP + ARP_SENDER_IP, IP_getLocalIP(), ARP_IP_ADDR_SIZE);
  // dest IP
  SET_BE32(pARP + ARP_TARGET_IP, IP);

  
  return NULL;
 
}





//ARP_process_request:
//
//	ldi	c,2		;load Prot-Type
//	rcall	LAN_RX_16Yic
//	
//	cpi	a,high(ARP_opcode_IP)
//	brne	ARP_process_end
//	cpi	b,low(ARP_opcode_IP)
//	brne	ARP_process_end
//
//	mov	c,r_RX_framecountL
//	lsr	c
//	
//	rcall	LAN_RX_16Yic
//	
//	clr_RX_Framecount
//
//	ldiZ	_ARP_RX_framestart+ARP_TargetIPadr ;test if the ARP-Packet
//	rcall	cmp_Z_locIP		;if for me
//	brne	ARP_process_end	;else exit
//
//	sbiw	ZL,ARP_TargetIPadr
//
//	ldd		a,Z+ARP_opcodeL
//	ldd		b,Z+ARP_opcodeH
//
//	cpi		a,high(ARP_opcode_Request)
//	brne	ARP_process_req_ckreply
//	cpi		b,low(ARP_opcode_Request)
//	brne	ARP_process_req_ckreply
//
//	rjmp	ARP_TX_reply
//
//ARP_process_req_ckreply:
//	cpi	a,high(ARP_opcode_Reply)
//	brne	ARP_process_end
//	cpi	b,low(ARP_opcode_Reply)
//	brne	ARP_process_end
//
//	rjmp	ARP_RX_process_reply
//
//
//
//
//;Class ABCDYZ Routine
//
//;CB_ARP_Timeout:
//;	ldiY	m_ARP_cache+s_ARP_cache_MAC
//;	ldi	d,_ARP_cache_entries
//
//;CB_ARP_Timeout_ck1:	;check if MAC adress is empty
//
//;	ldi	c,_DLC_address_length
//;	rcall	cmp0Yc			
//;	breq	CB_ARP_Timeout_ckip	;if entry emoty check IP of this entry
//;	sbiw	YL,s_ARP_cache_MAC
//;CB_ARP_Timeout_cne:
//;	adiw	YL,2*_IP_address_length+_DLC_address_length
//;	dec	d
//;	brne	CB_ARP_Timeout_ck1	;if no IP is set, just continue with the next entry
//;	rjmp	CB_ARP_Timeout_allok
//
//;CB_ARP_Timeout_ckip:
//;	sbiw	YL,s_ARP_cache_MAC
//;	ldi	c,_IP_address_length
//;	rcall	cmp0Yc
//;	breq	CB_ARP_Timeout_cne
//	
//;	rcall	ld32Yi
//;	rcall	ARP_TX_send_request
//
//	;ldiY	CB_ARP_Timeout
//	;ldiw	a,b,_ARP_Timeout
//	;rcall	to_restart_timer
//;	ret
//
//;CB_ARP_Timeout_allok:
//	;ldiY	CB_ARP_Timeout
//	;rcall	to_delete_timer
//;	ret	
//
