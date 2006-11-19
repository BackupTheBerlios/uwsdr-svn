
#include "codec.h"
#include "uip.h"

u8    codec_buf[2*CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE]; // ~6kb = 2000 24bit stereo samples
u16   codec_send_counter0;
u16   codec_send_counter1;
u32   codec_status_flag;

//********************************************************
//
// CODEC_start_output
//
// Parameters: 
// Returns: void
//
//********************************************************
void CODEC_start_output()
{
}


//********************************************************
//
// CODEC_init
//
// Parameters: void
// Returns: void
//
//********************************************************
void CODEC_init(void)
{
  int i;
  /****** AD1836A Init *******/
     
  i = CODEC_REG_ADC_CTRL3|CODEC_ADC_CLOCKMODE_X512; //0xE040
  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // ADC CTRL 3
    
  i = CODEC_REG_DAC_CTRL1|CODEC_DAC_SERMODE_LEFTJUSTIFIED;
  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 1
  // set volume to max
  i =   CODEC_REG_DAC_VOL_1L|1023;
  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 1
  i =   CODEC_REG_DAC_VOL_1R|1023;
  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 1

  i = CODEC_REG_DAC_CTRL2;
  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 2
  
  
  AT91F_SPI_PutChar(AT91C_BASE_SPI, 0xC000, 0); // ADC CTRL 1
//    AT91F_SPI_PutChar(AT91C_BASE_SPI, 0xD200, 0); // ADC CTRL 2
  
  i = CODEC_REG_ADC_CTRL2|CODEC_ADC_MASTER|CODEC_ADC_SOUT_MODE_LEFTJUSTIFIED;
  //0xD2C0
  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // ADC CTRL 2

  i = CODEC_REG_ADC_CTRL3|CODEC_ADC_CLOCKMODE_X512; //0xE040
  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // ADC CTRL 3
  
}

//********************************************************
//
// CODEC_start
//
// Parameters: void
// Returns: void
//
//********************************************************
void CODEC_start(void)
{
  codec_status_flag = 0;
  CODEC_SET_MODE(CODEC_MODE_RX);
  
  //* Enable receiver / transmitter
  AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXEN|AT91C_SSC_TXEN;
  
  //wait for first frame (rising edge)
  while(AT91C_BASE_SSC->SSC_SR & AT91C_SSC_TXSYN) {};
  
  // then set to sync to any edge
  AT91C_BASE_SSC->SSC_RCMR =  AT91C_SSC_CKS_RK| //clk from RK pin 
                              AT91C_SSC_CKO_NONE| //no output
                              AT91C_SSC_CKI|
                              AT91C_SSC_START_EDGE_RF;
  
  //CODEC_SSC_ISR(); //start rx/tx streaming
  //while(CODEC_GET_ACTIVE_BUFFER() != 1) {}; //wait for first buffer full
  
}


void CODEC_SSC_ISR() //__irq 
{
  u8* pCurrent;
  u8* pNext;
  u32 test32;

  //AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXDIS | AT91C_SSC_TXDIS ;

  if(CODEC_GET_ACTIVE_BUFFER() == 1) {
    // now use Buffer 0
    pCurrent = codec_buf + CODEC_HEADER_SIZE;
    pNext = codec_buf + CODEC_BUFFERSIZE;
    CODEC_SET_ACTIVE_BUFFER(0);
    codec_send_counter1 = 0;
  }
  else {
    pCurrent = codec_buf + (CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE);
    pNext = codec_buf;
    CODEC_SET_ACTIVE_BUFFER(1);
    codec_send_counter1 = 0;
  }

  // register conf for RX
  if(CODEC_IS_MODE(CODEC_MODE_RX)) {
    AT91C_BASE_SSC->SSC_RPR = (u32)pCurrent;
    AT91C_BASE_SSC->SSC_RCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_RNPR = (u32)pNext;
    AT91C_BASE_SSC->SSC_RNCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_PTCR = (1 << 0); // RXTEN
  }
  
  // register conf for TX
  if(CODEC_IS_MODE(CODEC_MODE_TX)) {
    AT91C_BASE_SSC->SSC_TPR = (u32)pCurrent;
    AT91C_BASE_SSC->SSC_TCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_TNPR = (u32)pNext;
    AT91C_BASE_SSC->SSC_TNCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_PTCR = (1 << 8); // TXTEN
  }
  //* Enable receiver / transmitter
  //AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXEN|AT91C_SSC_TXEN;
}

void UDP_process(void) {
  
  if(CODEC_GET_ACTIVE_BUFFER() == 1) {
    if(codec_send_counter0 == CODEC_BUFFERSIZE/2) // the buffer is sent
      return;
    uip_appdata = codec_buf + codec_send_counter0;
    codec_send_counter0 += CODEC_UDP_FRAMESIZE;
    //uip_slen = 
  }
}

// end of main
