# Microsoft Developer Studio Project File - Name="PA19" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PA19 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PA19.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PA19.mak" CFG="PA19 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PA19 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PA19 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PA19 - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PA19_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "asiosdk2\common" /I "asiosdk2\host" /I "asiosdk2\host\pc" /I "portaudio\pa_asio" /I "portaudio\pa_common" /I "portaudio\pa_win" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PA19_EXPORTS" /D "PA_NO_WDMKS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib setupapi.lib /nologo /dll /machine:I386 /out:"../Release/PA19.dll"

!ELSEIF  "$(CFG)" == "PA19 - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PA19_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "asiosdk2\common" /I "asiosdk2\host" /I "asiosdk2\host\pc" /I "portaudio\pa_asio" /I "portaudio\pa_common" /I "portaudio\pa_win" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PA19_EXPORTS" /D "PA_NO_WDMKS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib setupapi.lib /nologo /dll /pdb:"Debug/system32.pdb" /debug /machine:I386 /out:"../Debug/PA19.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "PA19 - Win32 Release"
# Name "PA19 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\asiosdk2\common\asio.cpp
# End Source File
# Begin Source File

SOURCE=.\asiosdk2\host\asiodrivers.cpp
# End Source File
# Begin Source File

SOURCE=.\asiosdk2\host\pc\asiolist.cpp
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_win_ds\dsound_wrapper.c
# End Source File
# Begin Source File

SOURCE=.\PA19.cpp

!IF  "$(CFG)" == "PA19 - Win32 Release"

!ELSEIF  "$(CFG)" == "PA19 - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\pa19.def
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_allocation.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_asio\pa_asio.cpp
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_converters.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_cpuload.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_dither.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_front.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_process.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_skeleton.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_stream.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_win_ds\pa_win_ds.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_win\pa_win_hostapis.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_win\pa_win_util.c
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_win_wmme\pa_win_wmme.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\portaudio\pa_win_ds\dsound_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_asio\iasiothiscallresolver.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_allocation.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_asio\pa_asio.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_converters.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_cpuload.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_dither.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_endianness.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_hostapi.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_process.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_stream.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_trace.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_types.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\pa_util.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_win_wmme\pa_win_wmme.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_win\pa_x86_plain_converters.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_common\portaudio.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
