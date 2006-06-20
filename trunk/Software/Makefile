export BINDIR  := /usr/local/bin
export DATADIR := /usr/local/share/uwsdr

export CC := $(shell wx-config --cxx)

# export LDFLAGS := $(shell wx-config --ldflags)

export LIBS := $(shell wx-config --libs)

export CFLAGS := -O2 -Wall -march=pentium -DDATA_DIR=$(DATADIR) -DBIN_DIR=$(BINDIR) $(shell wx-config --cxxflags)

all:		GUISetup/GUISetup SDREmulator/SDREmulator SDRSetup/SDRSetup UWSDRGUI/UWSDR

GUISetup/GUISetup:
		make -C GUISetup

SDREmulator/SDREmulator: Common/Common.a
		make -C SDREmulator

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
		make -C SDRSetup install
		make -C UWSDRGUI install
		make -C meta install

clean:
		rm -f core *~ *.bak
		make -C Common clean
		make -C DttSP++ clean
		make -C GUISetup clean
		make -C SDREmulator clean
		make -C SDRSetup clean
		make -C UWSDRGUI clean
