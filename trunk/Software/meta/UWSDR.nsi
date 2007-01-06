;NSIS Modern User Interface version 1.70
;UWSDR install script
;Written by Jonathan Naylor

  SetCompressor lzma

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;Configuration

  ;General
  Name "UWSDR 0.7.1"
  OutFile "UWSDR-0.7.1.exe"

  ;Folder selection page
  InstallDir "$PROGRAMFILES\UWSDR"
  
  ;Get install folder from registry if available
  InstallDirRegKey HKCU "Software\UWSDR" "InstPath"

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNI3NG

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Copying.txt"
  !insertmacro MUI_PAGE_DIRECTORY

  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "UWSDR Program Files" SecProgram

  CreateDirectory "$INSTDIR\SDR Files"

  SetOutPath "$INSTDIR"
  
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\UWSDR.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\GUISetup.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\SDRHelp.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\SDRSetup.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\SDREmulator.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\libfftw3f-3.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\portaudio_x86.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxbase28_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxbase28_net_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxmsw28_core_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxmsw28_html_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Help\GUISetup\GUISetup.zip"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Help\SDRSetup\SDRSetup.zip"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Help\UWSDR\UWSDR.zip"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Copying.txt"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Changes.txt"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Install.txt"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Jobs.txt"

  SetOutPath "$INSTDIR\SDR Files"

  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\SDRFiles\13cms.sdr"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\SDRFiles\23cms.sdr"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\SDRFiles\Demo.sdr"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\SDRFiles\SoftRock30.sdr"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\SDRFiles\SoftRock40.sdr"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\SDRFiles\SoftRock80.sdr"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\SDRFiles\SoftRock160.sdr"

  ;Create start menu entry
  CreateDirectory "$SMPROGRAMS\UWSDR"
  CreateShortCut "$SMPROGRAMS\UWSDR\GUISetup.lnk"  "$INSTDIR\GUISetup.exe"
  CreateShortCut "$SMPROGRAMS\UWSDR\SDRHelp.lnk"   "$INSTDIR\SDRHelp.exe"
  CreateShortCut "$SMPROGRAMS\UWSDR\SDRSetup.lnk"  "$INSTDIR\SDRSetup.exe"
  CreateShortCut "$SMPROGRAMS\UWSDR\Licence.lnk"   "$INSTDIR\Copying.txt"
  CreateShortCut "$SMPROGRAMS\UWSDR\Changes.lnk"   "$INSTDIR\Changes.txt"
  CreateShortCut "$SMPROGRAMS\UWSDR\Install.lnk"   "$INSTDIR\Install.txt"
  CreateShortCut "$SMPROGRAMS\UWSDR\TODO.lnk"      "$INSTDIR\Jobs.txt"
  CreateShortCut "$SMPROGRAMS\UWSDR\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

  ;Store install folder
  WriteRegStr HKCU "Software\UWSDR" "InstPath" $INSTDIR

  ;Store Windows uninstall keys
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UWSDR" "DisplayName"      "UWSDR"
  WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UWSDR" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UWSDR" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UWSDR" "NoRepair" 1

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\SDR Files\*.*"
  RMDir  "$INSTDIR\SDR Files"

  Delete "$INSTDIR\*.*"
  RMDir  "$INSTDIR"

  Delete "$SMPROGRAMS\UWSDR\*.*"
  RMDir  "$SMPROGRAMS\UWSDR"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UWSDR"

  DeleteRegKey HKCU "Software\UWSDR"

SectionEnd
