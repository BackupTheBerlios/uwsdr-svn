# Microsoft Developer Studio Project File - Name="UWSDR GUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=UWSDR GUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UWSDRGUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UWSDRGUI.mak" CFG="UWSDR GUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UWSDR GUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "UWSDR GUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UWSDR GUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /Ob0 /I "C:\wxWidgets-2.6.3\lib\vc_dll\msw" /I "." /I "C:\wxWidgets-2.6.3\include" /I "..\PA19" /D "WIN32" /D "__WXMSW__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /FD /c
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
# ADD LINK32 wxbase26.lib wxmsw26_core.lib wxbase26_net.lib DttSP.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib fftw3.lib /nologo /subsystem:windows /machine:I386 /out:"../Release/UWSDR.exe" /libpath:"..\DttSP++\Release" /libpath:"C:\wxWidgets-2.6.3\lib\vc_dll" /libpath:"..\FFTW" /libpath:"..\PA19"

!ELSEIF  "$(CFG)" == "UWSDR GUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "C:\wxWidgets-2.6.3\lib\vc_dll\mswd" /I "." /I "C:\wxWidgets-2.6.3\include" /I "..\PA19" /D "WIN32" /D "_DEBUG" /D "__WXMSW__" /D "__WXDEBUG__" /D "WXUSINGDLL" /D "_WINDOWS" /D "NOPCH" /FR /FD /GZ /c
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
# ADD LINK32 wxbase26d.lib wxmsw26d_core.lib wxbase26d_net.lib dttsp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib fftw3.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../Debug/UWSDR.exe" /pdbtype:sept /libpath:"..\DttSP++\Debug" /libpath:"C:\wxWidgets-2.6.3\lib\vc_dll" /libpath:"..\FFTW" /libpath:"..\PA19"

!ENDIF 

# Begin Target

# Name "UWSDR GUI - Win32 Release"
# Name "UWSDR GUI - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DSPControl.cpp
# End Source File
# Begin Source File

SOURCE=.\DTTSPControl.cpp
# End Source File
# Begin Source File

SOURCE=.\FreqDial.cpp
# End Source File
# Begin Source File

SOURCE=.\FreqDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\FreqKeypad.cpp
# End Source File
# Begin Source File

SOURCE=.\Frequency.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
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

SOURCE=.\SDRDescrFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SDRParameters.cpp
# End Source File
# Begin Source File

SOURCE=.\SignalReader.cpp
# End Source File
# Begin Source File

SOURCE=.\SMeter.cpp
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

SOURCE=.\SpectrumDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\UWSDRApp.cpp
# End Source File
# Begin Source File

SOURCE=.\UWSDRControl.cpp
# End Source File
# Begin Source File

SOURCE=.\UWSDRData.cpp
# End Source File
# Begin Source File

SOURCE=.\UWSDRFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\UWSDRPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\VolumeDial.cpp
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

SOURCE=.\DialInterface.h
# End Source File
# Begin Source File

SOURCE=.\DSPControl.h
# End Source File
# Begin Source File

SOURCE=.\DTTSPControl.h
# End Source File
# Begin Source File

SOURCE=.\DTTSPExports.h
# End Source File
# Begin Source File

SOURCE=.\FreqDial.h
# End Source File
# Begin Source File

SOURCE=.\FreqDisplay.h
# End Source File
# Begin Source File

SOURCE=.\FreqKeypad.h
# End Source File
# Begin Source File

SOURCE=.\Frequency.h
# End Source File
# Begin Source File

SOURCE=.\InfoBox.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
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

SOURCE=.\SDRControl.h
# End Source File
# Begin Source File

SOURCE=.\SDRDescrFile.h
# End Source File
# Begin Source File

SOURCE=.\SDRParameters.h
# End Source File
# Begin Source File

SOURCE=.\SignalReader.h
# End Source File
# Begin Source File

SOURCE=.\SMeter.h
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

SOURCE=.\SpectrumDisplay.h
# End Source File
# Begin Source File

SOURCE=.\UWSDRApp.h
# End Source File
# Begin Source File

SOURCE=.\UWSDRControl.h
# End Source File
# Begin Source File

SOURCE=.\UWSDRData.h
# End Source File
# Begin Source File

SOURCE=.\UWSDRDefs.h
# End Source File
# Begin Source File

SOURCE=.\UWSDRFrame.h
# End Source File
# Begin Source File

SOURCE=.\UWSDRPreferences.h
# End Source File
# Begin Source File

SOURCE=.\VolumeDial.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
