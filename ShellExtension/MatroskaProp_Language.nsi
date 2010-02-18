
!define MATROSKAPROP_VERSION_MAJOR 2
!define MATROSKAPROP_VERSION_MINOR 8
!define MATROSKAPROP_VERSION_BUILD 2219

#!define INCLUDE_SOURCE_CODE
!define MATROSKAPROP_SOURCE_ARCHIVE "D:\Visual Studio Projects\matroska\MatroskaUtils\ShellExtension\MatroskaProp-20050402-src.tar.bz2"

!include "${NSISDIR}\Include\WinMessages.nsh"
!include "${NSISDIR}\Include\Sections.nsh"

; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
Name "MatroskaProp"
BrandingText "MatroskaProp ${MATROSKAPROP_VERSION_MAJOR}.${MATROSKAPROP_VERSION_MINOR} Build ${MATROSKAPROP_VERSION_BUILD}"

LoadLanguageFile "..\gettext\lang\Texan_English\Texan_English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Italian.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Portuguese.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Spanish.nlf"

; Set name using the normal interface (Name command)
LangString LanguageName ${LANG_ENGLISH} "English"
LangString LanguageName ${LANG_Texan_English} "Texan English"
LangString LanguageName ${LANG_ITALIAN} "Italian"
LangString LanguageName ${LANG_PORTUGUESE} "Portuguese"
LangString LanguageName ${LANG_SPANISH} "Spanish"

; The language can be the last used language like above, but it can be defined using /LANG
;ComponentText /LANG=${LANG_ENGLISH} "English component page"
;ComponentText /LANG=${LANG_ITALIAN} "Italian component page"
;ComponentText /LANG=${LANG_PORTUGUESE} "Portuguese component page"
;ComponentText /LANG=${LANG_SPANISH} "Spanish component page"

LicenseText "Please read the license before continuing:"
LicenseData "..\COPYING.txt"
;LicenseText /LANG=${LANG_ENGLISH} "Please read the license before continuing:"
;LicenseData /LANG=${LANG_ENGLISH} "..\COPYING.txt"
;LicenseText /LANG=${LANG_Texan_English} "Please read yer license before continuing:"
;LicenseData /LANG=${LANG_Texan_English} "..\COPYING.txt"
;LicenseText /LANG=${LANG_ITALIAN} "Please read the license before continuing:"
;LicenseData /LANG=${LANG_ITALIAN} "..\COPYING.txt"
;LicenseText /LANG=${LANG_PORTUGUESE} "Please read the license before continuing:"
;LicenseData /LANG=${LANG_PORTUGUESE} "..\COPYING.txt"
;LicenseText /LANG=${LANG_SPANISH} "Please read the license before continuing:"
;LicenseData /LANG=${LANG_SPANISH} "..\COPYING.txt"

DirText "Please choose a directory to install MatroskaProp to."
;DirText /LANG=${LANG_Texan_English} "Please choose a directory to rope MatroskaProp to."
;DirText /LANG=${LANG_ITALIAN} "Please choose a directory to install MatroskaProp to."
;DirText /LANG=${LANG_PORTUGUESE} "Please choose a directory to install MatroskaProp to."
;DirText /LANG=${LANG_SPANISH} "Please choose a directory to install MatroskaProp to."

;LangString Sec1Name ${LANG_ENGLISH} "-Matroska Shell Extension"
;LangString Sec1Name ${LANG_ITALIAN} "-Matroska Shell Extension"
;LangString Sec1Name ${LANG_PORTUGUESE} "-Matroska Shell Extension"
;LangString Sec1Name ${LANG_SPANISH} "-Matroska Shell Extension"

LangString translationKey ${LANG_ENGLISH} "LANGUAGE=English"
LangString translationKey ${LANG_Texan_English} "LANGUAGE=Texan_English"
LangString translationKey ${LANG_ITALIAN} "LANGUAGE=Italian"
LangString translationKey ${LANG_PORTUGUESE} "LANGUAGE=Portuguese"
LangString translationKey ${LANG_SPANISH} "LANGUAGE=Spanish"
  
;Turn on XP Styling
XPStyle on
SetOverwrite on
Icon "${NSISDIR}\Contrib\Graphics\Icons\modern-install-colorful.ico"
UninstallIcon "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-colorful.ico"

VIAddVersionKey /LANG=${LANG_ENGLISH} FileDescription "Matroska Shell Ext Installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductName "MatroskaProp"
VIAddVersionKey /LANG=${LANG_ENGLISH} LegalCopyright "(c) 2003-2005 Jory Stone <matroskaprop@jory.info>"
VIAddVersionKey /LANG=${LANG_ENGLISH} FileVersion "${MATROSKAPROP_VERSION_MAJOR}, ${MATROSKAPROP_VERSION_MINOR}, 0, ${MATROSKAPROP_VERSION_BUILD}"
VIAddVersionKey /LANG=${LANG_ENGLISH} ProductVersion "${MATROSKAPROP_VERSION_MAJOR}, ${MATROSKAPROP_VERSION_MINOR}, 0, ${MATROSKAPROP_VERSION_BUILD}"
VIProductVersion "${MATROSKAPROP_VERSION_MAJOR}.${MATROSKAPROP_VERSION_MINOR}.0.${MATROSKAPROP_VERSION_BUILD}"

;NSIS Installer Setup exe
OutFile "Release\MatroskaPropv${MATROSKAPROP_VERSION_MAJOR}.${MATROSKAPROP_VERSION_MINOR}.exe"

;The dest Folder is
InstallDir "$PROGRAMFILES\MatroskaProp"
; Write the installation path into the registry
InstallDirRegKey HKLM SOFTWARE\MatroskaProp "Install_Dir"

ShowInstDetails show

Page license
Page components "" showComponents
Page directory
Page instfiles

InstType "Compact"
InstType "Typical"
InstType "Full"

Section "!MatroskaProp (required)"
  SectionIn 1 2 3 RO

  ; We need the Unicows wrapper installed first
  !insertmacro SectionFlagIsSet 10 SF_SELECTED InstallUnicows +1
  InstallUnicows:
  Call InstallUnicows
  
  SetOutPath "$INSTDIR"

  File "CheckUsage\Release\CheckUsage.exe"

  IfFileExists "$INSTDIR\MatroskaProp.dll" Have_MatroskaProp Need_MatroskaProp
  Have_MatroskaProp:
  ;Preserve the current value of the vars we need to use
  Push $R0
  Push $R1
  Push $R2
  GetDLLVersion "$INSTDIR\MatroskaProp.dll" $R0 $R1
  GetDLLVersionLocal "Release\MatroskaProp.dll" $R0 $R2
  ; $R1 is the build no of the currently installed MatroskaProp,
  ; $R2 is one we are installing
  IntCmpU $R1 $R2 TryToOverwriteSameVersion TryToOverwriteOldVersion TryToOverwriteNewVersion
  
  TryToOverwriteNewVersion:
  MessageBox MB_YESNO "You are trying to install build $R2 on top of build $R1. Are you sure you want to do this?" IDNO AbortInstall

  TryToOverwriteOldVersion:
  TryToOverwriteSameVersion:
  Pop $R2
  Pop $R1
  Pop $R0

  ; Now to copy the main .dll
  TryDelete:
  Delete "$INSTDIR\MatroskaProp.dll"
  IfErrors TryKill
  
  ;TryCopy:
  Need_MatroskaProp:
  File "Release\MatroskaProp.dll"
  Goto CopyDone

  TryKill:
  MessageBox MB_YESNO "Failed to delete existing MatroskaProp.dll. Do you wish to try and End Task the processes using it via CheckUsage?" IDNO GiveUp
  ExecWait "$INSTDIR\CheckUsage.exe"
  Goto TryDelete

  GiveUp:
  Abort "Failed to delete existing MatroskaProp.dll"
  AbortInstall:
  Abort "Install Aborted!"

  CopyDone:
  File "..\COPYING.txt"
  File "MatroskaProp_ChooseIcon.exe"
  File "MatroskaProp_Configure.exe"
  File "..\gettext\gnu_gettext.dll"

  ; Set the default language to be used
  WriteRegStr HKLM "Software\MatroskaProp" "Translation" $(translationKey)

  RegDLL "$INSTDIR\MatroskaProp.dll"
  ;RegDLL "$INSTDIR\MatroskaProp.dll" "ConfigureOptions"
SectionEnd

Section "-Copy Uninstaller"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MatroskaProp" "DisplayName" "MatroskaProp (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MatroskaProp" "UninstallString" "$INSTDIR\MatroskaProp-uninstall.exe"
  WriteUninstaller "MatroskaProp-uninstall.exe"
SectionEnd

SubSection Translations

Section "Italian"
SectionIn 2 3
  ; Copy the language files
  SetOutPath "$INSTDIR\lang\Italian\LC_MESSAGES\"
  File "..\gettext\lang\Italian\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

Section "Portuguese"
SectionIn 2 3
  SetOutPath "$INSTDIR\lang\Portuguese\LC_MESSAGES\"
  File "..\gettext\lang\Portuguese\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

Section "Spanish"
SectionIn 2 3
  SetOutPath "$INSTDIR\lang\Spanish\LC_MESSAGES\"
  File "..\gettext\lang\Spanish\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

Section "Texan English"
SectionIn 2 3
  SetOutPath "$INSTDIR\lang\Texan_English\LC_MESSAGES\"
  File "..\gettext\lang\Texan_English\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

SubSectionEnd

Section "Start Menu Items"
SectionIn 1 2 3
  ; Add the Start-Menu items
  CreateDirectory "$SMPROGRAMS\Matroska Shell Extension"
  CreateShortCut "$SMPROGRAMS\Matroska Shell Extension\Re-Register Shell Extensions.lnk" "rundll32.exe" '"$INSTDIR\MatroskaProp.dll",DllRegisterServer'
  CreateShortCut "$SMPROGRAMS\Matroska Shell Extension\Configuration.lnk" "$INSTDIR\MatroskaProp_Configure.exe" "" "$INSTDIR\CheckUsage.exe" 0
  CreateShortCut "$SMPROGRAMS\Matroska Shell Extension\Choose Icon.lnk" "$INSTDIR\MatroskaProp_ChooseIcon.exe" "" "$INSTDIR\CheckUsage.exe" 0
  CreateShortCut "$SMPROGRAMS\Matroska Shell Extension\Check Usage.lnk" "$INSTDIR\CheckUsage.exe" "" "$INSTDIR\CheckUsage.exe" 0
  CreateShortCut "$SMPROGRAMS\Matroska Shell Extension\Uninstall.lnk" "$INSTDIR\MatroskaProp-uninstall.exe" "" "$INSTDIR\MatroskaProp-uninstall.exe" 0
SectionEnd

!ifdef INCLUDE_SOURCE_CODE
Section "Source Code"
SectionIn 3
  SetOutPath "$INSTDIR"
  File "${MATROSKAPROP_SOURCE_ARCHIVE}"
SectionEnd
!endif

Section "Send Usage Stats"
SectionIn 1 2 3
  StrCpy $3 "SelectedLanguage=$(LanguageName):Version=${MATROSKAPROP_VERSION_MAJOR}.${MATROSKAPROP_VERSION_MINOR}.${MATROSKAPROP_VERSION_BUILD}"  
  WriteRegStr HKLM "Software\MatroskaProp" "Survey" $3
  RegDLL "$INSTDIR\MatroskaProp.dll" "SubmitSurvey"
  #MatroskaPropSurvey::SubmitSurvey $3
  #Pop $0
  ;MessageBox MB_OK $0

SectionEnd

Section "Unicows"
  Call InstallUnicows
SectionEnd

Function showComponents
  SetCurInstType 2
  Call .onSelChange
FunctionEnd

Function .onSelChange
  Push $R0
  ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" CurrentVersion
  Pop $R0
  IfErrors 0 WinNT
  ; Win9x
  ; We need the Unicode wrapper
  !insertmacro SelectSection 10
  WinNT:
FunctionEnd

Function .onInit
	Push ${LANG_ENGLISH}
	Push English
	Push ${LANG_ITALIAN}
	Push Italian
	Push ${LANG_PORTUGUESE}
	Push Portuguese
	Push ${LANG_SPANISH}
	Push Spanish
	Push ${LANG_Texan_English}
	Push "Texan English"
	Push 5 ; the number of languages
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
FunctionEnd

Function .onMouseOverSection
    FindWindow $R0 "#32770" "" $HWNDPARENT
    GetDlgItem $R0 $R0 1022 ; description item

    StrCmp $0 0 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:MatroskaProp, sort of required ;)"

    StrCmp $0 2 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Translations of the UI messages"

    StrCmp $0 3 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Italian Translation by"

    StrCmp $0 4 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Portuguese Translation by"

    StrCmp $0 5 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Spanish Translation by"

    StrCmp $0 6 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Texan English Translation by Jory"

    StrCmp $0 8 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Some Start Menu Shortcuts"

    StrCmp $0 9 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Send one-time usage stats during install. Includes your OS and Language."

    StrCmp $0 10 "" +2
      SendMessage $R0 ${WM_SETTEXT} 0 "STR:Install Unicows Unicode wrapper (required for Win9x systems)"

FunctionEnd

Function InstallUnicows
  IfFileExists "$SYSDIR\Unicows.dll" UnicowsAlreadyInstalled
  SetOutPath "$SYSDIR"
  File "..\Unicows.dll"
  UnicowsAlreadyInstalled:
FunctionEnd


UninstallText "You dare to uninstall MatroskaProp from your system? ;)"
Section "Uninstall"
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\MatroskaProp"

  ; MUST REMOVE UNINSTALLER, too
  Delete $INSTDIR\MatroskaProp-uninstall.exe
  RMDir "$INSTDIR"
SectionEnd

Section "un.MatroskaProp (required)"
  ; remove files
  UnRegDll $INSTDIR\MatroskaProp.dll
  Delete /REBOOTOK $INSTDIR\MatroskaProp.dll
  Delete /REBOOTOK $INSTDIR\gnu_gettext.dll
  Delete $INSTDIR\MatroskaProp_ChooseIcon.exe
  Delete $INSTDIR\MatroskaProp_Configure.exe
  Delete $INSTDIR\CheckUsage.exe
  Delete $INSTDIR\COPYING.txt
SectionEnd

Section "un.Start Menu Items"
  ; Delete the Start-Menu items
  Delete "$SMPROGRAMS\Matroska Shell Extension\Re-Register Shell Extensions.lnk"
  Delete "$SMPROGRAMS\Matroska Shell Extension\Configuration.lnk"
  Delete "$SMPROGRAMS\Matroska Shell Extension\Choose Icon.lnk"
  Delete "$SMPROGRAMS\Matroska Shell Extension\Check Usage.lnk"
  Delete "$SMPROGRAMS\Matroska Shell Extension\Uninstall.lnk"
  
  ; remove the directory
  RMDir "$SMPROGRAMS\Matroska Shell Extension"
SectionEnd

!ifdef INCLUDE_SOURCE_CODE
Section "un.Source Code"
  Delete $INSTDIR\${MATROSKAPROP_SOURCE_ARCHIVE_NAME}
SectionEnd
!endif

Section "un.Italian"
  Delete "$INSTDIR\lang\Italian\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

Section "un.Portuguese"
  Delete "$INSTDIR\lang\Portuguese\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

Section "un.Spanish"
  Delete "$INSTDIR\lang\Spanish\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

Section "un.Texan English"
  Delete "$INSTDIR\lang\Texan_English\LC_MESSAGES\MatroskaProp.mo"
SectionEnd

Function un.onUninstSuccess
   IfRebootFlag 0 NoReboot
      MessageBox MB_OK "A File Couldn't be deleted. It will be deleted when you reboot next"
   NoReboot:
FunctionEnd