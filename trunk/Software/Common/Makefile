all:		Common.a

OBJS := 	AudioDevInfo.o Frequency.o JackReaderWriter.o Log.o NullController.o NullReader.o NullWriter.o RingBuffer.o \
		SDRDataReader.o SDRDataWriter.o SDRDescrFile.o SerialControl.o SignalReader.o SoundCardReaderWriter.o \
		SoundFileReader.o SoundFileWriter.o SRTXRXController.o ThreadReader.o ThreeToneReader.o TwoToneReader.o \
		UWSDRController.o

Common.a:	$(OBJS)
		ar rcs Common.a $(OBJS)

AudioDevInfo.o:	AudioDevInfo.cpp AudioDevInfo.h Common.h
		$(CC) $(CFLAGS) -c AudioDevInfo.cpp

Frequency.o:	Frequency.cpp Frequency.h
		$(CC) $(CFLAGS) -c Frequency.cpp

JackReaderWriter.o: JackReaderWriter.cpp JackReaderWriter.h DataCallback.h DataReader.h DataWriter.h RingBuffer.h
		$(CC) $(CFLAGS) -c JackReaderWriter.cpp

Log.o:		Log.cpp Log.h
		$(CC) $(CFLAGS) -c Log.cpp

NullController.o:	NullController.cpp NullController.h ControlInterface.h Frequency.h SDRController.h
		$(CC) $(CFLAGS) -c NullController.cpp

NullReader.o:	NullReader.cpp NullReader.h ThreadReader.h
		$(CC) $(CFLAGS) -c NullReader.cpp

NullWriter.o:	NullWriter.cpp NullWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c NullWriter.cpp

RingBuffer.o:	RingBuffer.cpp RingBuffer.h
		$(CC) $(CFLAGS) -c RingBuffer.cpp

SDRDataReader.o: SDRDataReader.cpp SDRDataReader.h DataReader.h RingBuffer.h
		$(CC) $(CFLAGS) -c SDRDataReader.cpp

SDRDataWriter.o: SDRDataWriter.cpp SDRDataWriter.h DataWriter.h RingBuffer.h
		$(CC) $(CFLAGS) -c SDRDataWriter.cpp

SDRDescrFile.o:	SDRDescrFile.cpp SDRDescrFile.h Frequency.h
		$(CC) $(CFLAGS) -c SDRDescrFile.cpp

SerialControl.o:	SerialControl.cpp SerialControl.h
		$(CC) $(CFLAGS) -c SerialControl.cpp

SignalReader.o:	SignalReader.cpp SignalReader.h ThreadReader.h
		$(CC) $(CFLAGS) -c SignalReader.cpp

SoundCardReaderWriter.o: SoundCardReaderWriter.cpp SoundCardReaderWriter.h DataCallback.h DataReader.h DataWriter.h RingBuffer.h
		$(CC) $(CFLAGS) -c SoundCardReaderWriter.cpp

SoundFileReader.o: SoundFileReader.cpp SoundFileReader.h ThreadReader.h
		$(CC) $(CFLAGS) -c SoundFileReader.cpp

SoundFileWriter.o: SoundFileWriter.cpp SoundFileWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c SoundFileWriter.cpp

SRTXRXController.o:	SRTXRXController.cpp SRTXRXController.h ControlInterface.h Frequency.h SDRController.h SerialControl.h
		$(CC) $(CFLAGS) -c SRTXRXController.cpp

ThreadReader.o:	ThreadReader.cpp ThreadReader.h DataReader.h DataCallback.h
		$(CC) $(CFLAGS) -c ThreadReader.cpp

ThreeToneReader.o:	ThreeToneReader.cpp ThreeToneReader.h ThreadReader.h
		$(CC) $(CFLAGS) -c ThreeToneReader.cpp

TwoToneReader.o:	TwoToneReader.cpp TwoToneReader.h ThreadReader.h
		$(CC) $(CFLAGS) -c TwoToneReader.cpp

UWSDRController.o:	UWSDRController.cpp UWSDRController.h ControlInterface.h Frequency.h SDRController.h
		$(CC) $(CFLAGS) -c UWSDRController.cpp

clean:
		rm -f core Common.a *.o *~ *.bak
