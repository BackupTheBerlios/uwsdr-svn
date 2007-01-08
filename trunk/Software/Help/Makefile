ZIP = /usr/bin/zip

install:
	rm -f GUISetup/GUISetup.zip
	rm -f SDRSetup/SDRSetup.zip
	rm -f UWSDR/UWSDR.zip
	$(ZIP) -j -9 GUISetup/GUISetup.zip GUISetup/*.htm GUISetup/*.png GUISetup/*.hh[ckp]
	$(ZIP) -j -9 SDRSetup/SDRSetup.zip SDRSetup/*.htm SDRSetup/*.png SDRSetup/*.hh[ckp]
	$(ZIP) -j -9 UWSDR/UWSDR.zip UWSDR/*.htm UWSDR/*.png UWSDR/*.hh[ckp]
	install -g bin -o root -m 0755 -d $(DATADIR)
	install -g bin -o root -m 0664 GUISetup/GUISetup.zip $(DATADIR)
	install -g bin -o root -m 0664 SDRSetup/SDRSetup.zip $(DATADIR)
	install -g bin -o root -m 0664 UWSDR/UWSDR.zip       $(DATADIR)

clean:
	rm -f GUISetup/GUISetup.zip
	rm -f SDRSetup/SDRSetup.zip
	rm -f UWSDR/UWSDR.zip
