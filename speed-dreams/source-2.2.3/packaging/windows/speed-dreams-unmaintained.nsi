;
;        Windows NSIS configuration file for the "Unmaintained contents" installer
;        (obsolete physics engines, ...)
;
;        copyright  : (C) 2012 onwards Jean-Philippe Meuret
;        $Id: speed-dreams-unmaintained.nsi 7159 2020-07-31 13:40:13Z beaglejoe $
;
;        This program is free software; you can redistribute it and/or modify
;        it under the terms of the GNU General Public License as published by
;        the Free Software Foundation; either version 2 of the License, or
;        (at your option) any later version.

; Common definitions.
!include "speed-dreams.nsh"

; MUI Settings
!define MUI_WELCOMEPAGE_TITLE "${GAME_NAME}\n     ${GAME_SUBTITLE}\nUnmaintained contents"
!define MUI_WELCOMEPAGE_TITLE_3LINES

; Installer pages
;Page custom EnterXXXPage LeaveXXXPage
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_INSTFILES

; Language files (Warning: Can't be upper in the script)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "Swedish"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "Slovak"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "Romanian"
!insertmacro MUI_LANGUAGE "Greek"
!insertmacro MUI_LANGUAGE "Turkish"
!insertmacro MUI_LANGUAGE "Arabic"
!insertmacro MUI_LANGUAGE "Farsi"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "SimpChinese"

; Global installer settings.
Name "${GAME_VERSIONNED_NAME}"
Caption "Install Unmaintained contents for ${GAME_VERSIONNED_NAME}"
OutFile "${OUT_PATH}\${GAME_SHORT_FS_NAME}-${UNMAINTAINED_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
InstallDir "$PROGRAMFILES\${INST_INST_DIR_SUFFIX}" ; TODO: Detect base package install dir
InstallDirRegKey ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section -CoreBinaries

  SetOverwrite on

  ; Obsolete physics engines.
  SetOutPath "$INSTDIR\lib\modules\simu"
  File "${BUILD_INST_DIR}\lib\modules\simu\simuv2.1.dll"

SectionEnd

Section -CoreData

  SetOverwrite on

SectionEnd

Section -Drivers

  SetOverwrite on

  ; Warning: Must be here because kilo2008 also drivers the TRB1 cars.
  ; SetOutPath "$INSTDIR\lib\drivers\kilo2008"
  ; File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\kilo2008\*.*"
  
  ; Warning: Must be here because kilo2008 also drivers the TRB1 cars.
  ; SetOutPath "$INSTDIR\data\drivers\kilo2008"
  ; File /r "${BUILD_INST_DIR}\data\drivers\kilo2008\*.*"

SectionEnd

Section -Cars

  SetOverwrite on

SectionEnd

Section -Tracks

  SetOverwrite on

SectionEnd

; This function is called at installer startup.
Function .onInit

  ; Check if the base package is installed and OK, and abort if not.
  !insertmacro CheckIfBasePackageInstalled

FunctionEnd

