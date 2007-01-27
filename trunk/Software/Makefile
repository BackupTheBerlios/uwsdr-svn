export BINDIR  := "/usr/bin"
export DATADIR := "/usr/share/uwsdr"
export DESKTOPDIR := "/usr/share/applications"
export PNGDIR := "/usr/share/pixmaps"

export CC := $(shell wx-config --cxx)

export LIBS := $(shell wx-config --libs adv,core,html,net)

export CFLAGS := -O2 -Wall -Wno-non-virtual-dtor -Wno-strict-aliasing -march=pentium -DDATA_DIR='$(DATADIR)' -DBIN_DIR='$(BINDIR)' $(shell wx-config --cxxflags)

all:		GUISetup/GUISetup SDREmulator/SDREmulator SDRHelp/SDRHelp SDRSetup/SDRSetup UWSDRGUI/UWSDR

GUISetup/GUISetup:	Common/Common.a
		make -C GUISetup

SDREmulator/SDREmulator: Common/Common.a
		make -C SDREmulator

SDRHelp/SDRHelp:
		make -C SDRHelp

SDRSetup/SDRSetup:
		make -C SDRSetup

UWSDRGUI/UWSDR:	Common/Common.a DttSP++/DttSP.a
		make -C UWSDRGUI

DttSP++/DttSP.a:	Common/Common.a
		make -C DttSP++

Common/Common.a:
		make -C Common

install:
		make -C GUISetup install
		make -C SDREmulator install
		make -C SDRFiles install
		make -C SDRHelp install
		make -C SDRSetup install
		make -C UWSDRGUI install
		make -C Help install
		make -C meta install

clean:
		rm -f core *~ *.bak
		make -C Common clean
		make -C DttSP++ clean
		make -C GUISetup clean
		make -C SDREmulator clean
		make -C SDRHelp clean
		make -C SDRSetup clean
		make -C UWSDRGUI clean
		make -C Help clean
