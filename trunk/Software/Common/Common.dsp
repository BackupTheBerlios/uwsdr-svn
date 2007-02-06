# Microsoft Developer Studio Project File - Name="Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Common___Win32_Release"
# PROP BASE Intermediate_Dir "Common___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /Ob2 /I "C:\wxWidgets-2.8.0\lib\vc_dll\msw" /I "C:\wxWidgets-2.8.0\include" /I "..\PA19" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "__WXMSW__" /D "WXUSINGDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Common___Win32_Debug"
# PROP BASE Intermediate_Dir "Common___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /ZI /Od /I "C:\wxWidgets-2.8.0\lib\vc_dll\mswd" /I "C:\wxWidgets-2.8.0\include" /I "..\PA19" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /FR /FD /GZ /c
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

# Name "Common - Win32 Release"
# Name "Common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Frequency.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\NullController.cpp
# End Source File
# Begin Source File

SOURCE=.\NullReader.cpp
# End Source File
# Begin Source File

SOURCE=.\NullWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\RingBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\SDRDataReader.cpp
# End Source File
# Begin Source File

SOURCE=.\SDRDataWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\SDRDescrFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SerialControl.cpp
# End Source File
# Begin Source File

SOURCE=.\SignalReader.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundCardInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundCardReader.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundCardReaderWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundCardWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundFileReader.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundFileWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\SRTXRXController.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadReader.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreeToneReader.cpp
# End Source File
# Begin Source File

SOURCE=.\TwoToneReader.cpp
# End Source File
# Begin Source File

SOURCE=.\UWSDRController.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ControlInterface.h
# End Source File
# Begin Source File

SOURCE=.\DataCallback.h
# End Source File
# Begin Source File

SOURCE=.\DataReader.h
# End Source File
# Begin Source File

SOURCE=.\DataWriter.h
# End Source File
# Begin Source File

SOURCE=.\Frequency.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\NullController.h
# End Source File
# Begin Source File

SOURCE=.\NullReader.h
# End Source File
# Begin Source File

SOURCE=.\NullWriter.h
# End Source File
# Begin Source File

SOURCE=.\RingBuffer.h
# End Source File
# Begin Source File

SOURCE=.\SDRController.h
# End Source File
# Begin Source File

SOURCE=.\SDRDataReader.h
# End Source File
# Begin Source File

SOURCE=.\SDRDataWriter.h
# End Source File
# Begin Source File

SOURCE=.\SDRDescrFile.h
# End Source File
# Begin Source File

SOURCE=.\SerialControl.h
# End Source File
# Begin Source File

SOURCE=.\SignalReader.h
# End Source File
# Begin Source File

SOURCE=.\SoundCardInfo.h
# End Source File
# Begin Source File

SOURCE=.\SoundCardReader.h
# End Source File
# Begin Source File

SOURCE=.\SoundCardReaderWriter.h
# End Source File
# Begin Source File

SOURCE=.\SoundCardWriter.h
# End Source File
# Begin Source File

SOURCE=.\SoundFileReader.h
# End Source File
# Begin Source File

SOURCE=.\SoundFileWriter.h
# End Source File
# Begin Source File

SOURCE=.\SRTXRXController.h
# End Source File
# Begin Source File

SOURCE=.\ThreadReader.h
# End Source File
# Begin Source File

SOURCE=.\ThreeToneReader.h
# End Source File
# Begin Source File

SOURCE=.\TwoToneReader.h
# End Source File
# Begin Source File

SOURCE=.\UWSDRController.h
# End Source File
# Begin Source File

SOURCE=.\Version.h
# End Source File
# End Group
# End Target
# End Project
