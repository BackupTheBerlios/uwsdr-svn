all:		Common.a

OBJS := 	AudioDevInfo.o Downsampler.o Frequency.o GriffinPowerMate.o HackRFController.o JackReaderWriter.o Log.o NullController.o NullReader.o \
					NullWriter.o RingBuffer.o SDRDescrFile.o SerialControl.o SerialInterface.o SignalReader.o SI570Controller.o SoundCardReaderWriter.o \
					SoundFileReader.o SoundFileWriter.o SRTXRXController.o ThreadReader.o ThreeToneReader.o TwoToneReader.o UDPDataReader.o UDPDataWriter.o \
					Upsampler.o UWSDRController.o UWSDRDataReader.o UWSDRDataWriter.o

Common.a:	$(OBJS)
		ar rcs Common.a $(OBJS)

AudioDevInfo.o:	AudioDevInfo.cpp AudioDevInfo.h Common.h
		$(CC) $(CFLAGS) -c AudioDevInfo.cpp

Downsampler.o:		Downsampler.cpp Downsampler.h
		$(CC) $(CFLAGS) -c Downsampler.cpp

Frequency.o:	Frequency.cpp Frequency.h
		$(CC) $(CFLAGS) -c Frequency.cpp

GriffinPowerMate.o: GriffinPowerMate.cpp GriffinPowerMate.h DialInterface.h DialCallback.h
		$(CC) $(CFLAGS) -c GriffinPowerMate.cpp

HackRFController.o:	HackRFController.cpp HackRFController.h ControlInterface.h Frequency.h SDRController.h
		$(CC) $(CFLAGS) -c HackRFController.cpp

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

SerialInterface.o:	SerialInterface.cpp SerialInterface.h Common.h ExternalInterface.h SerialControl.h
		$(CC) $(CFLAGS) -c SerialInterface.cpp

SignalReader.o:	SignalReader.cpp SignalReader.h ThreadReader.h
		$(CC) $(CFLAGS) -c SignalReader.cpp

SI570Controller.o:	SI570Controller.cpp SI570Controller.h ControlInterface.h Frequency.h SDRController.h
		$(CC) $(CFLAGS) -c SI570Controller.cpp

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

TwoToneReader.o:		TwoToneReader.cpp TwoToneReader.h ThreadReader.h
		$(CC) $(CFLAGS) -c TwoToneReader.cpp

UDPDataReader.o:		UDPDataReader.cpp UDPDataReader.h RawDataCallback.h
		$(CC) $(CFLAGS) -c UDPDataReader.cpp

UDPDataWriter.o:		UDPDataWriter.cpp UDPDataWriter.h
		$(CC) $(CFLAGS) -c UDPDataWriter.cpp

Upsampler.o:				Upsampler.cpp Upsampler.h
		$(CC) $(CFLAGS) -c Upsampler.cpp

UWSDRController.o:	UWSDRController.cpp UWSDRController.h SDRController.h Frequency.h ControlInterface.h UDPDataReader.h \
			UDPDataWriter.h RawDataCallback.h
		$(CC) $(CFLAGS) -c UWSDRController.cpp

UWSDRDataReader.o:	UWSDRDataReader.cpp UWSDRDataReader.h DataReader.h RingBuffer.h  UDPDataReader.h RawDataCallback.h
		$(CC) $(CFLAGS) -c UWSDRDataReader.cpp

UWSDRDataWriter.o:	UWSDRDataWriter.cpp UWSDRDataWriter.h RingBuffer.h UDPDataWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c UWSDRDataWriter.cpp

clean:
		rm -f core Common.a *.o *~ *.bak
