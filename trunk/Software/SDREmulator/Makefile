all:		SDREmulator

CFLAGS :=	-I../Common $(CFLAGS)

LIBS :=		$(PALIBS) $(JACKLIBS) $(LIBS)

OBJS :=		SDREmulatorApp.o SDREmulatorFrame.o DataControl.o SoundCardDialog.o

SDREmulator:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDREmulator $(OBJS) ../Common/Common.a $(LIBS)

SDREmulatorApp.o:	SDREmulatorApp.cpp SDREmulatorApp.h SDREmulatorFrame.h ../Common/Log.h
		$(CC) $(CFLAGS) -c SDREmulatorApp.cpp

SDREmulatorFrame.o:	SDREmulatorFrame.cpp SDREmulatorFrame.h DataControl.h ../Common/Frequency.h SoundCardDialog.h \
		../Common/SoundFileReader.h ../Common/RawDataCallback.h
		$(CC) $(CFLAGS) -c SDREmulatorFrame.cpp

DataControl.o:	DataControl.cpp DataControl.h ../Common/DataReader.h ../Common/DataWriter.h ../Common/NullWriter.h \
		../Common/SignalReader.h ../Common/RingBuffer.h ../Common/UDPDataReader.h ../Common/UDPDataWriter.h \
		../Common/SoundCardReaderWriter.h ../Common/SoundFileReader.h ../Common/RawDataCallback.h
		$(CC) $(CFLAGS) -c DataControl.cpp

SoundCardDialog.o:	SoundCardDialog.cpp SoundCardDialog.h ../Common/AudioDevInfo.h
		$(CC) $(CFLAGS) -c SoundCardDialog.cpp

install:	SDREmulator
		install -g bin -o root -m 0775 SDREmulator $(BINDIR)

clean:
		rm -f core SDREmulator *.o *~ *.bak
