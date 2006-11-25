//************************************************************************
// Projekt:	uWSDR
//
// File:	main.c
//
// Author:	Tobias Weber
//
// DESC:        main file
//
// Copyright and protected by (c) 2006 by General Public Liscence GNU GPL
//***********************************************************************



//************************* GOBAL CONFIG INCLUDE *****************************
#include "config.h"
#include "ax88796.h"
#include "codec.h"
#include "uip.h"
#include "uip_arp.h"
#include "nic.h"
#include "timer.h"
#include "debug.h"
#include "delay.h"
//**************************** LOCAL DEFINES *********************************


//**************************************************************************
// NAME: main
//
// DESC: The main program
//
// PARAM: void
//
// RETVAL: void
//**************************************************************************

void main(void)
{
  //***** DECLARATION *****
  unsigned int length;
  int i, j;
  u8 uc;
  u32 test32;
  struct timer periodic_timer, arp_timer;

  struct uip_udp_conn *UDP_Socket;

  //***** INITIALZATION *****
  
  __disable_interrupt();
  j = 0;

  // if the software was compiled with DEBUG off, we deny
  // access to internal bus system and JTAG interface.
#ifndef __DEBUG__
#endif

  /****** Enable Peripheral Clock ******/

  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );

  /****** PIN init  ******/
  
#define _OUTPUT_PINS (ETH_IOW_PIN|ETH_IOR_PIN|ETH_PSEN_PIN|ETH_RST_PIN \
                      |ETH_ADDR_MASK|_DBG_LED1) //|CODEC_PERIPH_A) //|CODEC_SPICS_PIN)
  
  AT91F_PIO_CfgDirectDrive(AT91C_BASE_PIOA, _OUTPUT_PINS);
  ETH_CLR_RST();
  ETH_SET_IOW();
  ETH_SET_IOR();
  ETH_SET_PSEN();
    
  AT91F_PIO_Enable(AT91C_BASE_PIOA, _OUTPUT_PINS);
  AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, _OUTPUT_PINS);
  ETH_CLR_PSEN();
  ETH_SET_DATAIN();
  
  //AT91F_PIO_CfgPullup(AT91C_BASE_PIOA, AT91C_PIO_PA14);

  /******* Status LED default setting ******/


  //********* Init ETH **********

  ax88796Init();
 
  
//  LED_RED_ON;
//  LED_GREEN_OFF;
  
  /******** Enable Programmable Clock Output ********/
  
  AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 0, (1 << 21));
  AT91F_PMC_EnablePCK(AT91C_BASE_PMC, 1, 1 + (2 << 2));

  /***** EXT Interrupt *******/

  //* open external IRQ interrupt
  AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, ETH_IRQ_PIN, 0);
  //* start external IRQ0 interrupt
  AT91F_AIC_ConfigureIt (
                         AT91C_BASE_AIC,
                         AT91C_ID_IRQ1,
                         IRQ1_INTERRUPT_LEVEL,
                         AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE,
                         LAN_ISR
                        );
  
  LAN_ENA_IRQ();
  
  /***** SPI ******/
  
  AT91F_SPI_Reset(AT91C_BASE_SPI);
  AT91F_SPI_CfgPMC();
  AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA13_MOSI    ) |
		//((unsigned int) AT91C_PA31_NPCS1   ) |
		((unsigned int) AT91C_PA14_SPCK    ) |
		((unsigned int) AT91C_PA11_NPCS0   ) |
		((unsigned int) AT91C_PA12_MISO    ) //, // Peripheral A
		//((unsigned int) AT91C_PA9_NPCS1   ) |
		//((unsigned int) AT91C_PA22_NPCS3   ) |
		//((unsigned int) AT91C_PA3_NPCS3   ) |
		//((unsigned int) AT91C_PA5_NPCS3   ) |
		//((unsigned int) AT91C_PA10_NPCS2   ) |
		//((unsigned int) AT91C_PA30_NPCS2   )); // Peripheral B
                ,0);
  
  AT91F_SPI_CfgCs(AT91C_BASE_SPI, 0, (20<<8) | AT91C_SPI_CPOL | AT91C_SPI_BITS_16);
  
  AT91F_SPI_CfgMode(AT91C_BASE_SPI, AT91C_SPI_MSTR|AT91C_SPI_PS_FIXED);
  AT91F_SPI_Enable(AT91C_BASE_SPI);
                
  
 
  /****** SSC *******/

  // configure SSC pins as peripheral
  AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, CODEC_PERIPH_A, 0);
  
  //* Disable interrupts
  AT91C_BASE_SSC->SSC_IDR = (unsigned int) -1;
  
  // Enable PMC
  AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, (1 << AT91C_ID_SSC));
  
  //* Reset receiver and transmitter
  AT91C_BASE_SSC->SSC_CR = AT91C_SSC_SWRST | AT91C_SSC_RXDIS | AT91C_SSC_TXDIS ;

  //* Define the Clock Mode Register
  AT91C_BASE_SSC->SSC_CMR = 0; // internal clock is not active

  //* Write the Receive Clock Mode Register
  AT91C_BASE_SSC->SSC_RCMR =  AT91C_SSC_CKS_RK| //clk from RK pin 
                              AT91C_SSC_CKO_NONE| //no output
                              AT91C_SSC_CKI|
                              AT91C_SSC_START_RISE_RF;
                              //(AT91C_SSC_STTDLY & (1<16));

  //* Write the Receive Frame Mode Register
  AT91C_BASE_SSC->SSC_RFMR =  (AT91C_SSC_DATLEN & 7)|
                              AT91C_SSC_MSBF|
                              (AT91C_SSC_DATNB & (2 << 8));

  //* Write the Transmit Clock Mode Register
  AT91C_BASE_SSC->SSC_TCMR =  AT91C_SSC_TCMR_CKS_RXCLK|
                              AT91C_SSC_CKO_NONE|
                              AT91C_SSC_TCMR_RXSTART;
                              //AT91C_SSC_CKI;

  //* Write the Transmit Frame Mode Register
  AT91C_BASE_SSC->SSC_TFMR =  (AT91C_SSC_DATLEN & 7)|
                              AT91C_SSC_MSBF|
                              (AT91C_SSC_DATNB & (2 << 8));
                               //AT91C_SSC_FSEDGE;

  //* Clear Transmit and Receive Counters
  AT91F_PDC_Open((AT91PS_PDC) &(AT91C_BASE_SSC->SSC_RPR));

                     
  /*** enable PDC ***/
        
    
//    // enable SSC_PDC  interrupts
//    AT91F_AIC_ConfigureIt (
//                         AT91C_BASE_AIC,
//                         AT91C_ID_SSC,
//                         SSC_INTERRUPT_LEVEL,
//                         AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,
//                         SSC_ISR
//                        );
//  
//    AT91F_AIC_EnableIt( AT91C_BASE_AIC, AT91C_ID_SSC);
//
//    //enable interrupts in mask
//    AT91C_BASE_SSC->SSC_IER = AT91C_SSC_ENDTX|AT91C_SSC_OVRUN|AT91C_SSC_ENDRX;


  CODEC_init();
  
  //****************** the main loop ********************


  __enable_interrupt();                         // Global interrupt enable

   uip_listen(HTONS(2221));

   uip_ipaddr_t destaddr =  { HTONS((192 << 8) | 168), HTONS((1 << 8) | 4) };

   UDP_Socket = uip_udp_new(&destaddr, HTONS(2222));
   

   //uip_send(codec_buf, CODEC_BUFFERSIZE/2);


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
#define IPBUF ((struct ethip_hdr *)&uip_buf[0])   
#define UWSDR_UDP_FRAMELEN   (1200 + sizeof(t_codec_hdr))

  CODEC_start();
   
  t_codec_hdr*  pHdr;
  
  pHdr = (t_codec_hdr*)&codec_buf;

  u32 framecount;
  
  LAN_DIS_IRQ();
  
  uip_send_udp(UDP_Socket, codec_inactivebuf, UWSDR_UDP_FRAMELEN);
  uip_process(UIP_UDP_SEND_CONN);
  uip_arp_out();
  delay_us(10000);
  nic_send();      
  LAN_ENA_IRQ();
  delay_us(100000);

  framecount = 0;
  i = 0;
  for(;;)
  {
    test32 = AT91C_BASE_SSC->SSC_SR;
    if(test32 & (AT91C_SSC_RXBUFF)) {
      DBG_LED1_ON();
      CODEC_SSC_ISR();
      DBG_LED1_OFF();
      if(codec_inactivebuf) {
        DBG_LED1_OFF();
        LAN_DIS_IRQ();
        
       
        pHdr = (t_codec_hdr*)codec_inactivebuf; // - sizeof(t_codec_hdr);
        
        framecount += 2;
        if(framecount == 0xFFFE)
          framecount = 1;
        if(framecount == 0xFFFF)
          framecount = 0;

        pHdr->seqNr = framecount;
        
        uip_send_udp(UDP_Socket, codec_inactivebuf, UWSDR_UDP_FRAMELEN);
        //uip_send(UDP_Socket, codec_inactivebuf, UWSDR_UDP_FRAMELEN);
        uip_process(UIP_UDP_SEND_CONN);
        uip_arp_out();  
        nic_send();
         codec_inactivebuf = 0;
        //uip_arp_out();
        //nic_send();
        LAN_ENA_IRQ();
        DBG_LED1_ON();
      }
    }
    
    if(i > 10000) {
      //uip_process(UIP_UDP_TIMER);
      i = 0;
    }
    i++;
  }
  

  while(1) {



    
    if(timer_expired(&periodic_timer)) {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++) {
	uip_periodic(i);
	/* If the above function invocation resulted in data that
        should be sent out on the network, the global variable
        uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  nic_send();
	}
      }
      
#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
        should be sent out on the network, the global variable
        uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  nic_send();
	}
      }
#endif /* UIP_UDP */
      
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer)) {
	timer_reset(&arp_timer);
	uip_arp_timer();
      }
    }
     test32 = AT91C_BASE_SSC->SSC_SR;
     if(test32 & (AT91C_SSC_RXBUFF)) {
       DBG_LED1_ON();
       CODEC_SSC_ISR();
       if(CODEC_GET_ACTIVE_BUFFER() == 1) {
          uip_send(codec_buf + (CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE),
                   CODEC_BUFFERSIZE+CODEC_HEADER_SIZE);       
       }
       else {
          uip_send(codec_buf, CODEC_BUFFERSIZE+CODEC_HEADER_SIZE);       
       }
       DBG_LED1_OFF();
     }
  }
}



