all:		SDREmulator

CFLAGS := -I../UWSDRGUI $(CFLAGS)

OBJS :=		SDREmulatorApp.o SDREmulatorFrame.o Frequency.o

SDREmulator:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDREmulator $(OBJS) $(LIBS)

SDREmulatorApp.o:	SDREmulatorApp.cpp SDREmulatorApp.h SDREmulatorFrame.h
		$(CC) $(CFLAGS) -c SDREmulatorApp.cpp

SDREmulatorFrame.o:	SDREmulatorFrame.cpp SDREmulatorFrame.h
		$(CC) $(CFLAGS) -c SDREmulatorFrame.cpp

Frequency.o:	../UWSDRGUI/Frequency.cpp ../UWSDRGUI/Frequency.h
		$(CC) $(CFLAGS) -c ../UWSDRGUI/Frequency.cpp

clean:
		rm -f core SDREmulator *.o *~ *.bak
