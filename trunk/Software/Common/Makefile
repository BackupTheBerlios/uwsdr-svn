all:		Common.a

OBJS := 	AudioDevInfo.o Frequency.o HPSDRAudioReader.o HPSDRAudioWriter.o HPSDRDataReader.o HPSDRDataWriter.o \
		HPSDRReaderWriter.o JackReaderWriter.o Log.o NullController.o NullReader.o NullWriter.o RingBuffer.o \
		SDRDescrFile.o SerialControl.o SignalReader.o SoundCardReaderWriter.o SoundFileReader.o SoundFileWriter.o \
		SRTXRXController.o ThreadReader.o ThreeToneReader.o TwoToneReader.o UDPDataReader.o UDPDataWriter.o \
		USBBulkReaderWriter.o

Common.a:	$(OBJS)
		ar rcs Common.a $(OBJS)

AudioDevInfo.o:	AudioDevInfo.cpp AudioDevInfo.h Common.h
		$(CC) $(CFLAGS) -c AudioDevInfo.cpp

Frequency.o:	Frequency.cpp Frequency.h
		$(CC) $(CFLAGS) -c Frequency.cpp

HPSDRAudioReader.o:	HPSDRAudioReader.cpp HPSDRAudioReader.h DataCallback.h DataReader.h HPSDRReaderWriter.h
		$(CC) $(CFLAGS) -c HPSDRAudioReader.cpp

HPSDRAudioWriter.o:	HPSDRAudioWriter.cpp HPSDRAudioWriter.h DataWriter.h HPSDRReaderWriter.h
		$(CC) $(CFLAGS) -c HPSDRAudioWriter.cpp

HPSDRDataReader.o:	HPSDRDataReader.cpp HPSDRDataReader.h DataCallback.h DataReader.h HPSDRReaderWriter.h
		$(CC) $(CFLAGS) -c HPSDRDataReader.cpp

HPSDRDataWriter.o:	HPSDRDataWriter.cpp HPSDRDataWriter.h DataWriter.h HPSDRReaderWriter.h
		$(CC) $(CFLAGS) -c HPSDRDataWriter.cpp

HPSDRReaderWriter.o:	HPSDRReaderWriter.cpp HPSDRReaderWriter.h ControlInterface.h DataCallback.h Frequency.h SDRController.h \
			RawDataCallback.h
		$(CC) $(CFLAGS) -c HPSDRReaderWriter.cpp

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

UDPDataReader.o: UDPDataReader.cpp UDPDataReader.h RawDataCallback.h
		$(CC) $(CFLAGS) -c UDPDataReader.cpp

UDPDataWriter.o: UDPDataWriter.cpp UDPDataWriter.h
		$(CC) $(CFLAGS) -c UDPDataWriter.cpp

USBBulkReaderWriter.o:	USBBulkReaderWriter.cpp USBBulkReaderWriter.h RawDataCallback.h
		$(CC) $(CFLAGS) -c USBBulkReaderWriter.cpp

clean:
		rm -f core Common.a *.o *~ *.bak
