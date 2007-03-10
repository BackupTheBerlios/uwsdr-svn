all:		GUISetup

CFLAGS :=	-I../UWSDRGUI -I../Common $(CFLAGS)
LIBS   :=	$(PALIBS) $(JACKLIBS) $(LIBS)

OBJS :=		GUISetupApp.o GUISetupFrame.o EthernetDialog.o PortDialog.o SoundCardDialog.o

GUISetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o GUISetup $(OBJS) ../Common/Common.a $(LIBS)

GUISetupApp.o:	GUISetupApp.cpp GUISetupApp.h GUISetupFrame.h GUISetup.xpm
		$(CC) $(CFLAGS) -c GUISetupApp.cpp

GUISetupFrame.o: GUISetupFrame.cpp GUISetupFrame.h EthernetDialog.h PortDialog.h SoundCardDialog.h ../Common/AudioDevInfo.h \
		../Common/SDRDescrFile.h
		$(CC) $(CFLAGS) -c GUISetupFrame.cpp

EthernetDialog.o:	EthernetDialog.cpp EthernetDialog.h
		$(CC) $(CFLAGS) -c EthernetDialog.cpp

PortDialog.o:	PortDialog.cpp PortDialog.h ../Common/SerialControl.h ../Common/Common.h
		$(CC) $(CFLAGS) -c PortDialog.cpp

SoundCardDialog.o:	SoundCardDialog.cpp SoundCardDialog.h ../Common/AudioDevInfo.h
		$(CC) $(CFLAGS) -c SoundCardDialog.cpp

install:	GUISetup
		install -g bin -o root -m 0775 GUISetup $(BINDIR)

clean:
		rm -f core GUISetup *.o *~ *.bak
