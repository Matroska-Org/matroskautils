; ---------------------------------------------------------------------------
; Matroska Info CDL install script for NSIS
; ---------------------------------------------------------------------------

;These defines are all you need to change to make a basic Installer for your CDL Plugin
!define NAME "Matroska TCMP CDL v1.8" 									; This is the name of your App/Plugin
!define OUTFILE "Release\MatroskaCDL.exe" 		; The filename that the installer will be write out to
!define SRC_PREFIX "Release\"
!define CDL_FILE "MatroskaCDL.cdl" 								;Your CDL filename
;!define CDL_FILE_README "MatroskaCDL.txt" 					; Your readme
;!define CDL_DIR "MatroskaCDL"                      ; Dir on harddrive that contains all your extra files like readme.txt some docs etc. comment this out if you don't want a subdir
!define UNINST_NAME "MatroskaCDL-uninstall.exe" 		; Name of the Uninstaller filename. Please leave the '-uninstall.exe'

; ---------------------------------------------------------------------------
; NOTE: this .NSI script is designed for NSIS v1.8+
; ---------------------------------------------------------------------------

;!ifndef NAME|OUTFILE|CDL_FILE|UNINST_NAME
; !error "One of the Required defines is not defined please edit the nsi file and make sure that the required defines are properly defined"
;!endif

Name "${NAME}"
OutFile "${OUTFILE}"

SetCompress auto ; (can be off or force)
SetDatablockOptimize on ; (can be off)
CRCCheck on ; (can be off)
AutoCloseWindow false ; (can be true for the window go away automatically at end)
ShowInstDetails hide ; (can be show to have them shown, or nevershow to disable)
SetDateSave off ; (can be on to have files restored to their orginal date)

LicenseText "Please read the license before continuing:"
LicenseData "..\COPYING.txt"

InstallDir "$PROGRAMFILES\CoreCodec\The Core Media Player\Plugins"
InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\CoreCodec\The Core Media Player\${NAME}" ""
DirText "Select the directory to install ${NAME} in:"

; ---------------------------------------------------------------------------

Section "" ; (default section)
   SetOutPath "$INSTDIR"
   ; add files / whatever that need to be installed here.
   File "${SRC_PREFIX}${CDL_FILE}"
   ;File "..\unicows.dll"
  File "..\ShellExtension\id3lib.dll"
  File "..\ShellExtension\zlib1.dll"

!ifdef CDL_DIR
   File /r "${CDL_DIR}"
!endif

   ; Writes the strings into the registry for TCMP and also to the Installer Menu
   WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\CoreCodec\The Core Media Player\${NAME}" "" "$INSTDIR"
   WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "DisplayName" "${NAME} (remove only)"
   WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}" "UninstallString" '"$INSTDIR\${UNINST_NAME}"'

   ; write out uninstaller
   WriteUninstaller "$INSTDIR\${UNINST_NAME}"

SectionEnd ; end of default section

; ---------------------------------------------------------------------------
; begin uninstall settings/section

UninstallText "This will uninstall ${NAME} from your system"

Section Uninstall
   ; add delete commands to delete whatever files/registry keys/etc you installed here.
   Delete /REBOOTOK "$INSTDIR\${CDL_FILE_README}"   
   Delete /REBOOTOK "$INSTDIR\${CDL_FILE}"   
    Delete /REBOOTOK "$INSTDIR\id3lib.dll"
    Delete /REBOOTOK "$INSTDIR\zlib1.dll"
    
   Delete "$INSTDIR\${UNINST_NAME}"

!ifdef CDL_DIR
   ;This will delete your dir even if other files where added to it.
   RMDir /r "$INSTDIR\${CDL_DIR}"
!endif

   DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\CoreCodec\The Core Media Player\${NAME}"
   DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\${NAME}"
SectionEnd ; end of uninstall section

; ---------------------------------------------------------------------------

Function .onInit
   ReadRegStr $0 HKEY_LOCAL_MACHINE "SOFTWARE\CoreCodec\The Core Media Player" "instpath"
   StrCpy $INSTDIR $0\Plugins
FunctionEnd

; ---------------------------------------------------------------------------

Function un.onUninstSuccess
   IfRebootFlag 0 NoReboot
      MessageBox MB_OK \ 
          "A File Couldn't be deleted. This is most likely due to haveing$\n\
           The Core Media Player running while you tried to uninstall$\n\
           ${NAME} everything will be deleted when you reboot next"
   NoReboot:
FunctionEnd

; ---------------------------------------------------------------------------
; eof
; ---------------------------------------------------------------------------
