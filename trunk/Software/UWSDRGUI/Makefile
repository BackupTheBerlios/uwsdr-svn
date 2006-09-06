all:		UWSDR

CFLAGS		:= -I../Common $(CFLAGS)

LIBS		:= -lportaudio -lfftw3f -lfftw3 -lsndfile  $(LIBS)

OBJS :=		CWKeyboard.o CWKeyer.o DSPControl.o DTTSPControl.o FreqDial.o FreqDisplay.o \
		FreqKeypad.o InfoBox.o Log.o NullReader.o NullWriter.o \
		SDRDescrFile.o SDRParameters.o SMeter.o SignalReader.o SoundCardReader.o \
		SoundCardWriter.o SoundFileReader.o SoundFileWriter.o SpectrumDisplay.o \
		UWSDRApp.o UWSDRControl.o UWSDRData.o UWSDRFrame.o UWSDRPreferences.o \
		VolumeDial.o

UWSDR:		$(OBJS)
		$(CC) $(LDFLAGS) -o UWSDR $(OBJS) ../Common/Common.a ../DttSP++/DttSP.a $(LIBS)

CWKeyboard.o:	CWKeyboard.cpp CWKeyboard.h UWSDRApp.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c CWKeyboard.cpp

CWKeyer.o:	CWKeyer.cpp CWKeyer.h DataReader.h UWSDRApp.h ../Common/RingBuffer.h
		$(CC) $(CFLAGS) -c CWKeyer.cpp

DSPControl.o:	DSPControl.cpp DSPControl.h CWKeyer.h DataReader.h DataWriter.h DTTSPControl.h \
		../Common/RingBuffer.h SoundFileWriter.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c DSPControl.cpp

DTTSPControl.o:	DTTSPControl.cpp DTTSPControl.h DTTSPExports.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c DTTSPControl.cpp

FreqDial.o:	FreqDial.cpp FreqDial.h DialInterface.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c FreqDial.cpp

FreqDisplay.o:	FreqDisplay.cpp FreqDisplay.h ../Common/Frequency.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c FreqDisplay.cpp

FreqKeypad.o:	FreqKeypad.cpp FreqKeypad.h ../Common/Frequency.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c FreqKeypad.cpp

InfoBox.o:	InfoBox.cpp InfoBox.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c InfoBox.cpp

Log.o:		Log.cpp Log.h
		$(CC) $(CFLAGS) -c Log.cpp

NullReader.o:	NullReader.cpp NullReader.h DataReader.h
		$(CC) $(CFLAGS) -c NullReader.cpp

NullWriter.o:	NullWriter.cpp NullWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c NullWriter.cpp

SDRDescrFile.o:	SDRDescrFile.cpp SDRDescrFile.h ../Common/Frequency.h
		$(CC) $(CFLAGS) -c SDRDescrFile.cpp

SDRParameters.o: SDRParameters.cpp SDRParameters.h UWSDRDefs.h ../Common/Frequency.h
		$(CC) $(CFLAGS) -c SDRParameters.cpp

SMeter.o:	SMeter.cpp SMeter.h UWSDRDefs.h
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

UWSDRApp.o:	UWSDRApp.cpp UWSDRApp.h Log.h SDRDescrFile.h SDRParameters.h UWSDRDefs.h UWSDRFrame.h ../Common/Version.h
		$(CC) $(CFLAGS) -c UWSDRApp.cpp

UWSDRControl.o:	UWSDRControl.cpp UWSDRControl.h ControlInterface.h ../Common/Frequency.h SDRControl.h
		$(CC) $(CFLAGS) -c UWSDRControl.cpp

UWSDRData.o:	UWSDRData.cpp UWSDRData.h DataReader.h DataWriter.h ../Common/RingBuffer.h
		$(CC) $(CFLAGS) -c UWSDRData.cpp

UWSDRFrame.o:	UWSDRFrame.cpp UWSDRFrame.h ControlInterface.h CWKeyboard.h DialInterface.h DSPControl.h FreqDial.h FreqDisplay.h \
		FreqKeypad.h ../Common/Frequency.h InfoBox.h NullReader.h NullWriter.h SDRControl.h SDRParameters.h \
		SignalReader.h SMeter.h SoundCardReader.h SoundCardWriter.h SoundFileReader.h SoundFileWriter.h \
		SpectrumDisplay.h UWSDRApp.h UWSDRControl.h UWSDRData.h UWSDRDefs.h UWSDRPreferences.h UWSDRXpm.h \
		../Common/Version.h
		$(CC) $(CFLAGS) -c UWSDRFrame.cpp

UWSDRPreferences.o: UWSDRPreferences.cpp UWSDRPreferences.h DSPControl.h SDRParameters.h UWSDRApp.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c UWSDRPreferences.cpp

VolumeDial.o:	VolumeDial.cpp VolumeDial.h DialInterface.h UWSDRDefs.h
		$(CC) $(CFLAGS) -c VolumeDial.cpp

install:	UWSDR
		install -g bin -o root -m 0775 UWSDR $(BINDIR)

clean:
		rm -f core UWSDR *.o *~ *.bak
