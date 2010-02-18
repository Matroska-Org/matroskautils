
;Turn on XP Styling
XPStyle on

SetOverwrite try

;Program Name
Name "The Matroska Shell Extension"

;NSIS Installer Setup exe
OutFile "Release\MatroskaProp-Update.exe"

;The dest Folder is
InstallDir "$PROGRAMFILES\MatroskaProp"
; Write the installation path into the registry
InstallDirRegKey HKLM SOFTWARE\MatroskaProp "Install_Dir"

;License Introduction + Filename
LicenseText "Please read the license before continuing:"
LicenseData "..\COPYING.txt"

Page license
Page instfiles

Section "Matroska Shell Extension"
  ;SetOutPath "$SYSDIR"
  SetOutPath "$INSTDIR"  
  File "Release\MatroskaProp.dll"

  File "..\COPYING.txt"
  ;Call DoRegStuff
  RegDLL "$INSTDIR\MatroskaProp.dll"
  ;RegDLL "$INSTDIR\MatroskaProp.dll" "ConfigureOptions"

   ; Add the Start-Menu items
   CreateDirectory "$SMPROGRAMS\Matroska Shell Extension"
   CreateShortCut "$SMPROGRAMS\Matroska Shell Extension\Re-Register Shell Extensions.lnk" "rundll32.exe" '"$INSTDIR\MatroskaProp.dll",DllRegisterServer'
  CreateShortCut "$SMPROGRAMS\Matroska Shell Extension\Uninstall.lnk" "$INSTDIR\MatroskaProp-uninstall.exe" "" "$INSTDIR\MatroskaProp-uninstall.exe" 0

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MatroskaProp" "DisplayName" "MatroskaProp (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MatroskaProp" "UninstallString" "$INSTDIR\MatroskaProp-uninstall.exe"
  WriteUninstaller "MatroskaProp-uninstall.exe"
SectionEnd

;Uninstall stuff
UninstallText "What!?!?! You dare to uninstall The Matroska Shell Extension? Hit next to continue."

Section "Uninstall"
  ; remove files
  UnRegDll $INSTDIR\MatroskaProp.dll
  Delete /REBOOTOK $INSTDIR\MatroskaProp.dll

  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MatroskaProp"

  ; remove shortcuts, if any.
  Delete "$SMPROGRAMS\Matroska Shell Extension\*.*"
  ; remove directories used.
  RMDir "$SMPROGRAMS\Matroska Shell Extension"

  ; MUST REMOVE UNINSTALLER, too
  Delete $INSTDIR\MatroskaProp-uninstall.exe
  RMDir "$INSTDIR"

SectionEnd

Function un.onUninstSuccess
   IfRebootFlag 0 NoReboot
      MessageBox MB_OK "A File Couldn't be deleted. It will be deleted when you reboot next"
   NoReboot:
FunctionEnd

