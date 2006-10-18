all:		GUISetup

CFLAGS :=	-I../UWSDRGUI -I../Common $(CFLAGS)
LIBS   :=	-lportaudio $(LIBS)

OBJS :=		GUISetupApp.o GUISetupFrame.o

GUISetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o GUISetup $(OBJS) ../Common/Common.a $(LIBS)

GUISetupApp.o:	GUISetupApp.cpp GUISetupApp.h GUISetupFrame.h GUISetupXpm.h
		$(CC) $(CFLAGS) -c GUISetupApp.cpp

GUISetupFrame.o: GUISetupFrame.cpp GUISetupFrame.h ../Common/SoundCardInfo.h
		$(CC) $(CFLAGS) -c GUISetupFrame.cpp

install:	GUISetup
		install -g bin -o root -m 0775 GUISetup $(BINDIR)

clean:
		rm -f core GUISetup *.o *~ *.bak
