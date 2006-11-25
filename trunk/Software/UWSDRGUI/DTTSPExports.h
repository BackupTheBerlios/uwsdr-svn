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

typedef enum {
	LSB,				//  0
	USB,				//  1
	CWL,				//  2
	CWU,				//  3
	FMN,				//  4
	AM,					//  5
	SAM					//  6
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

extern void    Setup_SDR(float sampleRate, unsigned int audioSize);
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
extern void    SetTXSquelchSt(bool setit);
extern void    SetTXSquelchVal(float setit);
extern void    SetANF(bool setit);
extern void    SetBlkANF(bool setit);
extern void    SetANFvals(unsigned int taps, unsigned int delay, double gain, double leak);
extern void    SetNB(bool setit);
extern void    SetNBvals(float threshold);
extern void    SetSDROM(bool setit);
extern void    SetSDROMvals(float threshold);
extern void    SetBIN(bool setit);
extern void    SetRXAGC(AGCMODE setit);
extern void    SetTXCarrierLevel(float setit);
extern void    SetTXCompressionSt(bool setit);
extern void    SetTXCompressionLevel(float txc);
extern void    SetTXALCAttack(float attack);
extern void    SetTXALCDecay(float decay);
extern void    SetTXALCBot(float max_agc);
extern void    SetTXALCHang(float decay);
extern void    SetTXALCLimit(float limit);
extern void    SetTXLevelerSt(bool state);
extern void    SetTXLevelerAttack(float attack);
extern void    SetTXLevelerDecay(float decay);
extern void    SetTXLevelerTop(float top);
extern void    SetTXLevelerHang(float decay);
extern void    SetCorrectIQ(double phase, double gain);
extern void    SetCorrectTXIQ(double phase, double gain);
extern void    SetPWSmode(SPECTRUMtype type);
extern void    SetWindow(Windowtype Windowset);
extern void    SetSpectrumPolyphase(bool setit);
extern void    SetSquelchVal(float setit);
extern void    SetSquelchState(bool setit);
extern void    SetTRX(TRXMODE setit);
extern void    Process_Spectrum(float* results);
extern void    Process_Panadapter(float* results);
extern void    Process_Phase(float* results, unsigned int numpoints);
extern void    Process_Scope(float* results, unsigned int numpoints);
extern float   Calculate_Meters(METERTYPE mt);
extern void    SetDeviation(float value);

extern void    Audio_Callback(float* input_i, float* input_q, float* output_i, float* output_q, unsigned int nframes);
extern void    Audio_CallbackIL(float* input, float* output, unsigned int nframes);
extern void    process_samples_thread();

#endif
