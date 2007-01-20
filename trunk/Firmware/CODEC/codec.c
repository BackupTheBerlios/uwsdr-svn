
#include "codec.h"
#include "uip.h"
#include "string.h"
#include "delay.h"
#include "i2c.h"

u8    codec_buf[2*CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE]; // ~6kb = 2000 24bit stereo samples
//u16   codec_send_counter0;
//u16   codec_send_counter1;
u32   codec_status_flag;

u8 *  codec_inactivebuf;
volatile u32 max_I;
volatile u32 max_Q;

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

void codec_getpeaks()
{
  register u32 i;
  
  max_I = 0;
  max_Q = 0;
  
  for(i = 0; i < CODEC_BUFFERSIZE; i += 6) {
    if(codec_buf[i] > max_I) 
      max_I = codec_buf[i];
    if(codec_buf[i+3] > max_Q) 
      max_Q = codec_buf[i+3];
  }
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
  u8 uc;
  int i;
  /****** AD1836A Init *******/
  
  CLR_PIN(CODEC_RST_PIN);
  delay_us(10000);
  SET_PIN(CODEC_RST_PIN);
  
  /*** Init of the DAC PCM1740 ***/
//  I2C_send_byte(0x98, 0x00, 0x00);
//  delay_us(100);
//  I2C_send_byte(0x98, 0x01, 0x00);
//  delay_us(100);
//  I2C_send_byte(0x98, 0x02, 0x98);
//  delay_us(100);
//  I2C_send_byte(0x98, 0x03, 0x4B);
//  delay_us(1000);
//  I2C_send_byte(0x98, 0x04, 0x00);
//  delay_us(100);

  /*** Init of the CS4272 ***/
  delay_us(500);
  I2C_send_byte(0x20, 0x07, 0x03); // Mode Control 2
  delay_us(100);

  I2C_send_byte(0x20, 0x01, 0x29); // Mode Control
  delay_us(100);
  I2C_send_byte(0x20, 0x02, 0x00); // DAC Control
  delay_us(100);
  I2C_send_byte(0x20, 0x03, 0x09); // DAC Volume & Mixing control
  delay_us(100);
  I2C_send_byte(0x20, 0x04, 0x00); // Channel A Volume control
  delay_us(100);
  I2C_send_byte(0x20, 0x05, 0x00); // Channel B Volume control
  delay_us(100);
  I2C_send_byte(0x20, 0x06, 0x13); // ADC Control
  delay_us(100);
  I2C_send_byte(0x20, 0x07, 0x02); // Mode Control 2
  delay_us(100);
  
//  //***** First init of Reg 7 *****
//  delay_us(100);
//  // Chip Adress
//  uc = _CODEC_ADDR; 
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Map adress 
//  uc = 0x07;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Data    
//  uc = 0x03;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0);
//  
//  //***** Mode control *****
//  delay_us(100);
//  // Chip Adress
//  uc = _CODEC_ADDR; 
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Map adress 
//  uc = 0x01;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Data    
//  uc = 0x04;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0);
//
//  //***** *****
//  delay_us(100);
//  // Chip Adress
//  uc = _CODEC_ADDR; 
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Map adress 
//  uc = 0x07;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Data    
//  uc = 0x02;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0);

//  //***** *****
//  delay_us(100);
//  // Chip Adress
//  uc = _CODEC_ADDR; 
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Map adress 
//  uc = 0x07;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0); 
//  // Data    
//  uc = 0x03;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, uc, 0);
  
  

  //SET_PIN(AT91C_PA11_NPCS0);

  
 
//******** AD1836A **********
  
//  //i = CODEC_REG_ADC_CTRL3|CODEC_ADC_CLOCKMODE_X256; //0xE040
//  //AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // ADC CTRL 3
//    
//  i = CODEC_REG_DAC_CTRL1|CODEC_DAC_SERMODE_LEFTJUSTIFIED|CODEC_DAC_INTERPOL_96;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 1
//  // set volume to max
//  i =   CODEC_REG_DAC_VOL_1L|1023;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 1
//  i =   CODEC_REG_DAC_VOL_1R|1023;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 1
//
//  i = CODEC_REG_DAC_CTRL2;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // DAC CTRL 2
//  
//  i = CODEC_REG_DAC_CTRL1|CODEC_ADC_SAMPLERATE_96;
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // ADC CTRL 1
//  
//  i = CODEC_REG_ADC_CTRL2|CODEC_ADC_MASTER|CODEC_ADC_SOUT_MODE_LEFTJUSTIFIED|0xf;
//  //0xD2C0
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // ADC CTRL 2
//
//  i = CODEC_REG_ADC_CTRL3|CODEC_ADC_CLOCKMODE_X512; //0xE040
//  AT91F_SPI_PutChar(AT91C_BASE_SPI, i, 0); // ADC CTRL 3
  
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
  t_codec_hdr*  pHdr;
  
  pHdr = (t_codec_hdr*)&codec_buf;
  pHdr->type_char0 = 'D';
  pHdr->type_char1 = 'A';
  pHdr->seqNr = 1;
  pHdr->seqLen = CODEC_BUFFERSIZE;
  
  memcpy(codec_buf + (CODEC_BUFFERSIZE + CODEC_HEADER_SIZE), codec_buf, sizeof(t_codec_hdr));
  
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
  
  //****** setup DMA regsisters ******
  AT91C_BASE_SSC->SSC_RPR = (u32)codec_buf + CODEC_HEADER_SIZE;
  AT91C_BASE_SSC->SSC_RCR = CODEC_BUFFERSIZE;
  
  AT91C_BASE_SSC->SSC_TPR = (u32)codec_buf + CODEC_HEADER_SIZE;
  AT91C_BASE_SSC->SSC_TCR = CODEC_BUFFERSIZE;
  
  //while(CODEC_GET_ACTIVE_BUFFER() != 1) {}; //wait for first buffer full
  
  if(CODEC_IS_MODE(CODEC_MODE_RX)) {
    AT91C_BASE_SSC->SSC_PTCR = (1 << 0); // RXTEN
  }
  
  if(CODEC_IS_MODE(CODEC_MODE_TX)) {
    AT91C_BASE_SSC->SSC_PTCR = (1 << 8); // TXTEN
  }    
  
    
}


void CODEC_SSC_ISR() //__irq 
{
  u8* pCurrent;
  u8* pNext;
  u32 test32;

  //AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXDIS | AT91C_SSC_TXDIS ;

  if(CODEC_GET_ACTIVE_BUFFER() == 1) {
    // now use Buffer 0
    //pCurrent = codec_buf + CODEC_HEADER_SIZE;
    pNext = codec_buf + (CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE);
    //codec_inactivebuf = codec_buf + CODEC_BUFFERSIZE + CODEC_HEADER_SIZE;
    codec_inactivebuf = codec_buf + (CODEC_BUFFERSIZE + CODEC_HEADER_SIZE);;
    CODEC_SET_ACTIVE_BUFFER(0);
    //codec_send_counter1 = 0;
  }
  else {
    //pCurrent = codec_buf + CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE;
    pNext = codec_buf + CODEC_HEADER_SIZE;
//    codec_inactivebuf = codec_buf;
    codec_inactivebuf = codec_buf;
    CODEC_SET_ACTIVE_BUFFER(1);
    //codec_send_counter1 = 0;
  }

  // register conf for RX
  if(CODEC_IS_MODE(CODEC_MODE_RX)) {
    //AT91C_BASE_SSC->SSC_RPR = (u32)pCurrent;
    //AT91C_BASE_SSC->SSC_RCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_RNPR = (u32)pNext;
    AT91C_BASE_SSC->SSC_RNCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_PTCR = (1 << 0); // RXTEN
  }
  
  // register conf for TX
  if(CODEC_IS_MODE(CODEC_MODE_TX)) {
    //AT91C_BASE_SSC->SSC_TPR = (u32)pCurrent;
    //AT91C_BASE_SSC->SSC_TCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_TNPR = (u32)pNext;
    AT91C_BASE_SSC->SSC_TNCR = CODEC_BUFFERSIZE;
    AT91C_BASE_SSC->SSC_PTCR = (1 << 8); // TXTEN
  }
  //* Enable receiver / transmitter
  //AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXEN|AT91C_SSC_TXEN;
}

void UDP_process(void)
{
  if(CODEC_GET_ACTIVE_BUFFER() == 1) {
    //if(codec_send_counter0 == CODEC_BUFFERSIZE/2) // the buffer is sent
      //return;
    //uip_appdata = codec_buf + codec_send_counter0;
    //codec_send_counter0 += CODEC_UDP_FRAMESIZE;
    //uip_slen = 
  }
}

// end of main
