BINDIR=/usr/local/bin

export CC := $(shell wx-config --cxx)

#
# Set the version of FFTW that you have, here.
#
export FFT_VERS := 3
# export FFT_VERS := 2

ifeq ($(FFT_VERS),3)
    FFT_CFLAGS := -DHAVE_FFTW3=1
    FFT_LIB := -lfftw3
endif

ifeq ($(FFT_VERS),2)
    FFT_CFLAGS := -DHAVE_FFTW2=1
    FFT_LIB := -lfftw
endif

# export LDFLAGS := $(shell wx-config --ldflags)

# export LIBS := -lsndfile $(FFT_LIB) $(shell wx-config --libs)
export LIBS := $(shell wx-config --libs)

#export CFLAGS := -O2 -Wall -march=pentium -I.. $(FFT_CFLAGS) $(shell wx-config --cxxflags)
export CFLAGS := -O2 -Wall -march=pentium -I.. $(shell wx-config --cxxflags)

all:		GUI/UWSDR.exe

GUI/UWSDR.exe:
		make -C GUI

clean:
		rm -f core *~ *.bak
		make -C GUI clean
