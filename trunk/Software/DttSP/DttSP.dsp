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
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "..\fftw" /I "C:\wxMSW-2.7.2\lib\vc_dll\msw" /I "C:\wxMSW-2.7.2\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "DttSP - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W4 /Gm /GR /GX /ZI /Od /I "." /I "..\fftw" /I "C:\wxMSW-2.7.2\lib\vc_dll\mswd" /I "C:\wxMSW-2.7.2\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /FR /FD /GZ  /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
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

SOURCE=.\am_demod.cpp
# End Source File
# Begin Source File

SOURCE=.\banal.cpp
# End Source File
# Begin Source File

SOURCE=.\bufvec.cpp
# End Source File
# Begin Source File

SOURCE=.\correctIQ.cpp
# End Source File
# Begin Source File

SOURCE=.\cxops.cpp
# End Source File
# Begin Source File

SOURCE=.\dcblock.cpp
# End Source File
# Begin Source File

SOURCE=.\dttspagc.cpp
# End Source File
# Begin Source File

SOURCE=.\fastrig.cpp
# End Source File
# Begin Source File

SOURCE=.\filter.cpp
# End Source File
# Begin Source File

SOURCE=.\fm_demod.cpp
# End Source File
# Begin Source File

SOURCE=.\graphiceq.cpp
# End Source File
# Begin Source File

SOURCE=.\lmadf.cpp
# End Source File
# Begin Source File

SOURCE=.\meter.cpp
# End Source File
# Begin Source File

SOURCE=.\noiseblanker.cpp
# End Source File
# Begin Source File

SOURCE=.\oscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\ovsv.cpp
# End Source File
# Begin Source File

SOURCE=.\resample.cpp
# End Source File
# Begin Source File

SOURCE=.\resampleF.cpp
# End Source File
# Begin Source File

SOURCE=.\ringb.cpp
# End Source File
# Begin Source File

SOURCE=.\sdr.cpp
# End Source File
# Begin Source File

SOURCE=.\sdrexport.cpp
# End Source File
# Begin Source File

SOURCE=.\spectrum.cpp
# End Source File
# Begin Source File

SOURCE=.\speechproc.cpp
# End Source File
# Begin Source File

SOURCE=.\splitfields.cpp
# End Source File
# Begin Source File

SOURCE=.\spottone.cpp
# End Source File
# Begin Source File

SOURCE=.\thunk.cpp
# End Source File
# Begin Source File

SOURCE=.\update.cpp
# End Source File
# Begin Source File

SOURCE=.\window.cpp
# End Source File
# Begin Source File

SOURCE=.\winmain.cpp
# End Source File
# Begin Source File

SOURCE=.\wscompand.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\am_demod.h
# End Source File
# Begin Source File

SOURCE=.\banal.h
# End Source File
# Begin Source File

SOURCE=.\bufvec.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\complex.h
# End Source File
# Begin Source File

SOURCE=.\correctIQ.h
# End Source File
# Begin Source File

SOURCE=.\cxops.h
# End Source File
# Begin Source File

SOURCE=.\datatypes.h
# End Source File
# Begin Source File

SOURCE=.\dcblock.h
# End Source File
# Begin Source File

SOURCE=.\defs.h
# End Source File
# Begin Source File

SOURCE=.\dttspagc.h
# End Source File
# Begin Source File

SOURCE=.\fastrig.h
# End Source File
# Begin Source File

SOURCE=.\filter.h
# End Source File
# Begin Source File

SOURCE=.\fm_demod.h
# End Source File
# Begin Source File

SOURCE=.\fromsys.h
# End Source File
# Begin Source File

SOURCE=.\graphiceq.h
# End Source File
# Begin Source File

SOURCE=.\iosdr.h
# End Source File
# Begin Source File

SOURCE=.\lmadf.h
# End Source File
# Begin Source File

SOURCE=.\local.h
# End Source File
# Begin Source File

SOURCE=.\meter.h
# End Source File
# Begin Source File

SOURCE=.\noiseblanker.h
# End Source File
# Begin Source File

SOURCE=.\oscillator.h
# End Source File
# Begin Source File

SOURCE=.\ovsv.h
# End Source File
# Begin Source File

SOURCE=.\resample.h
# End Source File
# Begin Source File

SOURCE=.\resampleF.h
# End Source File
# Begin Source File

SOURCE=.\ringb.h
# End Source File
# Begin Source File

SOURCE=.\sdrexport.h
# End Source File
# Begin Source File

SOURCE=.\spectrum.h
# End Source File
# Begin Source File

SOURCE=.\speechproc.h
# End Source File
# Begin Source File

SOURCE=.\splitfields.h
# End Source File
# Begin Source File

SOURCE=.\spottone.h
# End Source File
# Begin Source File

SOURCE=.\thunk.h
# End Source File
# Begin Source File

SOURCE=.\update.h
# End Source File
# Begin Source File

SOURCE=.\window.h
# End Source File
# Begin Source File

SOURCE=.\wscompand.h
# End Source File
# End Group
# End Target
# End Project
