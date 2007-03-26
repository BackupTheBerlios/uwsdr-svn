/***************************************************************************** 
*  modified by Louis Beaudoin for uIP-AVR port - November 16, 2003
*  www.embedded-creations.com
*
*  Removed: The debugging calls to the Procyon AVRLIB UART functions and the
*             register dump function was removed
*
*  Modified:
*          The ax88796Write and Read functions/macros were changed to support
*            three methods of communcating with the NIC
*          Interfacing with the AVR ports changed from sbi/cbi/etc functions
*            to direct port names
*****************************************************************************/

//*****************************************************************************
//
// File Name	: 'ax88796.c'
// Title		: ASIX 88796 L Ethernet Interface Driver
// Author		: Pascal Stang
// Created		: 10/22/2002
// Revised		: 10/26/2002
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//	Description : This include file is designed to contain items useful to all
//					code files and projects.
//
// Based in part on code by Louis Beaudoin (www.embedded-creations.com)
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include "config.h"
#include "ax88796.h"
//#include "nic.h"
#include "uip.h"
#include "uip_arp.h"
#include "nic.h"
#include "debug.h"
#include "delay.h"
#include "main.h"
#include "codec.h"

// pointers to locations in the ax88796 receive buffer
static unsigned char nextPage;
static u8 u8_nextPacket;
static unsigned int currentRetreiveAddress;
static u8 CurrRXBuffer;
static u8 RcvNextPacket;

// offsets into ax88796 ethernet packet header
#define  enetpacketstatus     0x00
#define  nextblock_ptr        0x01
#define	 enetpacketLenL		  0x02
#define	 enetpacketLenH		  0x03


#if NIC_CONNECTION == GENERAL_IO
//void ax88796Write(u08 address, u08 data)
//#include "asix_write_asix.c"


//**************************************************************************
// NAME:
//      ax88796WriteBlock
//
// DESC:
//      
//
// PARAM: u8 regaddr, u16 len, u8 *buffer)
//      
//
// RETVAL: void
//      TRUE if successful, FALSE if failed.
//**************************************************************************
void ax88796WriteBlock(u8 regaddr, u32 len, u8 *buffer)
{
  // DECLARATIONS
  u32 i;
  // INITIALISATION
  // PARAMETER CHECK
  // PROGRAM CODE
  ETH_SET_ADDR(regaddr);
  ETH_SET_DATAOUT();
    ETH_CLR_IOW();
  for(i = 0; i < len; i++) {
    ETH_SET_DATA(buffer[i]);
    ETH_SET_PSEN();
    ETH_CLR_PSEN();
  }
    ETH_SET_IOW();
  
  ETH_SET_DATAIN();
}



//******************************************************************
//*	Write to NIC Control Register
//******************************************************************
void ax88796Write(u8 regaddr, u8 regdata)
{
    //--asixaddr &= 0xE0;
    //--asixaddr |= regaddr;
    ETH_SET_ADDR(regaddr);
    //--asixdata = regdata;
    ETH_SET_DATAOUT();
    ETH_SET_DATA(regdata);
    ETH_SET_PSEN();
    //--toasix;
    //ETH_SET_DATAOUT();
    ETH_CLR_IOW();
	//nop();
    ETH_SET_IOW();
    //--fromasix;
    ETH_CLR_PSEN();
    ETH_SET_DATAIN();
}
#endif

#if NIC_CONNECTION == MEMORY_MAPPED_HIGHADDR

#define ax88796Read(AX88796_ADDRESS) (*(volatile unsigned char *) \
                       (MEMORY_MAPPED_AX88796_OFFSET \
                       + (((unsigned char)(AX88796_ADDRESS)) << 8)) )
#endif
                             
#if NIC_CONNECTION == MEMORY_MAPPED
#define ax88796Read(AX88796_ADDRESS) (*(volatile unsigned char *) \
                       (MEMORY_MAPPED_AX88796_OFFSET \
                       + (unsigned char)(AX88796_ADDRESS)) )
#endif

#if NIC_CONNECTION == GENERAL_IO


//**************************************************************************
// NAME:
//      ax88796ReadBlock
//
// DESC:
//      
//
// PARAM: u8 address, u32 length
//      
//
// RETVAL: void
//      TRUE if successful, FALSE if failed.
//**************************************************************************
void ax88796ReadBlock(u8 address, u32 length, u8 *buffer)
{
  // DECLARATIONS
  u32 i;
  // INITIALISATION
  
  // PARAMETER CHECK
  if(buffer == NULL) return;
  if(length == 0) return;
  
  // PROGRAM CODE
//  ETH_SET_DATAOUT();
//  ETH_SET_DATA(0x00);
//  ETH_SET_DATAIN();
  ETH_SET_ADDR(address);
  ETH_SET_PSEN();

  for(i = 0; i < length; i++) {
    ETH_CLR_IOR();
    ETH_GET_DATA(buffer[i]);
    ETH_SET_IOR();
  }
  ETH_CLR_PSEN();
}



u8 ax88796Read(u8 address)
{
  u8 x;
  
  //--fromasix;
//  ETH_SET_DATAOUT();
//  ETH_SET_DATA(0x00);
  ETH_SET_DATAIN();
  //--asixaddr &= 0xE0;
  //--asixaddr |= regaddr;
  ETH_SET_ADDR(address);
  ETH_SET_PSEN();
    ETH_CLR_IOR();
    ETH_GET_DATA(x);
    ETH_SET_IOR();
  ETH_CLR_PSEN();
  
  return x;
}

#endif                       
//*** Fred Eady's init routine
void ax88796Init(void)
{
//	unsigned char delaycount=1;
        u8 tcrFduFlag;
//	ax88796SetupPorts();
	
	// do a hard reset
	//AX88796_RESET_PORT |= _BV(AX88796_RESET_PIN);
        ETH_SET_RST();
        delay_us(10000);
	//AX88796_RESET_PORT &= ~_BV(AX88796_RESET_PIN);
        ETH_CLR_RST();
	// do soft reset
	ax88796Write(ISR, ax88796Read(ISR));
	delay_us(50000);

	// wait for PHY to come out of reset
	ax88796Read(RSTPORT);
	while(tcrFduFlag = ax88796Read(TR) & RST_B);
   
	ax88796WriteMii(0x10,0x00,0x0800);
	//while(delaycount--)
        delay_us(2500000);
	ax88796WriteMii(0x10,0x00,0x1200);

	//ax88796Write(CR,(RD2|STOP));		// stop the NIC, abort DMA, page 0
	ax88796Write(CR,(RD2|STOP));
        delay_us(1000);				// make sure nothing is coming in or going out
	ax88796Write(DCR,DCR_INIT);    
	ax88796Write(RBCR0,0x00);
	ax88796Write(RBCR1,0x00);
	ax88796Write(IMR,0x00);
	ax88796Write(ISR,0xFF);
	ax88796Write(RCR,0x20);                   //Put into monitor mode
        
	ax88796Write(PSTART, RXSTART_INIT);
	ax88796Write(BNRY, RXSTART_INIT); // Boundary to RXSTART
	ax88796Write(PSTOP,RXSTOP_INIT);
	
	// switch to page 1
	ax88796Write(CR,(PS0|RD2|STOP));
	// write mac address
	ax88796Write(PAR0+0, UIP_ETHADDR0);
	ax88796Write(PAR0+1, UIP_ETHADDR1);
	ax88796Write(PAR0+2, UIP_ETHADDR2);
	ax88796Write(PAR0+3, UIP_ETHADDR3);
	ax88796Write(PAR0+4, UIP_ETHADDR4);
	ax88796Write(PAR0+5, UIP_ETHADDR5);
	// set start point
	ax88796Write(CURR,RXSTART_INIT + 1);
        CurrRXBuffer = RXSTART_INIT + 1;

	ax88796Write(CR,(RD2|START));
	ax88796Write(RCR,RCR_INIT);

	if(ax88796Read(GPI) & I_SPD)		// check PHY speed setting
		tcrFduFlag = FDU;				// if 100base, do full duplex
	else
		tcrFduFlag = 0;					// if 10base, do half duplex
		
	ax88796Write(TCR,(tcrFduFlag|TCR_INIT));

	ax88796Write(GPOC,MPSEL);			// select media interface
  
	ax88796Write(TPSR,TXSTART_INIT);

	ax88796Write(CR,(RD2|STOP));
	ax88796Write(DCR,DCR_INIT);
	ax88796Write(CR,(RD2|START));
	ax88796Write(ISR,0xFF);
	ax88796Write(IMR,IMR_INIT);
	ax88796Write(TCR,(tcrFduFlag|TCR_INIT));
        
}


void ax88796BeginPacketSend(unsigned int packetLength)
{
	unsigned int sendPacketLength;
	sendPacketLength = (packetLength>=ETHERNET_MIN_PACKET_LENGTH) ?
	                 packetLength : ETHERNET_MIN_PACKET_LENGTH ;
	
	//start the NIC
	ax88796Write(CR,0x22);
	
	// still transmitting a packet - wait for it to finish
	while( ax88796Read(CR) & 0x04 );

	//load beginning page for transmit buffer
	ax88796Write(TPSR,TXSTART_INIT);
	
	//set start address for remote DMA operation
	ax88796Write(RSAR0,0x00);
	ax88796Write(RSAR1,0x40);
	
	//clear the packet stored interrupt
	ax88796Write(ISR, PTX);

	//load data byte count for remote DMA
	ax88796Write(RBCR0, (unsigned char)(packetLength));
	ax88796Write(RBCR1, (unsigned char)(packetLength>>8));

	ax88796Write(TBCR0, (unsigned char)(sendPacketLength));
	ax88796Write(TBCR1, (unsigned char)((sendPacketLength)>>8));
	
	//do remote write operation
	ax88796Write(CR,0x12);
}

void ax88796SendPacketData(unsigned char * localBuffer, unsigned int length)
{
        ax88796WriteBlock(RDMAPORT, length, localBuffer);
}

void ax88796EndPacketSend(void)
{
	//send the contents of the transmit buffer onto the network
	ax88796Write(CR,0x24);
	
	// clear the remote DMA interrupt
	ax88796Write(ISR, RDC);
}


unsigned int ax88796BeginPacketRetreive(void)
{
  unsigned char writePagePtr;
  unsigned char readPagePtr;
  unsigned char bnryPagePtr;
  unsigned char i;
  unsigned char pageheader[4];
  unsigned int rxlen;
	
  // check for and handle an overflow
  if(ax88796ProcessInterrupt() != -2)
    return 0;
  //only continue if PRX event happened
  // clear the packet received interrupt flag

  // read CURR from page 1
  ax88796Write(CR,(PS0|RD2|STOP));
  writePagePtr = ax88796Read(CURR);
  
  // read the boundary register from page 0
  ax88796Write(CR,(RD2|STOP));
  bnryPagePtr = ax88796Read(BNRY);
  
  // first packet is at page bnryPtr+1
  readPagePtr = bnryPagePtr+1;
  if(readPagePtr >= RXSTOP_INIT)
    readPagePtr = RXSTART_INIT;

  
  // return if there is no packet in the buffer
  if( readPagePtr == writePagePtr ) {
    // clear the packet received interrupt flag
    ax88796Write(ISR, PRX);
    return 0;
  }
  
  
  // if the boundary pointer is invalid,
  // reset the contents of the buffer and exit
  if( (bnryPagePtr < RXSTART_INIT) || (bnryPagePtr >= RXSTOP_INIT) )
  {
    ax88796Write(BNRY, RXSTART_INIT);
    ax88796Write(CR, (PS0|RD2|START));
    ax88796Write(CURR, RXSTART_INIT+1);
    ax88796Write(CR, (RD2|START));
    // clear the packet received interrupt flag
    ax88796Write(ISR, PRX);
    return 0;
  }
  
  // initiate DMA to transfer the RTL8019 packet header
  ax88796Write(RBCR0, 4);
  ax88796Write(RBCR1, 0);
  ax88796Write(RSAR0, 0);
  ax88796Write(RSAR1, CurrRXBuffer);
  ax88796Write(CR, (RD0|START));
  //	for(i = 0; i < 4; i++)
  //		pageheader[i] = ax88796Read(RDMAPORT);
  
  ax88796ReadBlock(RDMAPORT, 4, pageheader);
  
  // end the DMA operation
  ax88796Write(CR, (RD2|START));
  for(i = 0; i <= 200; i++)
    if(ax88796Read(ISR) & RDC)
      break;
  
  ax88796Write(ISR, RDC);
  
  rxlen = (pageheader[enetpacketLenH]<<8) + pageheader[enetpacketLenL];
  nextPage = pageheader[nextblock_ptr] ;
  CurrRXBuffer = nextPage;
    
  currentRetreiveAddress = (readPagePtr<<8) + 4;

  // clear the packet received interrupt flag
  ax88796Write(ISR, PRX);
  
  // if the nextPage pointer is invalid, the packet is not ready yet - exit
  if( (nextPage >= RXSTOP_INIT) || (nextPage < RXSTART_INIT) ) {
    return 0;
  }
  
  return rxlen-4;
}


void ax88796RetreivePacketData(unsigned char * localBuffer, unsigned int length)
{
	unsigned int i;
	
	// initiate DMA to transfer the data
	ax88796Write(RBCR0, (unsigned char)length);
	ax88796Write(RBCR1, (unsigned char)(length >> 8));
	ax88796Write(RSAR0, (unsigned char)currentRetreiveAddress);
	ax88796Write(RSAR1, (unsigned char)(currentRetreiveAddress >> 8));
	ax88796Write(CR, (RD0|START));
        
        ax88796ReadBlock(RDMAPORT, length, localBuffer);

	// end the DMA operation
	ax88796Write(CR, (RD2|START));
	for(i = 0; i <= 20; i++)
		if(ax88796Read(ISR) & RDC)
			break;
	ax88796Write(ISR, RDC);
    
	currentRetreiveAddress += length;
	if( currentRetreiveAddress >= 0x6000 )
    	currentRetreiveAddress -= (0x6000-0x4600) ;
}



void ax88796EndPacketRetreive(void)
{
//  unsigned char i;
  unsigned char bnryPagePtr;
  
  // end the DMA operation
  ax88796Write(CR, (RD2|START));
//  for(i = 0; i <= 1; i++)
//    if(ax88796Read(ISR) & RDC)
//      break;
  ax88796Write(ISR, RDC);

  // set the boundary register to point
  // to the start of the next packet-1
  bnryPagePtr = nextPage-1;
  if(bnryPagePtr < RXSTART_INIT)
    bnryPagePtr = RXSTOP_INIT-1;

  ax88796Write(BNRY, bnryPagePtr);
  // clear the packet received interrupt flag
  //ax88796Write(ISR, PRX);
}


void ax88796Overrun(void)
{
	unsigned char cmdReg, resend;	

	// check if we were transmitting something
	cmdReg = ax88796Read(CR);
	// stop the interface
	ax88796Write(CR, (RD2|STOP));
	// wait for timeout
	delay_us(2000);
	// clear remote byte count registers
	ax88796Write(RBCR0, 0x00);
	ax88796Write(RBCR1, 0x00);
	
	if(!(cmdReg & TXP))
	{
		resend = 0;
	}
	else
	{
		cmdReg = ax88796Read(ISR);
		if((cmdReg & PTX) || (cmdReg & TXE))
	    	resend = 0;
	    else
	    	resend = 1;
	}
	// switch to loopback mode
	ax88796Write(TCR, LB0);
	// start the interface
	ax88796Write(CR, (RD2|START));
	// set boundary
	//ax88796Write(BNRY, RXSTART_INIT);RXSTOP_INIT
        ax88796Write(BNRY, RXSTART_INIT);
	// go to page 1 
	ax88796Write(CR, (PS0|RD2|START));
	// set current page register
	ax88796Write(CURR, RXSTART_INIT+1);
	// go to page 0
	ax88796Write(CR, (RD2|START));
	// clear the overflow int
	ax88796Write(ISR, OVW);
	// switch to normal (non-loopback mode)
	ax88796Write(TCR, TCR_INIT);

	if(resend)
		ax88796Write(CR, (RD2|TXP|START));
	
	//ax88796Write(ISR, 0xFF);
}

#define set_mdc		ax88796Write(MEMR,ax88796Read(MEMR)|0x01);
#define clr_mdc		ax88796Write(MEMR,ax88796Read(MEMR)&0xFE);

#define mii_clk		set_mdc; clr_mdc;				  
					
#define set_mdir	ax88796Write(MEMR,ax88796Read(MEMR)|0x02);
#define clr_mdir	ax88796Write(MEMR,ax88796Read(MEMR)&0xFD);
					
#define set_mdo		ax88796Write(MEMR,ax88796Read(MEMR)|0x08)
#define clr_mdo		ax88796Write(MEMR,ax88796Read(MEMR)&0xF7)

#define mii_write	clr_mdo;  	 			  \
					mii_clk;		  \
					set_mdo;		  \
					mii_clk;      \
					clr_mdo;      \
					mii_clk;      \
					set_mdo;	 \
					mii_clk;	  
					
#define mii_read	clr_mdo;  	 		 \
					mii_clk;	  \
					set_mdo;	  \
					mii_clk;	  \
					set_mdo;  	  \
					mii_clk;	  \
					clr_mdo;	  \
					mii_clk;				  

#define mii_r_ta    mii_clk;				  \
								  
					
#define mii_w_ta    set_mdo;				\
					mii_clk;	\
					clr_mdo;	\
					mii_clk;				  
			
void ax88796WriteMii(unsigned char phyad,unsigned char regad,unsigned int mii_data)
{
	unsigned char mask8;
	unsigned int  i,mask16;

	mii_write;
 
	mask8 = 0x10;
	for(i=0;i<5;++i)
	{
  	   	if(mask8 & phyad)
			set_mdo;
		else
			clr_mdo;
		mii_clk;
		mask8 >>= 1;	 
	}   
	mask8 = 0x10;
	for(i=0;i<5;++i)
	{
  		if(mask8 & regad)
			set_mdo;
		else
			clr_mdo;
		mii_clk;
		mask8 >>= 1;	 
	}    					
	mii_w_ta;
 
	mask16 = 0x8000;
	for(i=0;i<16;++i)
	{
		if(mask16 & mii_data)
			set_mdo;
		else
			clr_mdo;
		mii_clk;	 
		mask16 >>= 1;	 
	}   			
}
 
unsigned int ax88796ReadMii(unsigned char phyad,unsigned char regad)
{
	unsigned char mask8,i;
	unsigned int  mask16,result16;
 
	mii_read;

	mask8 = 0x10;
	for(i=0;i<5;++i)
	{
		if(mask8 & phyad)
			set_mdo;
		else
			clr_mdo;
		mii_clk;	 
		mask8 >>= 1;
	}
	mask8 = 0x10;
	for(i=0;i<5;++i)
	{
		if(mask8 & regad)
			set_mdo;
		else
			clr_mdo;
		mii_clk;
		mask8 >>= 1;
	}
   			
	mii_r_ta;
 
	mask16 = 0x8000;
	result16 = 0x0000;
	for(i=0;i<16;++i)
	{
		mii_clk;
		if(ax88796Read(MEMR) & 0x04)
		{
			result16 |= mask16;
		}
		else
		{
			NOP();
			break;
		}
		mask16 >>= 1;
	}
	return result16;
}

u32 cnt;

int ax88796ProcessInterrupt(void)
{
  unsigned char byte = ax88796Read(ISR);
  int ret;
  
  ret = 0;
  
  if( byte & RXE) {
    DBG_LED1_ON();
    ax88796Write(ISR, RXE); 
    ret = -1;
  }
  if( byte & PRX ) {
    ret = -2;
  }
  if( byte & PTX ) {
    ax88796Write(ISR, PTX); 
  }
  if( byte & OVW) {
    ax88796Overrun();
    ret = -1;
  }
  DBG_LED1_OFF();
  return ret;
}

//**************************************************************************
// NAME:
//      ax88796_process_irq
//
// DESC:
//      
//
// PARAM: void
//      
//
// RETVAL: void
//      TRUE if successful, FALSE if failed.
//**************************************************************************
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

void ax88796_process_irq(void)
{
  // DECLARATIONS
  u32 ul;
  // INITIALISATION
  // PARAMETER CHECK
  // PROGRAM CODE

  uip_len = ax88796BeginPacketRetreive();
  
  if(uip_len > 0 && uip_len < UIP_BUFSIZE) {
   
    ul = sizeof(struct uip_eth_hdr) + sizeof(struct uip_udpip_hdr);
    ul = MIN(ul, uip_len);
    ax88796RetreivePacketData( uip_buf, ul );
    //ax88796EndPacketRetreive();
    
    if(BUF->type == HTONS(UIP_ETHTYPE_IP)) {
      uip_arp_ipin();
      if(uip_input() == UDP_LISTEN_PORT) {
        // it is a packet for uwsdr
        UDP_process_incomming();
        ax88796EndPacketRetreive();
        uip_len = 0;
        uip_flags = 0;
        
      }
      // transmit a packet, if one is ready
      if(uip_len > 0) {
        uip_arp_out();
        nic_send();
      }
    } else if(BUF->type == HTONS(UIP_ETHTYPE_ARP)) {
      if(ul < uip_len)
        ax88796RetreivePacketData( &uip_buf[ul], uip_len - ul );
      uip_arp_arpin();
      ax88796EndPacketRetreive();
      
      if(uip_len > 0) {
        nic_send();
      }
    }
  }
  
//  if(uip_len < UIP_BUFSIZE ) { //sure ?
//    // copy the packet data into the uIP packet buffer
//    ax88796RetreivePacketData( uip_buf, uip_len );
//    ax88796EndPacketRetreive();
//
//    // process an IP packet
//      if(BUF->type == HTONS(UIP_ETHTYPE_IP)) {
//        // add the source to the ARP cache
//        // also correctly set the ethernet packet length before processing
//        uip_arp_ipin();
//        uip_input();
//
//        // transmit a packet, if one is ready
//        if(uip_len > 0)
//        {
//          uip_arp_out();
//          nic_send();
//        }
//      }
//      // process an ARP packet
//      else if(BUF->type == HTONS(UIP_ETHTYPE_ARP))
//      {
//        uip_arp_arpin();
//
//        // transmit a packet, if one is ready
//        if(uip_len > 0)
//          nic_send();
//      }
//  }
}

void LAN_ISR(void) //__irq 
{
  //u32 packetLength;
  DBG_LED1_ON();

  __disable_interrupt(); 
  //EXTINT = EINT2;		/* clear interrupt */
  ax88796_process_irq();
  //VICVectAddr = 0;		/* Acknowledge Interrupt */
  __enable_interrupt();

  DBG_LED1_OFF();
}




