all:		SDREmulator

CFLAGS :=	-I../Common $(CFLAGS)

LIBS :=		-lportaudio $(LIBS)

OBJS :=		SDREmulatorApp.o SDREmulatorFrame.o SDREmulatorReader.o SDREmulatorWriter.o DataControl.o SoundCardDialog.o

SDREmulator:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDREmulator $(OBJS) ../Common/Common.a $(LIBS)

SDREmulatorApp.o:	SDREmulatorApp.cpp SDREmulatorApp.h SDREmulatorFrame.h ../Common/Log.h ../Common/Version.h
		$(CC) $(CFLAGS) -c SDREmulatorApp.cpp

SDREmulatorFrame.o:	SDREmulatorFrame.cpp SDREmulatorFrame.h DataControl.h ../Common/Frequency.h SoundCardDialog.h \
		../Common/SoundFileReader.h
		$(CC) $(CFLAGS) -c SDREmulatorFrame.cpp

SDREmulatorReader.o:	SDREmulatorReader.cpp SDREmulatorReader.h ../Common/DataReader.h ../Common/RingBuffer.h
		$(CC) $(CFLAGS) -c SDREmulatorReader.cpp

SDREmulatorWriter.o:	SDREmulatorWriter.cpp SDREmulatorWriter.h ../Common/DataWriter.h ../Common/RingBuffer.h
		$(CC) $(CFLAGS) -c SDREmulatorWriter.cpp

DataControl.o:	DataControl.cpp DataControl.h ../Common/DataReader.h ../Common/DataWriter.h ../Common/NullWriter.h \
		../Common/SignalReader.h ../Common/RingBuffer.h SDREmulatorReader.h SDREmulatorWriter.h \
		../Common/SoundCardReader.h ../Common/SoundCardReaderWriter.h ../Common/SoundCardWriter.h \
		../Common/SoundFileReader.h
		$(CC) $(CFLAGS) -c DataControl.cpp

SoundCardDialog.o:	SoundCardDialog.cpp SoundCardDialog.h ../Common/SoundCardInfo.h
		$(CC) $(CFLAGS) -c SoundCardDialog.cpp

install:	SDREmulator
		install -g bin -o root -m 0775 SDREmulator $(BINDIR)

clean:
		rm -f core SDREmulator *.o *~ *.bak
