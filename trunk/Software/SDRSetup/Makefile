all:		SDRSetup

OBJS :=		SDRSetupApp.o SDRSetupFrame.o Frequency.o

SDRSetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDRSetup $(OBJS) $(LIBS)

SDRSetupApp.o:	SDRSetupApp.cpp SDRSetupApp.h SDRSetupFrame.h
		$(CC) $(CFLAGS) -c SDRSetupApp.cpp

SDRSetupFrame.o: SDRSetupFrame.cpp SDRSetupFrame.h SDRSetupXpm.h
		$(CC) $(CFLAGS) -c SDRSetupFrame.cpp

Frequency.o:	../Common/Frequency.cpp ../Common/Frequency.h
		$(CC) $(CFLAGS) -c ../Common/Frequency.cpp

install:	SDRSetup
		install -g bin -o root -m 0775 SDRSetup $(BINDIR)

clean:
		rm -f core SDRSetup *.o *~ *.bak
