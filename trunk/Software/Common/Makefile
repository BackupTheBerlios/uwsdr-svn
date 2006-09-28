all:		Common.a

OBJS := 	Frequency.o NullReader.o NullWriter.o RingBuffer.o SDRData.o SignalReader.o SoundCardReader.o \
		SoundCardWriter.o SoundFileReader.o SoundFileWriter.o

Common.a:	$(OBJS)
		ar rcs Common.a $(OBJS)

Frequency.o:	Frequency.cpp Frequency.h
		$(CC) $(CFLAGS) -c Frequency.cpp

NullReader.o:	NullReader.cpp NullReader.h DataReader.h
		$(CC) $(CFLAGS) -c NullReader.cpp

NullWriter.o:	NullWriter.cpp NullWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c NullWriter.cpp

RingBuffer.o:	RingBuffer.cpp RingBuffer.h
		$(CC) $(CFLAGS) -c RingBuffer.cpp

SDRData.o:	SDRData.cpp SDRData.h DataReader.h DataWriter.h RingBuffer.h
		$(CC) $(CFLAGS) -c SDRData.cpp

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

clean:
		rm -f core Common.a *.o *~ *.bak
