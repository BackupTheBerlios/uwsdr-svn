BINDIR=/usr/local/bin

export CC := $(shell wx-config --cxx)

# export LDFLAGS := $(shell wx-config --ldflags)

export LIBS := $(shell wx-config --libs)

export CFLAGS := -O2 -Wall -march=pentium $(shell wx-config --cxxflags)

export BINDIR  := /usr/local/bin
export DATADIR := /usr/local/share/uwsdr

all:		GUISetup/GUISetup SDREmulator/SDREmulator SDRSetup/SDRSetup UWSDRGUI/UWSDR

GUISetup/GUISetup:
		make -C GUISetup

SDREmulator/SDREmulator:
		make -C SDREmulator

SDRSetup/SDRSetup:
		make -C SDRSetup

UWSDRGUI/UWSDR:	DttSP++/DttSP.a
		make -C UWSDRGUI

DttSP++/DttSP.a:
		make -C DttSP++

install:
		make -C GUISetup install
		make -C SDREmulator install
		make -C SDRSetup install
		make -C UWSDRGUI install
		make -C meta install

clean:
		rm -f core *~ *.bak
		make -C DttSP++ clean
		make -C GUISetup clean
		make -C SDREmulator clean
		make -C SDRSetup clean
		make -C UWSDRGUI clean
