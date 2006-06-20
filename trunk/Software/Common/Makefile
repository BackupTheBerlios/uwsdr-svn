all:		Common.a

OBJS :=	Frequency.o RingBuffer.o

Common.a:	$(OBJS)
		ar rcs Common.a $(OBJS)

Frequency.o:	Frequency.cpp Frequency.h
		$(CC) $(CFLAGS) -c Frequency.cpp

RingBuffer.o:	RingBuffer.cpp RingBuffer.h
		$(CC) $(CFLAGS) -c RingBuffer.cpp

clean:
		rm -f core Common.a *.o *~ *.bak
