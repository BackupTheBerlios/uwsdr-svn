all:		SDREmulator

CFLAGS :=	-I../UWSDRGUI $(CFLAGS)

LDFLAGS :=	-L../fftw $(LDFLAGS)

OBJS :=	SDREmulatorApp.o SDREmulatorFrame.o

SDREmulator:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDREmulator $(OBJS) $(LIBS)

SDREmulatorApp.o:	SDREmulatorApp.cpp SDREmulatorApp.h SDREmulatorFrame.h
		$(CC) $(CFLAGS) -c SDREmulatorApp.cpp

SDREmulatorFrame.o:	SDREmulatorFrame.cpp SDREmulatorFrame.h
		$(CC) $(CFLAGS) -c SDREmulatorFrame.cpp

clean:
		rm -f core *.o *~ *.bak
