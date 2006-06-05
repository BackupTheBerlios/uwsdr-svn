all:		DttSP.a

OBJS :=	AGC.o AMDemod.o Compand.o CorrectIQ.o DCBlock.o FIR.o FMDemod.o FilterOVSV.o GraphicEQ.o Meter.o \
		NoiseBlanker.o Oscillator.o Resample.o ResampleF.o SSBDemod.o Spectrum.o SpeechProc.o SpotTone.o \
		Squelch.o Window.o banal.o bufvec.o cxops.o lmadf.o ringb.o sdr.o sdrexport.o thunk.o update.o \
		winmain.o

DttSP.a:	$(OBJS)
		ar rcs DttSP.a $(OBJS)

AGC.o:	AGC.cpp AGC.h banal.h bufvec.h complex.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c AGC.cpp

AMDemod.o:	AMDemod.cpp AMDemod.h bufvec.h complex.h datatypes.h Demod.h fromsys.h
		$(CC) $(CFLAGS) -c AMDemod.cpp

Compand.o:	Compand.cpp Compand.h bufvec.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c Compand.cpp

CorrectIQ.o: CorrectIQ.cpp CorrectIQ.h bufvec.h
		$(CC) $(CFLAGS) -c CorrectIQ.cpp

DCBlock.o:	DCBlock.cpp DCBlock.h bufvec.h complex.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c DCBlock.cpp

FIR.o:	FIR.cpp FIR.h banal.h bufvec.h complex.h datatypes.h fromsys.h Window.h
		$(CC) $(CFLAGS) -c FIR.cpp

FMDemod.o:	FMDemod.cpp FMDemod.h banal.h bufvec.h complex.h datatypes.h Demod.h fromsys.h
		$(CC) $(CFLAGS) -c FMDemod.cpp

FilterOVSV.o: FilterOVSV.cpp FilterOVSV.h banal.h bufvec.h complex.h cxops.h datatypes.h FIR.h
		$(CC) $(CFLAGS) -c FilterOVSV.cpp

GraphicEQ.o: GraphicEQ.cpp GraphicEQ.h bufvec.h complex.h datatypes.h FilterOVSV.h FIR.h fromsys.h
		$(CC) $(CFLAGS) -c GraphicEQ.cpp

Meter.o:	Meter.cpp Meter.h banal.h bufvec.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c Meter.cpp

NoiseBlanker.o: NoiseBlanker.cpp NoiseBlanker.h bufvec.h fromsys.h
		$(CC) $(CFLAGS) -c NoiseBlanker.cpp

Oscillator.o: Oscillator.cpp Oscillator.h bufvec.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c Oscillator.cpp

Resample.o:	Resample.cpp Resample.h banal.h cxops.h FIR.h
		$(CC) $(CFLAGS) -c Resample.cpp

ResampleF.o: ResampleF.cpp ResampleF.h banal.h FIR.h
		$(CC) $(CFLAGS) -c ResampleF.cpp

SSBDemod.o:	SSBDemod.cpp SSBDemod.h bufvec.h Demod.h
		$(CC) $(CFLAGS) -c SSBDemod.cpp

Spectrum.o:	Spectrum.cpp Spectrum.h banal.h bufvec.h datatypes.h FIR.h fromsys.h Window.h
		$(CC) $(CFLAGS) -c Spectrum.cpp

SpeechProc.o: Speechproc.cpp SpeechProc.h banal.h bufvec.h complex.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c SpeechProc.cpp

SpotTone.o:	SpotTone.cpp SpotTone.h bufvec.h datatypes.h fromsys.h Oscillator.h
		$(CC) $(CFLAGS) -c SpotTone.cpp

Squelch.o:	Squelch.cpp Squelch.h bufvec.h
		$(CC) $(CFLAGS) -c Squelch.cpp

Window.o:	Window.cpp Window.h banal.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c Window.cpp

banal.o:	banal.cpp banal.h bufvec.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c banal.cpp

bufvec.o:	bufvec.cpp bufvec.h complex.h cxops.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c bufvec.cpp

cxops.o:	cxops.cpp cxops.h banal.h complex.h datatypes.h fromsys.h
		$(CC) $(CFLAGS) -c cxops.cpp

lmadf.o:	lmadf.cpp lmadf.h banal.h bufvec.h cxops.h datatypes.h
		$(CC) $(CFLAGS) -c lmadf.cpp

ringb.o:	ringb.cpp ringb.h fromsys.h
		$(CC) $(CFLAGS) -c ringb.cpp

sdr.o:	sdr.cpp banal.h sdrexport.h
		$(CC) $(CFLAGS) -c sdr.cpp

sdrexport.o: sdrexport.cpp sdrexport.h AGC.h AMDemod.h bufvec.h Compand.h CorrectIQ.h datatypes.h DCBlock.h defs.h \
		FIR.h FilterOVSV.h FMDemod.h fromsys.h GraphicEQ.h lmadf.h Meter.h NoiseBlanker.h Oscillator.h ringb.h \
		Spectrum.h SpeechProc.h SpotTone.h
		$(CC) $(CFLAGS) -c sdrexport.cpp

thunk.o:	thunk.cpp thunk.h fromsys.h
		$(CC) $(CFLAGS) -c thunk.cpp

update.o:	update.cpp update.h banal.h bufvec.h datatypes.h fromsys.h ringb.h thunk.h sdrexport.h
		$(CC) $(CFLAGS) -c update.cpp

winmain.o:	winmain.cpp local.h sdrexport.h
		$(CC) $(CFLAGS) -c winmain.cpp

clean:
		rm -f core *.a *.o *~ *.bak
