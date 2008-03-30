
#ifndef __CODEC_H__
#define __CODEC_H__

#include "config.h"


//********* CS 4272 ***********

#define _CODEC_ADDR     0x20

//******** AD1836A **********
 
//************************************************************ 
#define CODEC_REG_DAC_CTRL1                (0x0 << 12)
#define CODEC_DAC_SERMODE_LEFTJUSTIFIED    (0x3 << 5)
#define CODEC_DAC_INTERPOL_96               (0x1 << 1)

//************************************************************ 
#define CODEC_REG_DAC_CTRL2                (0x1 << 12)
  
//************************************************************ 
#define CODEC_REG_DAC_VOL_1L               (0x2 << 12)
//************************************************************ 
#define CODEC_REG_DAC_VOL_1R               (0x3 << 12)
//************************************************************ 
#define CODEC_REG_DAC_VOL_2L               (0x4 << 12)
//************************************************************ 
#define CODEC_REG_DAC_VOL_2R               (0x5 << 12)

//************************************************************ 
#define CODEC_REG_ADC_CTRL1                 (0xC << 12)
#define CODEC_ADC_SAMPLERATE_96             (1 << 6)

//************************************************************ 
#define CODEC_REG_ADC_CTRL2                 (0xD << 12)
#define CODEC_ADC_MASTER                    (0x1 << 9)
#define CODEC_ADC_SOUT_MODE_LEFTJUSTIFIED   (0x3 << 6)

//************************************************************ 
#define CODEC_REG_ADC_CTRL3                 (0xE << 12)
#define CODEC_ADC_CLOCKMODE_X512            (0x1 << 6)
#define CODEC_ADC_CLOCKMODE_X256            (0x0 << 6)



typedef struct s_codec_hdr {
  u8    type_char0;
  u8    type_char1;
  u16   seqNr;
  u16   seqLen;
  u8    agc;
  u8    dummy;
} t_codec_hdr;

#define _CODEC_HEADER_SIZE          sizeof(t_codec_hdr)
#define _CODEC_NUM_OF_BUFS          4

#define _CODEC_SAMPLES_PER_BLOCK    480
#define _CODEC_BLOCKS_PER_FRAME     8
#define _CODEC_DATABLOCK_SIZE       (_CODEC_SAMPLES_PER_BLOCK * 3) // 1440bytes
#define _CODEC_FRAME_SIZE           (_CODEC_DATABLOCK_SIZE * _CODEC_BLOCKS_PER_FRAME)
#define _CODEC_DATA_START_ADR       ((u8*)&codec_buf + _CODEC_HEADER_SIZE)



//#define CODEC_BUFFER_SIZE     2048
//#define CODEC_HEADER_SIZE_TX  6
//#define CODEC_HEADER_SIZE_RX  7

#define CODEC_SET_CS() SET_PIN(CODEC_SPICS_PIN)
#define CODEC_CLR_CS() CLR_PIN(CODEC_SPICS_PIN)

#define CODEC_GET_BLOCK_ADR(x)    ((u8*)&codec_buf + _CODEC_DATABLOCK_SIZE * x + _CODEC_HEADER_SIZE)

#define CODEC_MODE_RX             (1<<2)
#define CODEC_MODE_TX             (1<<3)
#define CODEC_BUFFER_FLAG         (1<<4)

//#define CODEC_UDP_FRAMESIZE       1000


extern u32 codec_status_flag;
extern u8 codec_buf[ _CODEC_NUM_OF_BUFS * _CODEC_DATABLOCK_SIZE + _CODEC_HEADER_SIZE ];
extern u8 *codec_inactivebuf;

#define CODEC_SET_BUFFER_FULL(x)    (codec_status_flag |= (1 << x))
#define CODEC_SET_BUFFER_EMPTY(x)   (codec_status_flag &= ~(1 << x))
#define CODEC_IS_FULL(x)            (codec_status_flag & (1 << x))

#define CODEC_GET_ACTIVE_BUFFER()   (codec_status_flag & CODEC_BUFFER_FLAG)
#define CODEC_SET_ACTIVE_BUFFER(x)  (codec_status_flag &= ~(x*CODEC_BUFFER_FLAG))

#define CODEC_SET_MODE(x)           (codec_status_flag |= x)
#define CODEC_CLEAR_MODE(x)         (codec_status_flag &= ~x)
#define CODEC_IS_MODE(x)            (codec_status_flag & x)

//#define CODEC_PERIPH_A \
//          AT91C_PA21_TF| \
//          AT91C_PA22_TK| \
//          AT91C_PA23_TD| \
//          AT91C_PA24_RD

#define CODEC_PERIPH_A \
          AT91C_PA23_TD| \
          AT91C_PA24_RD| \
          AT91C_PA25_RK| \
          AT91C_PA26_RF
          

void CODEC_SSC_ISR(void);
void CODEC_start_output();
void UDP_process_(void);
void CODEC_init(void);
void CODEC_start(void);
void codec_getpeaks(void);
void UDP_process_incoming(void);

//****************************************************************************
// CODEC_startRX
// starts the RX into the buffer
//****************************************************************************
void CODEC_startRX(void);

//****************************************************************************
// CODEC_startTX
// starts the RX into the buffer
//****************************************************************************
void CODEC_startTX(void);

//****************************************************************************
// CODEC_sync
// returns when first channel is being received/transmitted
//****************************************************************************
void CODEC_sync(void);

//****************************************************************************
// CODEC_stop
// Starts Codec RX and TX
//****************************************************************************
void CODEC_stop(void);

#endif //__CODEC_H__

