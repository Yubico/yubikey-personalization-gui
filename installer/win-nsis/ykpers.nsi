!include "MUI.nsh"

; The name of the installer
Name "YubiKey Personalization Tool"

; The file to write
OutFile "../../build/release/yubikey-personalization-gui-${YKPERS_VERSION}.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Yubico\YubiKey Personalization Tool"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Yubico\yubikey-personalization-gui" "Install_Dir"

SetCompressor /SOLID lzma
ShowInstDetails show

Var MUI_TEMP
Var STARTMENU_FOLDER

Function .onInit
    # Display splash image.
    InitPluginsDir

    File /oname=$PLUGINSDIR\splash.bmp "yubico_logo.bmp"
    advsplash::show 1000 600 400 0x04025C $PLUGINSDIR\splash
    Pop $0

    Delete $PLUGINSDIR\splash.bmp
FunctionEnd

;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------

; Pages
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "../../COPYING"
  !insertmacro MUI_PAGE_DIRECTORY
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER "Yubico\YubiKey Personalization Tool"
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Yubico\YubiKey Personalization Tool"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;Languages
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------

Section "YubiKey Personalization Tool"
  SectionIn RO
  SetOutPath $INSTDIR
  FILE ../../build/release/yubikey-personalization-gui.exe
  FILE ../../build/release/libyubikey-0.dll
  FILE ../../build/release/libykpers-1-1.dll
  FILE ../../build/release/QtCore4.dll
  FILE ../../build/release/QtGui4.dll
  FILE ../../build/release/libgcc_s_dw2-1.dll
  FILE ../../build/release/mingwm10.dll
  SetOutPath $INSTDIR\imageformats
  FILE ../../build/release/qmng4.dll
SectionEnd

Var MYTMP

# Last section is a hidden one.
Section
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Write the installation path into the registry
  WriteRegStr HKLM "Software\Yubico\yubikey-personalization-gui" "Install_Dir" "$INSTDIR"

  # Windows Add/Remove Programs support
  StrCpy $MYTMP "Software\Microsoft\Windows\CurrentVersion\Uninstall\yubikey-personalization-gui"
  WriteRegStr       HKLM $MYTMP "DisplayName"     "YubiKey Personalization Tool"
  WriteRegExpandStr HKLM $MYTMP "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegExpandStr HKLM $MYTMP "InstallLocation" "$INSTDIR"
  WriteRegStr       HKLM $MYTMP "DisplayVersion"  "${YKPERS_VERSION}"
  WriteRegStr       HKLM $MYTMP "Publisher"       "Yubico AB"
  WriteRegStr       HKLM $MYTMP "URLInfoAbout"    "http://www.yubico.com"
  WriteRegDWORD     HKLM $MYTMP "NoModify"        "1"
  WriteRegDWORD     HKLM $MYTMP "NoRepair"        "1"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
;Create shortcuts
  SetShellVarContext all
  SetOutPath "$SMPROGRAMS\$STARTMENU_FOLDER"
  CreateShortCut "YubiKey Personalization Tool.lnk" "$INSTDIR\yubikey-personalization-gui.exe" "" "$INSTDIR\yubikey-personalization-gui.exe" 0
  CreateShortCut "Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 1
  WriteINIStr "$SMPROGRAMS\$STARTMENU_FOLDER\Yubico Web page.url" \
                   "InternetShortcut" "URL" "http://www.yubico.com/"
!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\yubikey-personalization-gui"
  DeleteRegKey HKLM "Software\Yubico\yubikey-personalization-gui"

  DELETE "$INSTDIR\yubikey-personalization-gui.exe"
  DELETE "$INSTDIR\libyubikey-0.dll"
  DELETE "$INSTDIR\libykpers-1-1.dll"
  DELETE "$INSTDIR\QtCore4.dll"
  DELETE "$INSTDIR\QtGui4.dll"
  DELETE "$INSTDIR\libgcc_s_dw2-1.dll"
  DELETE "$INSTDIR\libgcc_s_sjlj-1.dll"
  DELETE "$INSTDIR\libstdc++-6.dll"
  DELETE "$INSTDIR\mingwm10.dll"
  DELETE "$INSTDIR\imageformats\qmng4.dll"

  RMDir "$INSTDIR\imageformats"

  ; Remove uninstaller
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  SetShellVarContext all

  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\Yubico Web page.url"
  Delete "$SMPROGRAMS\$MUI_TEMP\YubiKey Personalization Tool.lnk"

  ;Delete empty start menu parent diretories
  StrCpy $MUI_TEMP "$SMPROGRAMS\$MUI_TEMP"

  startMenuDeleteLoop:
	ClearErrors
    RMDir $MUI_TEMP
    GetFullPathName $MUI_TEMP "$MUI_TEMP\.."

    IfErrors startMenuDeleteLoopDone

    StrCmp $MUI_TEMP $SMPROGRAMS startMenuDeleteLoopDone startMenuDeleteLoop
  startMenuDeleteLoopDone:

  DeleteRegKey /ifempty HKCU "Software\Yubico\yubikey-personalization-gui"

  ; Remove directories used
  RMDir "$INSTDIR"
SectionEnd
