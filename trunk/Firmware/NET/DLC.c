
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

//;because we only have fixed DLC header lentgh, we can
//;assume IP starts always at the same point



//.dseg
//m_frameheader_RX:		.byte 128
//m_frameheader_TX:		.byte 128
//.cseg

//;****************************************************************
//;****************************************************************
//;*																*
//;* DLC_RX_process_frame											*
//;*																*
//;****************************************************************
//;****************************************************************
//;*																*
//;* Processes frame at RX-Location and call corresponding 		*
//;* processor.													*
//;*																*
//;*Class F Routine												*
//;****************************************************************
//
s16 DLC_process_frame(u8* frame)
{
  return NET_OK;
}


//
//	load	a,_DLC_headersize
//	;substract the DLC part from framesize
//	subs	[r_RX_frameCountH:r_RX_frameCountL,a]
//
//	loadsrc	m_frameheader_RX
//	ldi		c,_DLC_headersize/2
//	rcall	LAN_RX_16ic
//
//	cpi		a,high(_DLC_type_IP)
//	breq	DLC_process_IP_frame
//	
//DLC_processed:
//	ret
//
//DLC_process_IP_frame:
//
//	cpi		b,low(_DLC_type_ARP)	
//	brne	DLC_process_test_IP_prot
//	rjmp	ARP_process_request		;call ARP-Processor
//	
//
//DLC_process_test_IP_prot:
//
//	cpi		b,low(_DLC_type_IP)	
//	brne	DLC_processed
//
//	;mov		r_IP_startH,YH		;save start of IP
//	;mov		r_IP_startL,YL
//
//	rjmp	IP_process_request 		;call IP-Processor
//

