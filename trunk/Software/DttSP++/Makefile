all:		DttSP.a

CFLAGS :=	-I../Common $(CFLAGS)

OBJS :=		AGC.o AMDemod.o AMMod.o BlockLMS.o Compand.o CorrectIQ.o CXB.o DCBlock.o FIR.o FMDemod.o FMMod.o \
		FilterOVSV.o GraphicEQ.o LMS.o Meter.o NoiseBlanker.o Oscillator.o Resampler.o Spectrum.o \
		SpeechProc.o SpotTone.o Squelch.o TX.o Window.o banal.o sdr.o sdrexport.o SSBDemod.o SSBMod.o \
		thunk.o update.o winmain.o

DttSP.a:	$(OBJS)
		ar rcs DttSP.a $(OBJS)

AGC.o:		AGC.cpp AGC.h banal.h Complex.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c AGC.cpp

AMDemod.o:	AMDemod.cpp AMDemod.h Complex.h CXB.h Demod.h FromSys.h
		$(CC) $(CFLAGS) -c AMDemod.cpp

AMMod.o:	AMMod.cpp AMMod.h Complex.h CXB.h FromSys.h Mod.h
		$(CC) $(CFLAGS) -c AMMod.cpp

BlockLMS.o:	BlockLMS.cpp BlockLMS.h banal.h CXB.h CXOps.h DataTypes.h
		$(CC) $(CFLAGS) -c BlockLMS.cpp

Compand.o:	Compand.cpp Compand.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c Compand.cpp

CorrectIQ.o: 	CorrectIQ.cpp CorrectIQ.h bufvec.h
		$(CC) $(CFLAGS) -c CorrectIQ.cpp

CXB.o:		CXB.cpp CXB.h Complex.h CXOps.h FromSys.h
		$(CC) $(CFLAGS) -c CXB.cpp

DCBlock.o:	DCBlock.cpp DCBlock.h Complex.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c DCBlock.cpp

FIR.o:		FIR.cpp FIR.h banal.h Complex.h CXB.h FromSys.h Window.h
		$(CC) $(CFLAGS) -c FIR.cpp

FMDemod.o:	FMDemod.cpp FMDemod.h banal.h Complex.h CXB.h Demod.h FromSys.h
		$(CC) $(CFLAGS) -c FMDemod.cpp

FMMod.o:	FMMod.cpp FMMod.h banal.h Complex.h CXB.h FromSys.h Mod.h
		$(CC) $(CFLAGS) -c FMMod.cpp

FilterOVSV.o: 	FilterOVSV.cpp FilterOVSV.h banal.h Complex.h CXB.h CXOps.h FIR.h
		$(CC) $(CFLAGS) -c FilterOVSV.cpp

GraphicEQ.o: 	GraphicEQ.cpp GraphicEQ.h Complex.h CXB.h FilterOVSV.h FIR.h FromSys.h
		$(CC) $(CFLAGS) -c GraphicEQ.cpp

LMS.o:		LMS.cpp LMS.h banal.h CXB.h CXOps.h DataTypes.h
		$(CC) $(CFLAGS) -c LMS.cpp

Meter.o:	Meter.cpp Meter.h banal.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c Meter.cpp

NoiseBlanker.o: NoiseBlanker.cpp NoiseBlanker.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c NoiseBlanker.cpp

Oscillator.o: 	Oscillator.cpp Oscillator.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c Oscillator.cpp

Resampler.o: 	Resampler.cpp Resampler.h banal.h CXB.h CXOps.h FIR.h
		$(CC) $(CFLAGS) -c Resampler.cpp

Spectrum.o:	Spectrum.cpp Spectrum.h banal.h CXB.h FIR.h FromSys.h Window.h
		$(CC) $(CFLAGS) -c Spectrum.cpp

SpeechProc.o: 	SpeechProc.cpp SpeechProc.h banal.h Complex.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c SpeechProc.cpp

SpotTone.o:	SpotTone.cpp SpotTone.h CXB.h FromSys.h Oscillator.h
		$(CC) $(CFLAGS) -c SpotTone.cpp

Squelch.o:	Squelch.cpp Squelch.h bufvec.h
		$(CC) $(CFLAGS) -c Squelch.cpp

SSBDemod.o:	SSBDemod.cpp SSBDemod.h Complex.h CXB.h Demod.h FromSys.h
		$(CC) $(CFLAGS) -c SSBDemod.cpp

SSBMod.o:	SSBMod.cpp SSBMod.h Complex.h CXB.h FromSys.h Mod.h
		$(CC) $(CFLAGS) -c SSBMod.cpp

TX.o:		TX.cpp TX.h AGC.h AMMod.h Compand.h CorrectIQ.h CXB.h DCBlock.h Defs.h FilterOVSV.h FMMod.h GraphicEQ.h \
		Meter.h Mod.h Oscillator.h Spectrum.h SpeechProc.h Squelch.h SSBMod.h
		$(CC) $(CFLAGS) -c TX.cpp

Window.o:	Window.cpp Window.h banal.h FromSys.h
		$(CC) $(CFLAGS) -c Window.cpp

banal.o:	banal.cpp banal.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c banal.cpp

sdr.o:		sdr.cpp AMDemod.h AMMod.h banal.h FMDemod.h FMMod.h sdrexport.h SSBDemod.h SSBMod.h
		$(CC) $(CFLAGS) -c sdr.cpp

sdrexport.o: 	sdrexport.cpp sdrexport.h AGC.h AMDemod.h BlockLMS.h Compand.h CorrectIQ.h CXB.h DCBlock.h \
		Defs.h FIR.h FilterOVSV.h FMDemod.h FromSys.h GraphicEQ.h LMS.h Meter.h NoiseBlanker.h Oscillator.h \
		../Common/RingBuffer.h Spectrum.h SpeechProc.h SpotTone.h Squelch.h TX.h
		$(CC) $(CFLAGS) -c sdrexport.cpp

thunk.o:	thunk.cpp thunk.h FromSys.h
		$(CC) $(CFLAGS) -c thunk.cpp

update.o:	update.cpp banal.h CXB.h FromSys.h ../Common/RingBuffer.h thunk.h sdrexport.h
		$(CC) $(CFLAGS) -c update.cpp

winmain.o:	winmain.cpp local.h sdrexport.h
		$(CC) $(CFLAGS) -c winmain.cpp

clean:
		rm -f core DttSP.a *.o *~ *.bak
