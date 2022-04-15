;
;        Windows NSIS configuration file for the "HQ Cars & Tracks (1)" installer
;        (HQ tracks and cars with full AI support)
;
;        copyright  : (C) 2011 onwards Jean-Philippe Meuret
;        $Id: speed-dreams-hq-cars-and-tracks.nsi 7487 2021-05-02 12:24:54Z torcs-ng $
;
;        This program is free software; you can redistribute it and/or modify
;        it under the terms of the GNU General Public License as published by
;        the Free Software Foundation; either version 2 of the License, or
;        (at your option) any later version.

; Common definitions.
!include "speed-dreams.nsh"

;SetCompressor /SOLID lzma ; 1-block compression, smaller but longer (generation, extraction)

; MUI Settings
!define MUI_WELCOMEPAGE_TITLE "${GAME_NAME}\n     ${GAME_SUBTITLE}\nHQ (1) tracks, cars and AI drivers"
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
Caption "Install HQ tracks, cars & AI drivers for ${GAME_VERSIONNED_NAME}"
OutFile "${OUT_PATH}\${GAME_SHORT_FS_NAME}-${HQCARSTRACKS_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
InstallDir "$PROGRAMFILES\${INST_INST_DIR_SUFFIX}"
InstallDirRegKey ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section -CoreData

  SetOverwrite on

  ; Game core data files (racemanagers, ...)
  SetOutPath "$INSTDIR\data\config\raceman"
  File "${BUILD_INST_DIR}\data\config\raceman\singleevent-36gp.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\singleevent-ls-gt1.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship-supercars.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship-36gp.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship-ls-gt1.xml"

SectionEnd

Section -Drivers

  SetOverwrite on

  ; Customizable robots binaries (needed for the Career mode)
  SetOutPath "$INSTDIR\lib\drivers\simplix"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid\*.*"

  ; Customizable robot data files (car and track setups) (needed for the Career mode)
  SetOutPath "$INSTDIR\data\drivers\simplix"
  File /r "${BUILD_INST_DIR}\data\drivers\simplix\*.*"

  ; Robots binaries
  SetOutPath "$INSTDIR\lib\drivers\simplix_ls1"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix_ls1\*.*"

  SetOutPath "$INSTDIR\lib\drivers\simplix_36GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix_36GP\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr_ls1"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr_ls1\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_36GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_36GP\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr_36GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr_36GP\*.*"

  SetOutPath "$INSTDIR\lib\drivers\shadow_ls1"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow_ls1\*.*"

  SetOutPath "$INSTDIR\lib\drivers\shadow_36GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow_36GP\*.*"

  ; SetOutPath "$INSTDIR\lib\drivers\kilo2008"
  ; File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\kilo2008\*.*"

  ; Robots data files (car and track setups)
  SetOutPath "$INSTDIR\data\drivers\simplix_ls1"
  File /r "${BUILD_INST_DIR}\data\drivers\simplix_ls1\*.*"

  SetOutPath "$INSTDIR\data\drivers\simplix_36GP"
  File /r "${BUILD_INST_DIR}\data\drivers\simplix_36GP\*.*"

  SetOutPath "$INSTDIR\data\drivers\usr_ls1"
  File /r "${BUILD_INST_DIR}\data\drivers\usr_ls1\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_36GP"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_36GP\*.*"

  SetOutPath "$INSTDIR\data\drivers\usr_36GP"
  File /r "${BUILD_INST_DIR}\data\drivers\usr_36GP\*.*"

  SetOutPath "$INSTDIR\data\drivers\shadow_ls1"
  File /r "${BUILD_INST_DIR}\data\drivers\shadow_ls1\*.*"

  SetOutPath "$INSTDIR\data\drivers\shadow_36GP"
  File /r "${BUILD_INST_DIR}\data\drivers\shadow_36GP\*.*"

  ; SetOutPath "$INSTDIR\data\drivers\kilo2008"
  ; File /r "${BUILD_INST_DIR}\data\drivers\kilo2008\*.*"

SectionEnd

Section -Cars

  SetOverwrite on

  ; HQ / AI driven LD-GT1 car models
  SetOutPath "$INSTDIR\data\cars\models\ls1-archer-r9"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-archer-r9\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-cavallo-570s1"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-cavallo-570s1\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-marisatech-gt4r"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-marisatech-gt4r\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-newcastle-fury"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-newcastle-fury\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-taipan-ltsr"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-taipan-ltsr\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-toro-rgt"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-toro-rgt\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-vulture-v5r"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-vulture-v5r\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-vulture-v6r"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-vulture-v6r\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls1-zentek-z7r"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls1-zentek-z7r\*.*"

  ; HQ / AI driven 36 Grand-Prix car models
  SetOutPath "$INSTDIR\data\cars\models\36gp-century-r11b"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-century-r11b\*.*"

  SetOutPath "$INSTDIR\data\cars\models\36gp-ettore-t59"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-ettore-t59\*.*"

  SetOutPath "$INSTDIR\data\cars\models\36gp-milano-12c36"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-milano-12c36\*.*"

  SetOutPath "$INSTDIR\data\cars\models\36gp-milano-tipop3"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-milano-tipop3\*.*"

  SetOutPath "$INSTDIR\data\cars\models\36gp-motorunion-typec"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-motorunion-typec\*.*"

  SetOutPath "$INSTDIR\data\cars\models\36gp-silber-w25b"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-silber-w25b\*.*"

  SetOutPath "$INSTDIR\data\cars\models\36gp-tridenti-6c34"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-tridenti-6c34\*.*"

  SetOutPath "$INSTDIR\data\cars\models\36gp-tridenti-v8ri"
  File /r "${BUILD_INST_DIR}\data\cars\models\36gp-tridenti-v8ri\*.*"

SectionEnd

Section -Tracks

  SetOverwrite on

  ; HQ (1) circuits
  SetOutPath "$INSTDIR\data\tracks\circuit\brondehach"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\brondehach\*.*"

  SetOutPath "$INSTDIR\data\tracks\circuit\chemisay"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\chemisay\*.*"

  SetOutPath "$INSTDIR\data\tracks\circuit\corkscrew"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\corkscrew\*.*"

  SetOutPath "$INSTDIR\data\tracks\circuit\forza"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\forza\*.*"

  SetOutPath "$INSTDIR\data\tracks\circuit\karwada"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\karwada\*.*"

  SetOutPath "$INSTDIR\data\tracks\circuit\hidden-valley"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\hidden-valley\*.*"

  ; HQ (1) road tracks
  SetOutPath "$INSTDIR\data\tracks\road\allondaz"
  File /r "${BUILD_INST_DIR}\data\tracks\road\allondaz\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\alicante"
  File /r "${BUILD_INST_DIR}\data\tracks\road\alicante\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\auriac"
  File /r "${BUILD_INST_DIR}\data\tracks\road\auriac\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\charmey"
  File /r "${BUILD_INST_DIR}\data\tracks\road\charmey\*.*"

  ; HQ (1) speed ways
  SetOutPath "$INSTDIR\data\tracks\speedway\arizona"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\arizona\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\manton"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\manton\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\mile"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\mile\*.*"

SectionEnd

; This function is called at installer startup.
Function .onInit

  ; Check if the base package is installed and OK, and abort if not.
  !insertmacro CheckIfBasePackageInstalled

FunctionEnd

