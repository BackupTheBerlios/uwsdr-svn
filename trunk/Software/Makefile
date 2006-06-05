BINDIR=/usr/local/bin

export CC := $(shell wx-config --cxx)

# export LDFLAGS := $(shell wx-config --ldflags)

export LIBS := $(shell wx-config --libs)

export CFLAGS := -O2 -Wall -march=pentium $(shell wx-config --cxxflags)

all:		GUISetup/GUISetup.exe SDREmulator/SDREmulator.exe SDRSetup/SDRSetup.exe UWSDRGUI/UWSDR.exe

GUISetup/GUISetup.exe:
		make -C GUISetup

SDREmulator/SDREmulator.exe:
		make -C SDREmulator

SDRSetup/SDRSetup.exe:
		make -C SDRSetup

UWSDRGUI/UWSDR.exe:	DttSP++/DttSP.a
		make -C UWSDRGUI

DttSP++/DttSP.a:
		make -C DttSP++

clean:
		rm -f core *~ *.bak
		make -C DttSP++ GUISetup SDREmulator SDRSetup UWSDRGUI clean
