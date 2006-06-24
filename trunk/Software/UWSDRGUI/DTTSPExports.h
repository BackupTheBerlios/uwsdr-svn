/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef	DttSPExports_H
#define	DttSPExports_H

typedef float REAL;

typedef enum {
	LSB,				//  0
	USB,				//  1
	DSB,				//  2
	CWL,				//  3
	CWU,				//  4
	FMN,				//  5
	AM,				//  6
	DIGU,				//  7
	SPEC,				//  8
	DIGL,				//  9
	SAM,				// 10
	DRM				// 11
} SDRMODE;

typedef enum {
	RX,
	TX
} TRXMODE;

typedef enum {
	agcOFF,
	agcLONG,
	agcSLOW,
	agcMED,
	agcFAST
} AGCMODE;

typedef enum {
	SIGNAL_STRENGTH,
	AVG_SIGNAL_STRENGTH,
	ADC_REAL,
	ADC_IMAG,
	AGC_GAIN,
	MIC,
	PWR,
	ALC,
	EQtap,
	LEVELER,
	COMP,
	CPDR,
	ALC_G,
	LVL_G
} METERTYPE;

typedef enum {
	RECTANGULAR_WINDOW,
	HANN_WINDOW,
	WELCH_WINDOW,
	PARZEN_WINDOW,
	BARTLETT_WINDOW,
	HAMMING_WINDOW,
	BLACKMAN2_WINDOW,
	BLACKMAN3_WINDOW,
	BLACKMAN4_WINDOW,
	EXPONENTIAL_WINDOW,
	RIEMANN_WINDOW,
	BLACKMANHARRIS_WINDOW,
	NUTTALL_WINDOW,
} Windowtype;

typedef enum {
	SPEC_PRE_FILT,
	SPEC_POST_FILT,
	SPEC_POST_AGC,
	SPEC_POST_DET
} SPECTRUMtype;

extern void    Setup_SDR(REAL sampleRate, unsigned int audioSize);
extern void    Destroy_SDR();
extern void    SetMode(SDRMODE m);
extern void    SetDCBlock(bool setit);
extern void    SetFilter(double low_frequency, double high_frequency, int taps, TRXMODE trx);
extern void    Release_Update();
extern void    SetOsc(double newfreq);
extern void    SetRIT(double newfreq);
extern void    SetTXOsc(double newfreq);
extern void    SetNR(bool setit);
extern void    SetBlkNR(bool setit);
extern void    SetNRvals(unsigned int taps, unsigned int delay, double gain, double leak);
extern void    SetTXCompandSt(bool setit);
extern void    SetTXCompand(double setit);
extern void    SetTXSquelchSt(bool setit);
extern void    SetTXSquelchVal(float setit);
extern void    SetANF(bool setit);
extern void    SetBlkANF(bool setit);
extern void    SetANFvals(unsigned int taps, unsigned int delay, double gain, double leak);
extern void    SetNB(bool setit);
extern void    SetNBvals(REAL threshold);
extern void    SetSDROM(bool setit);
extern void    SetSDROMvals(REAL threshold);
extern void    SetBIN(bool setit);
extern void    SetRXAGC(AGCMODE setit);
extern void    SetTXALCAttack(int attack);
extern void    SetTXCarrierLevel(double setit);
extern void    SetTXALCDecay(int decay);
extern void    SetTXALCBot(double max_agc);
extern void    SetTXALCHang(int decay);
extern void    SetTXLevelerSt(bool state);
extern void    SetTXLevelerAttack(int attack);
extern void    SetTXLevelerDecay(int decay);
extern void    SetTXLevelerTop(double top);
extern void    SetTXLevelerHang(int decay);
extern void    SetCorrectIQ(double phase, double gain);
extern void    SetCorrectTXIQ(double phase, double gain);
extern void    SetPWSmode(SPECTRUMtype type);
extern void    SetWindow(Windowtype Windowset);
extern void    SetSpectrumPolyphase(bool setit);
extern void    SetTXEQ(int* txeq);
extern void    SetGrphTXEQ(int* txeq);
extern void    SetGrphTXEQcmd(bool state);
extern void    SetNotch160(bool state);
extern void    SetGrphRXEQ(int* rxeq);
extern void    SetGrphRXEQcmd(bool state);
extern void    SetTXAGCFF(bool setit);
extern void    SetTXAGCFFCompression(REAL txc);
extern void    SetSquelchVal(float setit);
extern void    SetSquelchState(bool setit);
extern void    SetTRX(TRXMODE setit);
extern void    SetTXAGCLimit(double limit);
extern void    Process_Spectrum(float* results);
extern void    Process_Panadapter(float* results);
extern void    Process_Phase(float* results, unsigned int numpoints);
extern void    Process_Scope(float* results, unsigned int numpoints);
extern float   Calculate_Meters(METERTYPE mt);
extern void    SetDeviation(float value);

extern void    Audio_Callback(float* input_i, float* input_q, float* output_i, float* output_q, unsigned int nframes);
extern void    Audio_CallbackIL(float* input, float* output, unsigned int nframes);
extern void    process_samples_thread(void);

#endif
