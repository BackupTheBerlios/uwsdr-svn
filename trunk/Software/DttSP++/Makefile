all:		DttSP.a

CFLAGS :=	-I../Common $(CFLAGS)

OBJS :=		AGC.o AMDemod.o AMMod.o BlockLMS.o Compand.o CorrectIQ.o CXB.o DCBlock.o DttSP.o FIR.o FMDemod.o \
		FMMod.o FilterOVSV.o GraphicEQ.o LMS.o Meter.o NoiseBlanker.o Oscillator.o Resampler.o RX.o Spectrum.o \
		SpeechProc.o SpotTone.o Squelch.o TX.o Window.o SSBDemod.o SSBMod.o Utils;o update.o

DttSP.a:	$(OBJS)
		ar rcs DttSP.a $(OBJS)

AGC.o:		AGC.cpp AGC.h Complex.h CXB.h FromSys.h Utils.h
		$(CC) $(CFLAGS) -c AGC.cpp

AMDemod.o:	AMDemod.cpp AMDemod.h Complex.h CXB.h Demod.h FromSys.h
		$(CC) $(CFLAGS) -c AMDemod.cpp

AMMod.o:	AMMod.cpp AMMod.h Complex.h CXB.h FromSys.h Mod.h
		$(CC) $(CFLAGS) -c AMMod.cpp

BlockLMS.o:	BlockLMS.cpp BlockLMS.h CXB.h CXOps.h DataTypes.h Utils.h
		$(CC) $(CFLAGS) -c BlockLMS.cpp

Compand.o:	Compand.cpp Compand.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c Compand.cpp

CorrectIQ.o: 	CorrectIQ.cpp CorrectIQ.h CXB.h
		$(CC) $(CFLAGS) -c CorrectIQ.cpp

CXB.o:		CXB.cpp CXB.h Complex.h CXOps.h FromSys.h
		$(CC) $(CFLAGS) -c CXB.cpp

DCBlock.o:	DCBlock.cpp DCBlock.h Complex.h CXB.h FromSys.h Meter.h Spectrum.h
		$(CC) $(CFLAGS) -c DCBlock.cpp

DttSP.o:	DttSP.cpp DttSP.h Defs.h ../Common/RingBuffer.h RX.h TX.h
		$(CC) $(CFLAGS) -c DttSP.cpp

FIR.o:		FIR.cpp FIR.h Complex.h CXB.h FromSys.h Window.h Utils.h
		$(CC) $(CFLAGS) -c FIR.cpp

FMDemod.o:	FMDemod.cpp FMDemod.h Complex.h CXB.h Demod.h FromSys.h Utils.h
		$(CC) $(CFLAGS) -c FMDemod.cpp

FMMod.o:	FMMod.cpp FMMod.h Complex.h CXB.h FromSys.h Mod.h Utils.h
		$(CC) $(CFLAGS) -c FMMod.cpp

FilterOVSV.o: 	FilterOVSV.cpp FilterOVSV.h Complex.h CXB.h CXOps.h FIR.h Utils.h
		$(CC) $(CFLAGS) -c FilterOVSV.cpp

GraphicEQ.o: 	GraphicEQ.cpp GraphicEQ.h Complex.h CXB.h FilterOVSV.h FIR.h FromSys.h
		$(CC) $(CFLAGS) -c GraphicEQ.cpp

LMS.o:		LMS.cpp LMS.h CXB.h CXOps.h DataTypes.h Utils.h
		$(CC) $(CFLAGS) -c LMS.cpp

Meter.o:	Meter.cpp Meter.h CXB.h FromSys.h Utils.h
		$(CC) $(CFLAGS) -c Meter.cpp

NoiseBlanker.o: NoiseBlanker.cpp NoiseBlanker.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c NoiseBlanker.cpp

Oscillator.o: 	Oscillator.cpp Oscillator.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c Oscillator.cpp

Resampler.o: 	Resampler.cpp Resampler.h CXB.h CXOps.h FIR.h Utils.h
		$(CC) $(CFLAGS) -c Resampler.cpp

RX.o:		RX.cpp RX.h AGC.h AMDemod.h BlockLMS.h Compand.h CorrectIQ.h CXB.h Defs.h Demod.h FilterOVSV.h FMDemod.h \
		GraphicEQ.h LMS.h Meter.h NoiseBlanker.h Oscillator.h Spectrum.h SpotTone.h Squelch.h SSBDemod.h
		$(CC) $(CFLAGS) -c RX.cpp

Spectrum.o:	Spectrum.cpp Spectrum.h CXB.h FIR.h FromSys.h Utils.h Window.h
		$(CC) $(CFLAGS) -c Spectrum.cpp

SpeechProc.o: 	SpeechProc.cpp SpeechProc.h Complex.h CXB.h FromSys.h Utils.h
		$(CC) $(CFLAGS) -c SpeechProc.cpp

SpotTone.o:	SpotTone.cpp SpotTone.h CXB.h FromSys.h Oscillator.h
		$(CC) $(CFLAGS) -c SpotTone.cpp

Squelch.o:	Squelch.cpp Squelch.h CXB.h
		$(CC) $(CFLAGS) -c Squelch.cpp

SSBDemod.o:	SSBDemod.cpp SSBDemod.h Complex.h CXB.h Demod.h FromSys.h
		$(CC) $(CFLAGS) -c SSBDemod.cpp

SSBMod.o:	SSBMod.cpp SSBMod.h Complex.h CXB.h FromSys.h Mod.h
		$(CC) $(CFLAGS) -c SSBMod.cpp

TX.o:		TX.cpp TX.h AGC.h AMMod.h Compand.h CorrectIQ.h CXB.h DCBlock.h Defs.h FilterOVSV.h FMMod.h GraphicEQ.h \
		Meter.h Mod.h Oscillator.h Spectrum.h SpeechProc.h Squelch.h SSBMod.h
		$(CC) $(CFLAGS) -c TX.cpp

Utils.o:	Utils.cpp Utils.h CXB.h FromSys.h
		$(CC) $(CFLAGS) -c Utils.cpp

Window.o:	Window.cpp Window.h FromSys.h Utils.h
		$(CC) $(CFLAGS) -c Window.cpp

clean:
		rm -f core DttSP.a *.o *~ *.bak
