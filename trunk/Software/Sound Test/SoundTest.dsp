# Microsoft Developer Studio Project File - Name="Sound Test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Sound Test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Sound Test.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Sound Test.mak" CFG="Sound Test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Sound Test - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Sound Test - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Sound Test - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\wxWidgets-2.6.3\lib\wxmsw\wx_dll" /I "C:\wxWidgets-2.6.3\include" /I "..\PortAudio\portaudio\pa_common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__WXMSW__" /D "WXUSINGDLL" /D "NOPCH" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Sound Test - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "C:\wxWidgets-2.6.3\lib\vc_dll\mswd" /I "C:\wxWidgets-2.6.3\include" /I "..\PortAudio\portaudio\pa_common" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__WXMSW__" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "NOPCH" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wxbase26d.lib wxmsw26d_core.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"C:\wxWidgets-2.6.3\lib\vc_dll" /libpath:"..\DttSP\Debug"

!ENDIF 

# Begin Target

# Name "Sound Test - Win32 Release"
# Name "Sound Test - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\NullReader.cpp
# End Source File
# Begin Source File

SOURCE=.\NullWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\SoundCardReader.cpp
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

SOURCE=.\SoundTestApp.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DataCallbackInterface.h
# End Source File
# Begin Source File

SOURCE=.\DataReaderInterface.h
# End Source File
# Begin Source File

SOURCE=.\DataWriterInterface.h
# End Source File
# Begin Source File

SOURCE=.\NullReader.h
# End Source File
# Begin Source File

SOURCE=.\NullWriter.h
# End Source File
# Begin Source File

SOURCE=.\PAExports.h
# End Source File
# Begin Source File

SOURCE=.\SoundCardReader.h
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

SOURCE=.\SoundTestApp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
