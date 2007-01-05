This is a selection of .sdr files that cover all three types of hardware supported by the GUI.

The Demo.sdr file is as its name suggests, used for demonstration purposes, both the transmit and receive sides work and all of the modes and filtering are operational. The step size is large so that the interpolation oscillator is used and so the sample signal may be tuned, but using third-method/zero-IF is not usable.

The SoftRock .sdr files should be usable with the SoftRock receivers. All of the tuning is done in the GUI and the sample rate is set to 96 kHz to allow for a reasonable tuning range. With a good sound card even higher sample rates could be used. For this sort of hardware, the transmit side of the GUI is disabled.

The 13cms and 23cms .sdr files will be the basis of the real hardware description files. They require the SDR Emulator to be running on another PC to be usable.
