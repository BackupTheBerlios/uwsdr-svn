all:		UWSDR

CFLAGS := 	-I../FFTW -I../PA19 $(CFLAGS)

OBJS :=	DSPControl.o DTTSPControl.o FreqDial.o FreqDisplay.o FreqKeypad.o Frequency.o \
		InfoBox.o Log.o NullReader.o NullWriter.o RingBuffer.o \
		SDRDescrFile.o SDRParameters.o SMeter.o SignalReader.o SoundCardReader.o \
		SoundCardWriter.o SoundFileReader.o SoundFileWriter.o SpectrumDisplay.o \
		UWSDRApp.o UWSDRControl.o UWSDRData.o UWSDRFrame.o UWSDRPreferences.o \
		VolumeDial.o

UWSDR:	$(OBJS)
		$(CC) $(LDFLAGS) -o UWSDR $(OBJS) $(LIBS)

DSPControl.o:	DSPControl.cpp DSPControl.h DataReader.h DataWriter.h DttSPControl.h RingBuffer.h SoundFileWriter.h 
		$(CC) $(CFLAGS) -c DSPControl.cpp

DTTSPControl.o:	DTTSPControl.cpp DTTSPControl.h DTTSPExports.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c DTTSPControl.cpp

FreqDial.o:		FreqDial.cpp FreqDial.h DialInterface.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c FreqDial.cpp

FreqDisplay.o:	FreqDisplay.cpp FreqDisplay.h Frequency.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c FreqDisplay.cpp

FreqKeypad.o:	FreqKeypad.cpp FreqKeypad.h Frequency.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c FreqKeypad.cpp

Frequency.o:	Frequency.cpp Frequency.h
		$(CC) $(CFLAGS) -c Frequency.cpp

InfoBox.o:		InfoBox.cpp InfoBox.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c InfoBox.cpp

Log.o:		Log.cpp Log.h
		$(CC) $(CFLAGS) -c Log.cpp

NullReader.o:	NullReader.cpp NullReader.h DataReader.h
		$(CC) $(CFLAGS) -c NullReader.cpp

NullWriter.o:	NullWriter.cpp NullWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c NullWriter.cpp

RingBuffer.o:	RingBuffer.cpp RingBuffer.h Frequency.h
		$(CC) $(CFLAGS) -c RingBuffer.cpp

SDRDescrFile.o:	SDRDescrFile.cpp SDRDescrFile.h Frequency.h
		$(CC) $(CFLAGS) -c SDRDescrFile.cpp

SDRParameters.o:	SDRParameters.cpp SDRParameters.h Frequency.h
		$(CC) $(CFLAGS) -c SDRParameters.cpp

SMeter.o:		SMeter.cpp SMeter.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c SMeter.cpp

SignalReader.o:	SignalReader.cpp SignalReader.h DataReader.h
		$(CC) $(CFLAGS) -c SignalReader.cpp

SoundCardReader.o: SoundCardReader.cpp SoundCardReader.h DataReader.h
		$(CC) $(CFLAGS) -c SoundCardReader.cpp

SoundCardWriter.o: SoundCardWriter.cpp SoundCardWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c SoundCardWriter.cpp

SoundFileReader.o: SoundFileReader.cpp SoundFileReader.h DataReader.h
		$(CC) $(CFLAGS) -c SoundFileReader.cpp

SoundFileWriter.o: SoundFileWriter.cpp SoundFileWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c SoundFileWriter.cpp

SpectrumDisplay.o: SpectrumDisplay.cpp SpectrumDisplay.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c SpectrumDisplay.cpp

UWSDRApp.o:		UWSDRApp.cpp UWSDRApp.h Log.h SDRDescrFile.h SDRParameters.h UWSDRDefs.h UWSDRFrame.h
		$(CC) $(CFLAGS) -c UWSDRApp.cpp

UWSDRControl.o:	UWSDRControl.cpp UWSDRControl.h ControlInterface.h Frequency.h SDRControl.h
		$(CC) $(CFLAGS) -c UWSDRControl.cpp

UWSDRData.o:	UWSDRData.cpp UWSDRData.h DataReader.h DataWriter.h RingBuffer.h
		$(CC) $(CFLAGS) -c UWSDRData.cpp

UWSDRFrame.o:	UWSDRFrame.cpp UWSDRFrame.h ControlInterface.h DialInterface.h DSPControl.h FreqDial.h FreqDisplay.h \
			FreqKeypad.h Frequency.h InfoBox.h NullReader.h NullWriter.h SDRControl.h SDRParameters.h SignalReader.h \
			SMeter.h SoundCardReader.h SoundCardWriter.h SoundFileReader.h SoundFileWriter.h SpectrumDisplay.h \
			UWSDRApp.h UWSDRControl.h UWSDRData.h UWSDRDefs.h UWSDRPreferences.h UWSDR.xpm
		$(CC) $(CFLAGS) -c UWSDRFrame.cpp

UWSDRPreferences.o: UWSDRPreferences.cpp UWSDRPreferences.h SDRParameters.h UWSDRApp.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c UWSDRPreferences.cpp

VolumeDial.o:	VolumeDial.cpp VolumeDial.h DialInterface.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c VolumeDial.cpp

clean:
		rm -f core *.o *~ *.bak
