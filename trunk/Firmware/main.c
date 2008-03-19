
#include "intrinsics.h"
#include "config.h"
#include "SAM7_EMAC.h"
#include "net.h"
#include "UDP.h"
#include "codec.h"
#include "I2C.h"
#include "delay.h"
#include "UWSDRcontroller.h"
#include "ring.h"


u32 RX_buf[NET_BUFSIZE/4];
u32 TX_buf[NET_BUFSIZE/4];


__noreturn int main()
{
  s16 result;
  u32 ul;

  __disable_interrupt();

  //*** Hardware Init ***
  
  /* When using the JTAG debugger the hardware is not always initialised to
  the correct default state.  This line just ensures that this does not
  cause all interrupts to be masked at the start. */
  AT91C_BASE_AIC->AIC_EOICR = 0;
  
  /* Enable the peripheral clock. */
  AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
  AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_PIOB ) ;
  AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_EMAC ) ;
    
  // This Pull ups are disabled
  *AT91C_PIOB_PPUDR = (AT91C_PIO_PB5|AT91C_PIO_PB6|AT91C_PIO_PB13|AT91C_PIO_PB14|AT91C_PIO_PB26);

  AT91F_PIO_CfgPeriph(PIOA, 0, 0);
  AT91F_PIO_CfgPeriph(PIOB, 0, 0);
  
  CLR_PIN(PIOB, AT91C_PB18_EF100);
  SET_OUTPUT(PIOA, AT91C_PA22_TK|AT91C_PA21_TF|AT91C_PA0_RXD0);
  SET_OUTPUT(PIOB, AT91C_PIO_PB21);

  // Release Reset of the CODEC


  NET_init(RX_buf, TX_buf);
  EMAC_Init();

  __enable_interrupt();

  u8 buf[10];
  
  buf[0] = 0x03;
  buf[1] = 0x4B;

  I2C_Init(100);
 
  CODEC_init();
  
  RING_init((u8*)&codec_buf, 0, _CODEC_FRAME_SIZE, _CODEC_NUM_OF_BUFS);
  
  // Init the UWSDR sub system
  UWSDR_init();
  
  
  //CODEC_start();

  //*********** THE MAIN LOOP ************
  
  _DBG_STATE_POS(_DBG_STATE_MAIN);

  while(1) {
    result = EMAC_Poll();
    _DBG_STATE_POS(_DBG_STATE_MAIN);
    
    if(result == _NET_GOT_RX_DATA) {
      NET_process();
    }
    _DBG_STATE_POS(_DBG_STATE_MAIN);

    ul = AT91C_BASE_SSC->SSC_SR;
    if((ul & AT91C_SSC_ENDRX) && CODEC_IS_MODE(CODEC_MODE_RX)) {
//      DBG_LED1_ON();
      CODEC_SSC_ISR();
      UWSDR_upload();
//      DBG_LED1_OFF();
    }
    if(ul & AT91C_SSC_ENDRX && CODEC_IS_MODE(CODEC_MODE_TX)) {
      CODEC_SSC_ISR();
      
    }

    _DBG_STATE_POS(_DBG_STATE_MAIN);
  }
}
