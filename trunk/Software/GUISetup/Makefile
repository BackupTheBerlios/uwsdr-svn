all:		GUISetup

CFLAGS :=	-I../UWSDRGUI -I../Common $(CFLAGS)
LIBS   :=	$(PALIBS) $(JACKLIBS) $(LIBS)

OBJS :=		GUISetupApp.o GUISetupFrame.o EthernetDialog.o ExternalDialog.o PortDialog.o SoundCardDialog.o TuningDialog.o

GUISetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o GUISetup $(OBJS) ../Common/Common.a $(LIBS)

GUISetupApp.o:	GUISetupApp.cpp GUISetupApp.h GUISetupFrame.h GUISetup.xpm
		$(CC) $(CFLAGS) -c GUISetupApp.cpp

GUISetupFrame.o: GUISetupFrame.cpp GUISetupFrame.h EthernetDialog.h PortDialog.h SoundCardDialog.h ../Common/AudioDevInfo.h \
		../Common/SDRDescrFile.h TuningDialog.h ExternalDialog.h
		$(CC) $(CFLAGS) -c GUISetupFrame.cpp

EthernetDialog.o:	EthernetDialog.cpp EthernetDialog.h
		$(CC) $(CFLAGS) -c EthernetDialog.cpp

ExternalDialog.o:	ExternalDialog.cpp ExternalDialog.h ../Common/Common.h
		$(CC) $(CFLAGS) -c ExternalDialog.cpp

PortDialog.o:	PortDialog.cpp PortDialog.h ../Common/SerialControl.h ../Common/Common.h
		$(CC) $(CFLAGS) -c PortDialog.cpp

SoundCardDialog.o:	SoundCardDialog.cpp SoundCardDialog.h ../Common/AudioDevInfo.h
		$(CC) $(CFLAGS) -c SoundCardDialog.cpp

TuningDialog.o:	TuningDialog.cpp TuningDialog.h ../Common/Common.h
		$(CC) $(CFLAGS) -c TuningDialog.cpp

install:	GUISetup
		install -g bin -o root -m 0775 GUISetup $(BINDIR)

clean:
		rm -f core GUISetup *.o *~ *.bak
