DATADIR = "/usr/local/share/uwsdr"

ZIP = /usr/bin/zip

all:	GUISetup/GUISetup.zip SDRSetup/SDRSetup.zip UWSDR/UWSDR.zip

GUISetup/GUISetup.zip:
	$(ZIP) -9 GUISetup/GUISetup.zip GUISetup/*.htm GUISetup/*.png GUISetup/*.hh[ckp]

SDRSetup/SDRSetup.zip:
	$(ZIP) -9 SDRSetup/SDRSetup.zip SDRSetup/*.htm SDRSetup/*.png SDRSetup/*.hh[ckp]

UWSDR/UWSDR.zip:
	$(ZIP) -9 UWSDR/UWSDR.zip UWSDR/*.htm UWSDR/*.png UWSDR/*.hh[ckp]

install: GUISetup/GUISetup.zip SDRSetup/SDRSetup.zip UWSDR/UWSDR.zip
	install -g bin -o root -m 0775 GUISetup/GUISetup.zip $(DATADIR)
	install -g bin -o root -m 0775 SDRSetup/SDRSetup.zip $(DATADIR)
	install -g bin -o root -m 0775 UWSDR/UWSDR.zip       $(DATADIR)

clean:
	rm -f GUISetup/GUISetup.zip
	rm -f SDRSetup/SDRSetup.zip
	rm -f UWSDR/UWSDR.zip
