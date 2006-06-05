all:		SDRSetup

OBJS :=	SDRSetupApp.o SDRSetupFrame.o

SDRSetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDRSetup $(OBJS) $(LIBS)

SDRSetupApp.o:	SDRSetupApp.cpp SDRSetupApp.h SDRSetupFrame.h SDRSetup.xpm
		$(CC) $(CFLAGS) -c SDRSetupApp.cpp

SDRSetupFrame.o:	SDRSetupFrame.cpp SDRSetupFrame.h
		$(CC) $(CFLAGS) -c SDRSetupFrame.cpp

clean:
		rm -f core *.o *~ *.bak
