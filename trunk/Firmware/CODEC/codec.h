
#ifndef __CODEC_H__
#define __CODEC_H__

#include "config.h"


 
//************************************************************ 
#define CODEC_REG_DAC_CTRL1                (0x0 << 12)
#define CODEC_DAC_SERMODE_LEFTJUSTIFIED    (0x3 << 5)

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
#define CODEC_REG_ADC_CTRL3                 (0xE << 12)
#define CODEC_ADC_CLOCKMODE_X512            (0x1 << 6)

//************************************************************ 
#define CODEC_REG_ADC_CTRL2                 (0xD << 12)
#define CODEC_ADC_MASTER                    (0x1 << 9)
#define CODEC_ADC_SOUT_MODE_LEFTJUSTIFIED   (0x3 << 6)


typedef struct s_codec_hdr {
  u8    type_char0;
  u8    type_char1;
  u16   seqNr;
  u16   seqLen;
} t_codec_hdr;

#define CODEC_HEADER_SIZE     sizeof(t_codec_hdr)
#define CODEC_BUFFERSIZE      400 * 3 

#define CODEC_SET_CS() SET_PIN(CODEC_SPICS_PIN)
#define CODEC_CLR_CS() CLR_PIN(CODEC_SPICS_PIN)

#define CODEC_MODE_RX             (1<<2)
#define CODEC_MODE_TX             (1<<3)
#define CODEC_BUFFER_FLAG         (1<<4)

//#define CODEC_UDP_FRAMESIZE       1000


extern u32 codec_status_flag;
extern u8 codec_buf[2*CODEC_BUFFERSIZE + 2*CODEC_HEADER_SIZE];
extern u8 *codec_inactivebuf;

#define CODEC_SET_BUFFER_FULL(x)    (codec_status_flag |= (1 << x))
#define CODEC_SET_BUFFER_EMPTY(x)   (codec_status_flag &= ~(1 << x))
#define CODEC_IS_FULL(x)            (codec_status_flag & (1 << x))

#define CODEC_GET_ACTIVE_BUFFER()   (codec_status_flag & CODEC_BUFFER_FLAG)
#define CODEC_SET_ACTIVE_BUFFER(x)  (codec_status_flag &= ~(x*CODEC_BUFFER_FLAG))

#define CODEC_SET_MODE(x)           (codec_status_flag |= x)
#define CODEC_CLEAR_MODE(x)         (codec_status_flag &= ~x)
#define CODEC_IS_MODE(x)            (codec_status_flag & x)

#define CODEC_PERIPH_A \
          AT91C_PA15_TF| \
          AT91C_PA16_TK| \
          AT91C_PA17_TD| \
          AT91C_PA18_RD| \
          AT91C_PA19_RK| \
          AT91C_PA20_RF

void CODEC_SSC_ISR(void);
void CODEC_start_output();
void UDP_process(void);
void CODEC_init(void);
void CODEC_start(void);

#endif //__CODEC_H__

