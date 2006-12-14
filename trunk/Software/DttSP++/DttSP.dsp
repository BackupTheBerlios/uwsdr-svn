# Microsoft Developer Studio Project File - Name="DttSP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DttSP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DttSP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DttSP.mak" CFG="DttSP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DttSP - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DttSP - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DttSP - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DttSP___Win32_Release"
# PROP BASE Intermediate_Dir "DttSP___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob0 /I "C:\wxWidgets-2.8.0\lib\vc_dll\msw" /I "..\Common" /I "..\FFTW" /I "C:\wxWidgets-2.8.0\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__WXMSW__" /D "WXUSINGDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "DttSP - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DttSP___Win32_Debug"
# PROP BASE Intermediate_Dir "DttSP___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "C:\wxWidgets-2.8.0\lib\vc_dll\mswd" /I "..\Common" /I "..\FFTW" /I "C:\wxWidgets-2.8.0\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "DttSP - Win32 Release"
# Name "DttSP - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AGC.cpp
# End Source File
# Begin Source File

SOURCE=.\AMDemod.cpp
# End Source File
# Begin Source File

SOURCE=.\AMMod.cpp
# End Source File
# Begin Source File

SOURCE=.\BlockLMS.cpp
# End Source File
# Begin Source File

SOURCE=.\CorrectIQ.cpp
# End Source File
# Begin Source File

SOURCE=.\CXB.cpp
# End Source File
# Begin Source File

SOURCE=.\DCBlock.cpp
# End Source File
# Begin Source File

SOURCE=.\DttSP.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterOVSV.cpp
# End Source File
# Begin Source File

SOURCE=.\FIR.cpp
# End Source File
# Begin Source File

SOURCE=.\FMDemod.cpp
# End Source File
# Begin Source File

SOURCE=.\FMMod.cpp
# End Source File
# Begin Source File

SOURCE=.\LMS.cpp
# End Source File
# Begin Source File

SOURCE=.\Meter.cpp
# End Source File
# Begin Source File

SOURCE=.\NoiseBlanker.cpp
# End Source File
# Begin Source File

SOURCE=.\Oscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\RX.cpp
# End Source File
# Begin Source File

SOURCE=.\Spectrum.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeechProc.cpp
# End Source File
# Begin Source File

SOURCE=.\SpotTone.cpp
# End Source File
# Begin Source File

SOURCE=.\Squelch.cpp
# End Source File
# Begin Source File

SOURCE=.\SSBDemod.cpp
# End Source File
# Begin Source File

SOURCE=.\SSBMod.cpp
# End Source File
# Begin Source File

SOURCE=.\TX.cpp
# End Source File
# Begin Source File

SOURCE=.\update.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AGC.h
# End Source File
# Begin Source File

SOURCE=.\AMDemod.h
# End Source File
# Begin Source File

SOURCE=.\AMMod.h
# End Source File
# Begin Source File

SOURCE=.\BlockLMS.h
# End Source File
# Begin Source File

SOURCE=.\Complex.h
# End Source File
# Begin Source File

SOURCE=.\CorrectIQ.h
# End Source File
# Begin Source File

SOURCE=.\CXB.h
# End Source File
# Begin Source File

SOURCE=.\CXOps.h
# End Source File
# Begin Source File

SOURCE=.\DCBlock.h
# End Source File
# Begin Source File

SOURCE=.\Defs.h
# End Source File
# Begin Source File

SOURCE=.\Demod.h
# End Source File
# Begin Source File

SOURCE=.\DttSP.h
# End Source File
# Begin Source File

SOURCE=.\FilterOVSV.h
# End Source File
# Begin Source File

SOURCE=.\FIR.h
# End Source File
# Begin Source File

SOURCE=.\FMDemod.h
# End Source File
# Begin Source File

SOURCE=.\FMMod.h
# End Source File
# Begin Source File

SOURCE=.\FromSys.h
# End Source File
# Begin Source File

SOURCE=.\LMS.h
# End Source File
# Begin Source File

SOURCE=.\Meter.h
# End Source File
# Begin Source File

SOURCE=.\Mod.h
# End Source File
# Begin Source File

SOURCE=.\NoiseBlanker.h
# End Source File
# Begin Source File

SOURCE=.\Oscillator.h
# End Source File
# Begin Source File

SOURCE=.\RX.h
# End Source File
# Begin Source File

SOURCE=.\Spectrum.h
# End Source File
# Begin Source File

SOURCE=.\SpeechProc.h
# End Source File
# Begin Source File

SOURCE=.\SpotTone.h
# End Source File
# Begin Source File

SOURCE=.\Squelch.h
# End Source File
# Begin Source File

SOURCE=.\SSBDemod.h
# End Source File
# Begin Source File

SOURCE=.\SSBMod.h
# End Source File
# Begin Source File

SOURCE=.\TX.h
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# End Group
# End Target
# End Project
