RawDataCallall:		SDRSetup

CFLAGS :=	-I../Common $(CFLAGS)

OBJS :=		SDRSetupApp.o SDRSetupFrame.o

SDRSetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o SDRSetup $(OBJS) ../Common/Common.a $(LIBS)

SDRSetupApp.o:	SDRSetupApp.cpp SDRSetupApp.h SDRSetupFrame.h
		$(CC) $(CFLAGS) -c SDRSetupApp.cpp

SDRSetupFrame.o: SDRSetupFrame.cpp SDRSetupFrame.h SDRSetup.xpm ../Common/UDPDataReader.h ../Common/UDPDataWriter.h \
		../Common/SocketCallback.h
		$(CC) $(CFLAGS) -c SDRSetupFrame.cpp

install:	SDRSetup
		install -g bin -o root -m 0775 SDRSetup $(BINDIR)

clean:
		rm -f core SDRSetup *.o *~ *.bak
