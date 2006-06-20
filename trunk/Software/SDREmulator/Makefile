all:		SDREmulator

CFLAGS :=	-I../Common $(CFLAGS)

OBJS :=		SDREmulatorApp.o SDREmulatorFrame.o

SDREmulator:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDREmulator $(OBJS) ../Common/Common.a $(LIBS)

SDREmulatorApp.o:	SDREmulatorApp.cpp SDREmulatorApp.h SDREmulatorFrame.h
		$(CC) $(CFLAGS) -c SDREmulatorApp.cpp

SDREmulatorFrame.o:	SDREmulatorFrame.cpp SDREmulatorFrame.h ../Common/Frequency.h
		$(CC) $(CFLAGS) -c SDREmulatorFrame.cpp

install:	SDREmulator
		install -g bin -o root -m 0775 SDREmulator $(BINDIR)

clean:
		rm -f core SDREmulator *.o *~ *.bak
