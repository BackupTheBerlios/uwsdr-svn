all:		SDRSetup

OBJS :=		SDRSetupApp.o SDRSetupFrame.o Frequency.o

SDRSetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDRSetup $(OBJS) $(LIBS)

SDRSetupApp.o:	SDRSetupApp.cpp SDRSetupApp.h SDRSetupFrame.h SDRSetup.xpm
		$(CC) $(CFLAGS) -c SDRSetupApp.cpp

SDRSetupFrame.o: SDRSetupFrame.cpp SDRSetupFrame.h
		$(CC) $(CFLAGS) -c SDRSetupFrame.cpp

Frequency.o:	../UWSDRGUI/Frequency.cpp ../UWSDRGUI/Frequency.h
		$(CC) $(CFLAGS) -c ../UWSDRGUI/Frequency.cpp

clean:
		rm -f core SDRSetup *.o *~ *.bak
