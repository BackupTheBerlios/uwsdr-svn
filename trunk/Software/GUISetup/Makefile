all:		GUISetup

CFLAGS := -I../UWSDRGUI $(CFLAGS)
LIBS   := -lportaudio $(LIBS)

OBJS :=		GUISetupApp.o GUISetupFrame.o

GUISetup:	$(OBJS)
		$(CC) $(LDFLAGS) -o GUISetup $(OBJS) $(LIBS)

GUISetupApp.o:	GUISetupApp.cpp GUISetupApp.h GUISetupFrame.h GUISetup.xpm
		$(CC) $(CFLAGS) -c GUISetupApp.cpp

GUISetupFrame.o: GUISetupFrame.cpp GUISetupFrame.h
		$(CC) $(CFLAGS) -c GUISetupFrame.cpp

clean:
		rm -f core GUISetup *.o *~ *.bak
