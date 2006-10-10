all:		Common.a

OBJS := 	Frequency.o Hilbert.o Log.o NullReader.o NullWriter.o RingBuffer.o SDRDataReader SDRDataWriter.o.\
		SignalReader.o SoundCardInfo.o SoundCardReader.o SoundCardWriter.o SoundFileReader.o \
		SoundFileWriter.o

Common.a:	$(OBJS)
		ar rcs Common.a $(OBJS)

Frequency.o:	Frequency.cpp Frequency.h
		$(CC) $(CFLAGS) -c Frequency.cpp

Log.o:		Log.cpp Log.h
		$(CC) $(CFLAGS) -c Log.cpp

Hilbert.o:	Hilbert.cpp Hilbert.h
		$(CC) $(CFLAGS) -c Hilbert.cpp

NullReader.o:	NullReader.cpp NullReader.h DataReader.h
		$(CC) $(CFLAGS) -c NullReader.cpp

NullWriter.o:	NullWriter.cpp NullWriter.h DataWriter.h
		$(CC) $(CFLAGS) -c NullWriter.cpp

RingBuffer.o:	RingBuffer.cpp RingBuffer.h
		$(CC) $(CFLAGS) -c RingBuffer.cpp

SDRDataReader.o: SDRDataReader.cpp SDRDataReader.h DataReader.h
		$(CC) $(CFLAGS) -c SDRDataReader.cpp

SDRDataWriter.o: SDRDataWriter.cpp SDRDataWriter.h DataWriter.h RingBuffer.h
		$(CC) $(CFLAGS) -c SDRDataWriter.cpp

SignalReader.o:	SignalReader.cpp SignalReader.h DataReader.h
		$(CC) $(CFLAGS) -c SignalReader.cpp

SoundCardInfo.o: SoundCardInfo.cpp SoundCardInfo.h
		$(CC) $(CFLAGS) -c SoundCardInfo.cpp

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
