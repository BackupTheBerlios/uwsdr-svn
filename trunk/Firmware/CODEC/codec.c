
#include "codec.h"
#include <string.h>
#include "delay.h"
#include "i2c.h"
#include "ring.h"

#pragma data_alignment=4
u8    codec_buf[_CODEC_NUM_OF_BUFS][_CODEC_FRAME_SIZE]; // ~6kb = 2000 24bit stereo samples

//u16   codec_send_counter0;
//u16   codec_send_counter1;
//u8    codec_buffer0[CODEC_BUFFER_SIZE+8];
//u8    codec_buffer1[CODEC_BUFFER_SIZE+8];
//u8    codec_buffer2[CODEC_BUFFER_SIZE+8];
//u8    codec_buffer3[CODEC_BUFFER_SIZE+8];

u8    m_codec_pos;
u8    m_codec_tx_pos;

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
//void CODEC_start_output()
//{
//}

//void codec_getpeaks()
//{
//  register u32 i;
//  
//  max_I = 0;
//  max_Q = 0;
//  
//  for(i = 0; i < _CODEC_FRAME_SIZE; i += 6) {
//    if(codec_buf[i] > max_I) 
//      max_I = codec_buf[i];
//    if(codec_buf[i+3] > max_Q) 
//      max_Q = codec_buf[i+3];
//  }
//}


//****************************************************************************
// CODEC_startRX
//
// Parameters: void
// Return type: void
// starts the RX into the buffer
//
//****************************************************************************
void CODEC_startRX(void)
{
  //*** DEFINITON ***
  t_codec_hdr*  pHdr;
  int i;

  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

   //* Disable receiver / transmitter
  AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXDIS|AT91C_SSC_TXDIS;

  RING_reset();
  
  //Prepare outbuffer headers
  pHdr = (t_codec_hdr*)&codec_buf;
  pHdr->type_char0 = 'D';
  pHdr->type_char1 = 'R';
  pHdr->seqNr = 0;
  pHdr->seqLen = _CODEC_SAMPLES_PER_FRAME;
  pHdr->Dummy = 0x1234;
  for(i = 1; i < _CODEC_NUM_OF_BUFS; i++) {
    memcpy(codec_buf[i] , codec_buf, sizeof(t_codec_hdr));
  }
  
  CODEC_SET_MODE(CODEC_MODE_RX);

  AT91C_BASE_SSC->SSC_RPR = (u32)codec_buf + _CODEC_HEADER_SIZE;
  AT91C_BASE_SSC->SSC_RCR = _CODEC_DATA_SIZE;
  AT91C_BASE_SSC->SSC_RNPR = (u32)codec_buf + _CODEC_HEADER_SIZE;
  AT91C_BASE_SSC->SSC_RNCR = _CODEC_DATA_SIZE;
  //AT91C_BASE_SSC->SSC_TNPR = _CODEC_DATA_SIZE;
  

  CODEC_sync();

  AT91C_BASE_SSC->SSC_PTCR = AT91C_PDC_RXTEN; // Start PDC RX
  
}

//****************************************************************************
// CODEC_startTX
//
// Parameters: void
// Return type: void
// starts the TX into the buffer
//
//****************************************************************************
void CODEC_startTX(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  //* Disable receiver / transmitter
  AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXDIS|AT91C_SSC_TXDIS;
 
  RING_reset();

  CODEC_SET_MODE(CODEC_MODE_TX);
 
  // prepare ring TX
  AT91C_BASE_SSC->SSC_TPR = (u32)codec_buf + _CODEC_HEADER_SIZE;
  AT91C_BASE_SSC->SSC_TCR = _CODEC_DATA_SIZE;
  AT91C_BASE_SSC->SSC_TNPR = (u32)codec_buf + _CODEC_HEADER_SIZE;
  AT91C_BASE_SSC->SSC_TNCR = _CODEC_DATA_SIZE;

  CODEC_sync();
  
  AT91C_BASE_SSC->SSC_PTCR = AT91C_PDC_TXTEN; // TXTEN
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
  //u8 uc;
//  int i;


  //m_codec_rx_pos = 0;
  m_codec_tx_pos = 0;
  codec_status_flag = 0;

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
//  AT91C_BASE_SSC->SSC_RCMR =  AT91C_SSC_CKS_TK| //clk from RK pin 
//                              AT91C_SSC_CKO_NONE| //no output
//                              AT91C_SSC_CKI| // lock inverted (rising edge)
//                              AT91C_SSC_START_TX| //start at rising frame
//                              (AT91C_SSC_STTDLY & (1 << 16));
//                              //(AT91C_SSC_STTDLY & (1<16));

  //* Write the Receive Clock Mode Register
  AT91C_BASE_SSC->SSC_RCMR =  AT91C_SSC_CKS_RK| //clk from RK pin 
                              AT91C_SSC_CKO_NONE| //no output
                              AT91C_SSC_CKI| // lock inverted (rising edge)
                              AT91C_SSC_START_FALL_RF| //start at rising frame
                              (AT91C_SSC_STTDLY & (1 << 16));
                              //(AT91C_SSC_STTDLY & (1<16));
    
  //* Write the Receive Frame Mode Register (3x8 bye)
  AT91C_BASE_SSC->SSC_RFMR =  (AT91C_SSC_DATLEN & (7 << 0))|
                              AT91C_SSC_MSBF|
                              (AT91C_SSC_DATNB & (2 << 8));

  //* Write the Transmit Clock Mode Register
  AT91C_BASE_SSC->SSC_TCMR =  AT91C_SSC_TCMR_RKCLK|
                              AT91C_SSC_CKO_NONE|
                              AT91C_SSC_CKI|
                              AT91C_SSC_TCMR_START_RX;
                              ; //(AT91C_SSC_STTDLY & (1 << 16));

  //* Write the Transmit Frame Mode Register (3x8 bye)
  AT91C_BASE_SSC->SSC_TFMR =  (AT91C_SSC_DATLEN & (7 << 0))|
                              AT91C_SSC_MSBF|
                              (AT91C_SSC_DATNB & (2 << 8));
                               //AT91C_SSC_FSEDGE;

  //* Clear Transmit and Receive Counters
  AT91F_PDC_Open((AT91PS_PDC) &(AT91C_BASE_SSC->SSC_RPR));

  
//  CLR_PIN(CODEC_RST_PIN);
  delay_us(10000);
//  SET_PIN(CODEC_RST_PIN);
  
  /*** Init of the DAC PCM1740 ***/
  I2C_send_byte(0x98, 0x00, 0x00);
  delay_us(100);
  I2C_send_byte(0x98, 0x01, 0x00);
  delay_us(100);
  I2C_send_byte(0x98, 0x02, 0x98);
  delay_us(100);
  I2C_send_byte(0x98, 0x03, 0x4B);
  delay_us(1000);
  I2C_send_byte(0x98, 0x04, 0x01);
  delay_us(100);

  /*** Init of the CS4272 ***/
//  delay_us(500);
//  I2C_send_byte(0x20, 0x07, 0x03); // Mode Control 2
//  delay_us(100);
//
//  I2C_send_byte(0x20, 0x01, 0x29); // Mode Control
//  delay_us(100);
//  I2C_send_byte(0x20, 0x02, 0x00); // DAC Control
//  delay_us(100);
//  I2C_send_byte(0x20, 0x03, 0x09); // DAC Volume & Mixing control
//  delay_us(100);
//  I2C_send_byte(0x20, 0x04, 0x00); // Channel A Volume control
//  delay_us(100);
//  I2C_send_byte(0x20, 0x05, 0x00); // Channel B Volume control
//  delay_us(100);
//  I2C_send_byte(0x20, 0x06, 0x13); // ADC Control
//  delay_us(100);
//  I2C_send_byte(0x20, 0x07, 0x02); // Mode Control 2
//  delay_us(100);
  
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

//****************************************************************************
// CODEC_sync
//
// Parameters: void
// Return type: void
// returns when first channel is being received/transmitted
//
//****************************************************************************
__inline void CODEC_sync(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***

  
  //* Write the Transmit Clock Mode Register
//  AT91C_BASE_SSC->SSC_TCMR =  AT91C_SSC_TCMR_RKCLK|
//                              AT91C_SSC_CKO_NONE|
//                              AT91C_SSC_CKI|
//                              AT91C_SSC_TCMR_START_RX;
//                              ; //(AT91C_SSC_STTDLY & (1 << 16));
  //* Write the Receive Clock Mode Register
  AT91C_BASE_SSC->SSC_RCMR =  AT91C_SSC_CKS_RK| //clk from RK pin 
                              AT91C_SSC_CKO_NONE| //no output
                              AT91C_SSC_CKI| // lock inverted (rising edge)
                              AT91C_SSC_START_RISE_RF| //start at rising frame
                              (AT91C_SSC_STTDLY & (1 << 16));

   //* Enable receiver / transmitter
  AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXEN|AT91C_SSC_TXEN;
                              
                              
  //wait for first frame (rising edge)

  while(AT91C_BASE_SSC->SSC_SR & AT91C_SSC_TXSYN) {};
  
  // then set to sync to any edge
  AT91C_BASE_SSC->SSC_RCMR =  AT91C_SSC_CKS_RK| //clk from RK pin 
                              AT91C_SSC_CKO_NONE| //no output
                              AT91C_SSC_CKI|
                              AT91C_SSC_START_EDGE_RF|
                              (AT91C_SSC_STTDLY & (1 << 16));

  //  AT91C_BASE_SSC->SSC_TCMR =  AT91C_SSC_TCMR_TKPIN|
//                              AT91C_SSC_CKO_NONE|
//                              AT91C_SSC_CKI|
//                              AT91C_SSC_START_LEVEL_RF|
//                              (AT91C_SSC_STTDLY & (1 << 16));
}



void CODEC_SSC_ISR() //__irq 
{
//  u8* pCurrent;
  u8* pNext;
//  u32 test32;

  //AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXDIS | AT91C_SSC_TXDIS ;

  _DBG_STATE_POS(_DBG_STATE_CODEC);

  
  if(CODEC_IS_MODE(CODEC_MODE_RX)) {
    pNext = RING_produce();
  }
  else {
    return;
  }

  if(pNext != NULL) {
    pNext += _CODEC_HEADER_SIZE;
    AT91C_BASE_SSC->SSC_RNPR = (u32)pNext;
    AT91C_BASE_SSC->SSC_TNPR = (u32)pNext;
  }
  
  
  AT91C_BASE_SSC->SSC_RNCR = _CODEC_DATA_SIZE;
  AT91C_BASE_SSC->SSC_TNCR = _CODEC_DATA_SIZE;


  
  //AT91C_BASE_SSC->SSC_PTCR = (1 << 8); // TXTEN
    //AT91C_BASE_SSC->SSC_PTCR = (1 << 0); // RXTEN

//  
//    // now use Buffer 0
//    DBG_LED1_ON();
//    //pCurrent = codec_buf + CODEC_HEADER_SIZE;
//    pNext = codec_buf[m_codec_rx_pos] + (_CODEC_FRAME_SIZE + 2*_CODEC_HEADER_SIZE);
//    //codec_inactivebuf = codec_buf + CODEC_BUFFERSIZE + CODEC_HEADER_SIZE;
//    codec_inactivebuf = codec_buf + (_CODEC_FRAME_SIZE + _CODEC_HEADER_SIZE);;
//    //CODEC_SET_ACTIVE_BUFFER(0);
//    //codec_send_counter1 = 0;
//    activeBuffNr = 0;
//  }
//  else {
//    //pCurrent = codec_buf + CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE;
//    pNext = codec_buf + CODEC_HEADER_SIZE;
////    codec_inactivebuf = codec_buf;
//    codec_inactivebuf = codec_buf;
//    //CODEC_SET_ACTIVE_BUFFER(1);
//    //codec_send_counter1 = 0;
//    activeBuffNr = 1;
//  }

  // register conf for RX
  //if(CODEC_IS_MODE(CODEC_MODE_RX)) {
    //AT91C_BASE_SSC->SSC_RPR = (u32)pCurrent;
    //AT91C_BASE_SSC->SSC_RCR = CODEC_BUFFERSIZE;
//    AT91C_BASE_SSC->SSC_RNPR = (u32)pNext;
//    AT91C_BASE_SSC->SSC_RNCR = CODEC_BUFFERSIZE;
//    AT91C_BASE_SSC->SSC_PTCR = (1 << 0); // RXTEN
  //}
  
  // register conf for TX
  //if(CODEC_IS_MODE(CODEC_MODE_TX)) {
    //AT91C_BASE_SSC->SSC_TPR = (u32)pCurrent;
    //AT91C_BASE_SSC->SSC_TCR = CODEC_BUFFERSIZE;
//    AT91C_BASE_SSC->SSC_TNPR = (u32)pNext;
//    AT91C_BASE_SSC->SSC_TNCR = CODEC_BUFFERSIZE;
//    AT91C_BASE_SSC->SSC_PTCR = (1 << 8); // TXTEN
  //}
  //* Enable receiver / transmitter
  //AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXEN|AT91C_SSC_TXEN;
}

void UDP_process_(void)
{
  if(CODEC_GET_ACTIVE_BUFFER() == 1) {
    //if(codec_send_counter0 == CODEC_BUFFERSIZE/2) // the buffer is sent
      //return;
    //uip_appdata = codec_buf + codec_send_counter0;
    //codec_send_counter0 += CODEC_UDP_FRAMESIZE;
    //uip_slen = 
  }
}

void UDP_process_incomming()
{
  //*** DEFINITON ***
  u16 us;
  t_codec_hdr hdr_buf;

  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
  
  if(!CODEC_IS_MODE(CODEC_MODE_TX))
    return;
  
}

//****************************************************************************
// CODEC_stop
//
// Parameters: void
// Return type: void
// Starts Codec RX and TX
//
//****************************************************************************
void CODEC_stop(void)
{
  //*** DEFINITON ***
  //*** INITIALIZATION ***
  //*** PARAMETER CHECK ***
  //*** PROGRAM CODE ***
   //* Disable receiver / transmitter
  AT91C_BASE_SSC->SSC_CR = AT91C_SSC_RXDIS|AT91C_SSC_TXDIS;
}



// end of main
