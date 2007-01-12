This is a selection of .sdr files that cover all three types of hardware supported by the GUI.

The Demo.sdr file is as its name suggests, used for demonstration purposes, both the transmit and receive sides work and all of the modes and filtering are operational. The step size is large so that the interpolation oscillator is used and so the sample signal may be tuned, but the third-method/zero-IF is not available.

The SoftRock .sdr files should be usable with the SoftRock receivers. All of the tuning is done in the GUI and the sample rate is set to 96 kHz to allow for a reasonable tuning range. With a good sound card even higher sample rates could be used. For this sort of hardware, the transmit side of the GUI is disabled. If you are using a SoftRock then you should look into the .sdr file and edit the frequency if it doesn't match your crystal. The lowFreq and highFreq entries should be the same.

The 13cms and 23cms .sdr files will be the basis of the real hardware description files. They require the SDR Emulator to be running on another PC to be usable.

The Skeleton.sdr is a copy of Demo.sdr with many comments included so that anyone wishing to create or edit an .sdr file can understand the meaning of the various entries.

