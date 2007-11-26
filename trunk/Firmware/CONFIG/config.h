//****************************************************************************
// Projekt:	uWSDR
//
// File:	config.h
//
// Author:	Tobias Weber
//
// Version: 1.00A
//
// Beschreibung: Globales Config-File
// Diese Datei ist für die modularisierung eines Projekts verantwortlich.
// Hier werden die includes sämtlicher C-Module eingebunden. Alle weiteren
// Module sollten nur diese config.h includen.
//
//
//****************************************************************************

#ifndef __CONFIG_H__
#define __CONFIG_H__

//******************************** USE FLAGS *********************************
#define _USE_ADC  0

//******************************* INCLUDES ***********************************
#include "types.h"
//#include "AT91SAM7X-EK.h"
#include "ioat91sam7x256.h"
#include "lib_AT91SAM7XC256.h"
//******************************** DEFINES ***********************************

#define _SW_VERSION_            100

//#define  _BIG_ENDIAN_MODE_
#define  _LITTLE_ENDIAN_MODE_

#define _DELAY_COEFF            5
#define _DELAY_CORRECTION       50/60
#define NOP() __asm("nop");
#define HANG() while(1) {NOP();}

#define RTTC_INTERRUPT_LEVEL	1
#define IRQ0_INTERRUPT_LEVEL	2
#define IRQ1_INTERRUPT_LEVEL	3
#define SSC_INTERRUPT_LEVEL	    4

#define udp_appcall             UDP_process
#define UIP_APPCALL()           // TCP apcall non existent
#define UIP_UDP_APPCALL()       UDP_process()

#define _DBG_PIN0              (1<<31)
#define _DBG_PIN1              (1<<13)

#define _DBG_STATE_MAIN         0
#define _DBG_STATE_CODEC        1
#define _DBG_STATE_UDP          2
#define _DBG_STATE_ETH          3


#define _DBG_STATE_POS(x)       CLR_PIN(PIOA, ~(x<<21)); \
                                SET_PIN(PIOA, (x<<21))

#define pdTRUE		( 1 )
#define pdFALSE		( 0 )

#define pdPASS		( 1 )
#define pdFAIL		( 0 )

#define SET_PIN_OUTPUT(x)   AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, x)
#define SET_PIN_INPUT(x)    AT91F_PIO_CfgInput(AT91C_BASE_PIOA, x)

//extern volatile u16 uip_len, uip_slen;
//extern u8 uip_buf[UIP_BUFSIZE+2];   /* The packet buffer that contains
//				      incoming packets. */

//************************* NETWORK DEFINITIONS ********************************

//#define UIP_BYTE_ORDER  UIP_LITTLE_ENDIAN

#define _IP_ADDR_0          192
#define _IP_ADDR_1          168
#define _IP_ADDR_2          1
#define _IP_ADDR_3          123

//#define _IP_ADDR_0          10
//#define _IP_ADDR_1          0
//#define _IP_ADDR_2          0
//#define _IP_ADDR_3          123
//
#define _MAC_ADDR_0         0x00
#define _MAC_ADDR_1         0x11
#define _MAC_ADDR_2         0x22
#define _MAC_ADDR_3         0x33
#define _MAC_ADDR_4         0x44
#define _MAC_ADDR_5         0x55


#define _UDP_CTRL_PORT      2221
#define _UDP_DATA_PORT      2222

#define _IP_DRIPADDR0   192
#define _IP_DRIPADDR1   168
#define _IP_DRIPADDR2   1
#define _IP_DRIPADDR3   4

#define _IP_MYIPADDR    (u32)(_IP_ADDR_0<<24|_IP_ADDR_1<<16|_IP_ADDR_2<<8|_IP_ADDR_3)

#define UIP_NETMASK0    255
#define UIP_NETMASK1    255
#define UIP_NETMASK2    255
#define UIP_NETMASK3    0

//#define HTONS(src) 
#define NTOHS(src) ( ((u16)src & 0x00FF) << 8 | ((u16)src >> 8) )
#define NTOHL(src)    tbd
#define NTOHL(src)    tbd

#define NET_BUFSIZE     1600

//***** UIP definitions *******

/*-----------------*/
/* LED Definition */
/*-----------------*/
/*                                 PIO   Flash    PA    PB   PIN */
#define LED1            (1<<0)	/* PA0 / PGMEN0 & PWM0 TIOA0  48 */
#define LED2            (1<<1)	/* PA1 / PGMEN1 & PWM1 TIOB0  47 */

#define ETH_IOW_PIN       (1 << 29)
#define ETH_IOR_PIN       (1 << 8)
#define ETH_PSEN_PIN      (1 << 22)
#define ETH_RST_PIN       (1 << 23)
#define ETH_IRQ_PIN       (1 << 30)

/****** CODEC DEFINITIONS ******/

#define CODEC_SPICS_PIN   AT91C_PA11_NPCS0
#define CODEC_RST_PIN     (AT91C_BASE_PIOA, (1 << 16))


/****** I2C DEFINITIONS ******/

#define I2C_SCL_PIN       AT91C_PA11_TWCK
#define I2C_SDA_PIN       AT91C_PA10_TWD

#define ETH_CLR_IOW() \
  AT91C_BASE_PIOA->PIO_CODR = ETH_IOW_PIN
#define ETH_SET_IOW() \
  AT91C_BASE_PIOA->PIO_SODR = ETH_IOW_PIN

#define ETH_CLR_IOR() \
  AT91C_BASE_PIOA->PIO_CODR = ETH_IOR_PIN
#define ETH_SET_IOR() \
  AT91C_BASE_PIOA->PIO_SODR = ETH_IOR_PIN

#define ETH_CLR_PSEN() \
  AT91C_BASE_PIOA->PIO_CODR = ETH_PSEN_PIN
#define ETH_SET_PSEN() \
  AT91C_BASE_PIOA->PIO_SODR = ETH_PSEN_PIN

#define ETH_CLR_RST() \
  AT91C_BASE_PIOA->PIO_CODR = ETH_RST_PIN
#define ETH_SET_RST() \
  AT91C_BASE_PIOA->PIO_SODR = ETH_RST_PIN

//******* PORT PIN ABSTRACTION ********
#define PIOA AT91C_BASE_PIOA
#define PIOB AT91C_BASE_PIOB

#define SET_PIN(port, pinmask) (port->PIO_SODR = pinmask)
#define CLR_PIN(port, pinmask) (port->PIO_CODR = pinmask)
#define GET_PIN(port, pinmask) (port->PIO_PDSR & pinmask)

#define SET_INPUT(port, pinmask)  AT91F_PIO_CfgInput(port, pinmask)
#define SET_OUTPUT(port, pinmask) AT91F_PIO_CfgOutput(port, pinmask)

/******************************************************************************
* general PIN definitions
*******************************************************************************/

#define CODEC_RESET_HIGH() SET_PIN(PIOB, AT91C_PIO_PB23);


//******************* DEBUG PINS **************************

#define DBG_LED1_ON()   SET_PIN(PIOB, AT91C_PIO_PB21)
#define DBG_LED1_OFF()  CLR_PIN(PIOB, AT91C_PIO_PB21)


#define MAX(x,y) ( (x>y)?x:y )

#endif //__CONFIG_H__

