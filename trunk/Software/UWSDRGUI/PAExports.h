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

#ifndef PAExports_H
#define PAExports_H

#ifdef __cplusplus
extern "C" {
#endif

#include "portaudio.h"

extern int __stdcall PA_GetVersion();
extern const char* __stdcall PA_GetVersionText();
extern const char* __stdcall PA_GetErrorText(PaError errorCode);
extern PaError __stdcall PA_Initialize();
extern PaError __stdcall PA_Terminate();
extern PaHostApiIndex __stdcall PA_GetHostApiCount();
extern PaHostApiIndex __stdcall PA_GetDefaultHostApi();
extern const PaHostApiInfo* __stdcall PA_GetHostApiInfo(PaHostApiIndex hostApi);
extern PaHostApiIndex __stdcall PA_HostApiTypeIdToHostApiIndex(PaHostApiTypeId type);
extern PaDeviceIndex __stdcall PA_HostApiDeviceIndexToDeviceIndex(PaHostApiIndex hostApi, int hostApiDeviceIndex);
extern const PaHostErrorInfo* __stdcall PA_GetLastHostErrorInfo();
extern PaDeviceIndex __stdcall PA_GetDeviceCount();
extern PaDeviceIndex __stdcall PA_GetDefaultInputDevice();
extern PaDeviceIndex __stdcall PA_GetDefaultOutputDevice();
extern const PaDeviceInfo* __stdcall PA_GetDeviceInfo(PaDeviceIndex device);
extern PaError __stdcall PA_IsFormatSupported(const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate);
extern PaError __stdcall PA_OpenStream(PaStream** stream, const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate, unsigned long framesPerBuffer, PaStreamFlags streamFlags, PaStreamCallback* streamCallback, void* userData);
extern PaError __stdcall PA_OpenDefaultStream(PaStream** stream, int numInputChannels, int numOutputChannels, PaSampleFormat sampleFormat, double sampleRate, unsigned long framesPerBuffer, PaStreamCallback* streamCallback, void* userData);
extern PaError __stdcall PA_CloseStream(PaStream* stream);
extern PaError __stdcall PA_SetStreamFinishedCallback(PaStream *stream, PaStreamFinishedCallback* streamFinishedCallback);
extern PaError __stdcall PA_StartStream(PaStream* stream);
extern PaError __stdcall PA_StopStream(PaStream* stream);
extern PaError __stdcall PA_AbortStream(PaStream* stream);
extern PaError __stdcall PA_IsStreamStopped(PaStream* stream);
extern PaError __stdcall PA_IsStreamActive(PaStream* stream);
extern const PaStreamInfo* __stdcall PA_GetStreamInfo(PaStream* stream);
extern PaTime __stdcall PA_GetStreamTime(PaStream* stream);
extern double __stdcall PA_GetStreamCpuLoad(PaStream* stream);
extern PaError __stdcall PA_ReadStream(PaStream* stream, void* buffer, unsigned long frames);
extern PaError __stdcall PA_WriteStream(PaStream* stream, const void* buffer, unsigned long frames);
extern signed long __stdcall PA_GetStreamReadAvailable(PaStream* stream);
extern signed long __stdcall PA_GetStreamWriteAvailable(PaStream* stream);
extern PaError __stdcall PA_GetSampleSize(PaSampleFormat format) ;
extern void __stdcall PA_Sleep(long msec);


#ifdef __cplusplus
}
#endif

#endif
