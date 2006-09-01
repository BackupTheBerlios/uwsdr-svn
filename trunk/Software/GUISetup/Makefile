all:		GUISetup

CFLAGS := -I../UWSDRGUI $(CFLAGS)
LIBS   := -lportaudio $(LIBS)

OBJS :=		GUISetupApp.o GUISetupFrame.o

GUISetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o GUISetup $(OBJS) $(LIBS)

GUISetupApp.o:	GUISetupApp.cpp GUISetupApp.h GUISetupFrame.h GUISetupXpm.h
		$(CC) $(CFLAGS) -c GUISetupApp.cpp

GUISetupFrame.o: GUISetupFrame.cpp GUISetupFrame.h
		$(CC) $(CFLAGS) -c GUISetupFrame.cpp

install:	GUISetup
		install -g bin -o root -m 0775 GUISetup $(BINDIR)

clean:
		rm -f core GUISetup *.o *~ *.bak
