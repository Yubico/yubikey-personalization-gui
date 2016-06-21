!include "MUI.nsh"
!include "LogicLib.nsh"

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
    # Display splash image it not silent
    ${IfNot} ${Silent}
        InitPluginsDir

        File /oname=$PLUGINSDIR\splash.bmp "yubico_logo.bmp"
        advsplash::show 1000 600 400 0x04025C $PLUGINSDIR\splash
        Pop $0

        Delete $PLUGINSDIR\splash.bmp
    ${EndIf}
FunctionEnd

;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------

; Pages
  !insertmacro MUI_PAGE_WELCOME
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
  FILE ../../build/release/libjson-c-2.dll
  FILE ../../build/release/libyubikey-0.dll
  FILE ../../build/release/libykpers-1-1.dll
  FILE ../../build/release/Qt5Core.dll
  FILE ../../build/release/Qt5Gui.dll
  FILE ../../build/release/Qt5Widgets.dll
  FILE ../../build/release/libgcc_s_dw2-1.dll
  FILE ../../build/release/libwinpthread-1.dll
  FILE ../../build/release/libstdc++-6.dll
  FILE ../../build/release/icuin54.dll
  FILE ../../build/release/icuuc54.dll
  FILE ../../build/release/icudt54.dll
  SetOutPath $INSTDIR\licenses
  FILE /r ../../build/release/licenses/
  SetOutPath $INSTDIR\imageformats
  FILE ../../build/release/qmng.dll
  SetOutPath $INSTDIR\platforms
  FILE ../../build/release/qwindows.dll
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
  WriteRegStr       HKLM $MYTMP "URLInfoAbout"    "https://www.yubico.com"
  WriteRegDWORD     HKLM $MYTMP "NoModify"        "1"
  WriteRegDWORD     HKLM $MYTMP "NoRepair"        "1"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
;Create shortcuts
  SetShellVarContext all
  SetOutPath "$SMPROGRAMS\$STARTMENU_FOLDER"
  CreateShortCut "YubiKey Personalization Tool.lnk" "$INSTDIR\yubikey-personalization-gui.exe" "" "$INSTDIR\yubikey-personalization-gui.exe" 0
  CreateShortCut "Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 1
  WriteINIStr "$SMPROGRAMS\$STARTMENU_FOLDER\Yubico Web page.url" \
                   "InternetShortcut" "URL" "https://www.yubico.com/"
!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\yubikey-personalization-gui"
  DeleteRegKey HKLM "Software\Yubico\yubikey-personalization-gui"

  DELETE "$INSTDIR\yubikey-personalization-gui.exe"
  DELETE "$INSTDIR\libjson-c-2.dll"
  DELETE "$INSTDIR\libyubikey-0.dll"
  DELETE "$INSTDIR\libykpers-1-1.dll"
  DELETE "$INSTDIR\Qt5Core.dll"
  DELETE "$INSTDIR\Qt5Gui.dll"
  DELETE "$INSTDIR\Qt5Widgets.dll"
  DELETE "$INSTDIR\libgcc_s_dw2-1.dll"
  DELETE "$INSTDIR\libwinpthread-1.dll"
  DELETE "$INSTDIR\libstdc++-6.dll"
  DELETE "$INSTDIR\icuin54.dll"
  DELETE "$INSTDIR\icuuc54.dll"
  DELETE "$INSTDIR\icudt54.dll"
  DELETE "$INSTDIR\licenses\*"
  DELETE "$INSTDIR\imageformats\qmng.dll"
  DELETE "$INSTDIR\platforms\qwindows.dll"

  RMDir "$INSTDIR\licenses"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\accessible"
  RMDir "$INSTDIR\platforms"

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
