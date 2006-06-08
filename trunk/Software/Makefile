BINDIR=/usr/local/bin

export CC := $(shell wx-config --cxx)

# export LDFLAGS := $(shell wx-config --ldflags)

export LIBS := $(shell wx-config --libs)

export CFLAGS := -O2 -Wall -march=pentium $(shell wx-config --cxxflags)

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

clean:
		rm -f core *~ *.bak
		make -C DttSP++ clean
		make -C GUISetup clean
		make -C SDREmulator clean
		make -C SDRSetup clean
		make -C UWSDRGUI clean
