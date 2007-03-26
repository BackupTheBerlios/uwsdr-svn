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
#include "lib_AT91SAM7S256.h"
#include "types.h"
//#include "adc.h"
//#include "timeout.h"
//#include "delay.h"
#include "intrinsics.h"
//#include "main.h"
//#include "uip.h"
//#include "uip_arp.h"
//#include "uipopt.h"
//#include "debug.h"
//******************************** DEFINES ***********************************

#define _SW_VERSION_            100

#define _DELAY_COEFF            5
#define _DELAY_CORRECTION       50/60
#define NOP() __asm("nop");

#define RTTC_INTERRUPT_LEVEL	1
#define IRQ0_INTERRUPT_LEVEL	2
#define IRQ1_INTERRUPT_LEVEL	3
#define SSC_INTERRUPT_LEVEL	4

#define udp_appcall             UDP_process
#define UIP_APPCALL()           TCP_process()
#define UIP_UDP_APPCALL()       UDP_process()

#define _DBG_LED1               (1<<31)

//extern volatile u16 uip_len, uip_slen;
//extern u8 uip_buf[UIP_BUFSIZE+2];   /* The packet buffer that contains
//				      incoming packets. */

//************************* NETWORK DEFINITIONS ********************************

//#define UIP_BYTE_ORDER  UIP_LITTLE_ENDIAN

#define UDP_LISTEN_PORT 2222

#define UIP_ETHADDR0    0x00
#define UIP_ETHADDR1    0x01
#define UIP_ETHADDR2    0x02
#define UIP_ETHADDR3    0x03
#define UIP_ETHADDR4    0x04
#define UIP_ETHADDR5    0x05

#define UIP_IPADDR0     192
#define UIP_IPADDR1     168
#define UIP_IPADDR2     1
#define UIP_IPADDR3     123

#define UIP_DRIPADDR0   192
#define UIP_DRIPADDR1   168
#define UIP_DRIPADDR2   1
#define UIP_DRIPADDR3   4

#define UIP_NETMASK0    255
#define UIP_NETMASK1    255
#define UIP_NETMASK2    255
#define UIP_NETMASK3    0

//***** UIP definitions *******

struct uwsdr_state {
  u8 state;
  u16 count;
  char *dataptr;
  char *script;
};
typedef struct uwsdr_state uip_tcp_appstate_t;
typedef struct uwsdr_state uip_udp_appstate_t;


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
#define CODEC_RST_PIN     (1 << 16)


/****** I2C DEFINITIONS ******/

#define I2C_SCL_PIN       AT91C_PIO_PA14
#define I2C_SDA_PIN       AT91C_PIO_PA13

#define LED_RED_ON \
  AT91C_BASE_PIOA->PIO_SODR = LED2;
  //AT91F_PIO_SetOutput(AT91C_BASE_PIOA, LED2)

#define LED_RED_OFF \
  AT91C_BASE_PIOA->PIO_CODR = LED2;
  //AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, LED2)

#define LED_GREEN_ON \
  AT91C_BASE_PIOA->PIO_SODR = LED1;
  //AT91F_PIO_SetOutput(AT91C_BASE_PIOA, LED1)

#define LED_GREEN_OFF \
  AT91C_BASE_PIOA->PIO_CODR = LED1;
  //AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, LED1)

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


#define ETH_ADDR_MASK           0x1F000000
#define ETH_DATA_MASK           0x000000FF

#define ETH_GET_DATA(x) \
  x = AT91C_BASE_PIOA->PIO_PDSR & ETH_DATA_MASK

#define ETH_SET_ADDR(x) \
  AT91C_BASE_PIOA->PIO_SODR = (x<<24) & ETH_ADDR_MASK; \
  AT91C_BASE_PIOA->PIO_CODR = ~(x<<24) & ETH_ADDR_MASK

#define ETH_SET_DATA(x) \
  AT91C_BASE_PIOA->PIO_SODR = x & ETH_DATA_MASK; \
  AT91C_BASE_PIOA->PIO_CODR = ~x & ETH_DATA_MASK

#define ETH_SET_DATAOUT() \
  AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, ETH_DATA_MASK)

#define ETH_SET_DATAIN() \
  AT91F_PIO_OutputDisable(AT91C_BASE_PIOA, ETH_DATA_MASK)

#define SET_PIN(x) AT91C_BASE_PIOA->PIO_SODR = x
#define CLR_PIN(x) AT91C_BASE_PIOA->PIO_CODR = x
#define GET_PIN(x) (AT91C_BASE_PIOA->PIO_PDSR & x)

#define SET_PIN_OUTPUT(x)   AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, x)
#define SET_PIN_INPUT(x)    AT91F_PIO_CfgInput(AT91C_BASE_PIOA, x)

#endif //__CONFIG_H__
