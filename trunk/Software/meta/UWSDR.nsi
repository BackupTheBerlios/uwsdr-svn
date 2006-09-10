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
  Name "UWSDR 0.5.3"
  OutFile "UWSDR-0.5.3.exe"

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

  SetOutPath "$INSTDIR"
  
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\UWSDR.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\GUISetup.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\SDRHelp.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\SDRSetup.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\SDREmulator.exe"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\libfftw3f-3.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\PA19.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxbase26_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxbase26_net_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxmsw26_core_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Release\wxmsw26_html_vc_custom.dll"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Help\GUISetup\GUISetup.zip"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Help\SDRSetup\SDRSetup.zip"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Copying.txt"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Changes.txt"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Jobs.txt"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\Usage.txt"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\meta\UWSDR.ico"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\meta\GUISetup.ico"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\meta\SDRSetup.ico"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\meta\SDRHelp.ico"
  File "C:\Documents and Settings\Jonathan\My Documents\UWSDR_SVN\Software\13cms.sdr"

  ;Create start menu entry
  CreateDirectory "$SMPROGRAMS\UWSDR"
  CreateShortCut "$SMPROGRAMS\UWSDR\GUISetup.lnk"  "$INSTDIR\GUISetup.exe"   "" "$INSTDIR\GUISetup.ico"
  CreateShortCut "$SMPROGRAMS\UWSDR\SDRHelp.lnk"   "$INSTDIR\SDRHelp.exe"    "" "$INSTDIR\SDRHelp.ico"
  CreateShortCut "$SMPROGRAMS\UWSDR\SDRSetup.lnk"  "$INSTDIR\SDRSetup.exe"   "" "$INSTDIR\SDRSetup.ico"
  CreateShortCut "$SMPROGRAMS\UWSDR\Licence.lnk"   "$INSTDIR\Copying.txt"
  CreateShortCut "$SMPROGRAMS\UWSDR\Changes.lnk"   "$INSTDIR\Changes.txt"
  CreateShortCut "$SMPROGRAMS\UWSDR\TODO.lnk"      "$INSTDIR\Jobs.txt"
  CreateShortCut "$SMPROGRAMS\UWSDR\Usage.lnk"     "$INSTDIR\Usage.txt"
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

  Delete "$INSTDIR\*.*"
  RMDir  "$INSTDIR"

  Delete "$SMPROGRAMS\UWSDR\*.*"
  RMDir  "$SMPROGRAMS\UWSDR"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\UWSDR"

  DeleteRegKey HKCU "Software\UWSDR"

SectionEnd
