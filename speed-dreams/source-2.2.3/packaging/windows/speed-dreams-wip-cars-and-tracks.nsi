;
;        Windows NSIS configuration file for the "WIP cars and tracks" installer
;        (remaining cars and tracks that don't fit into the "HQ cars and tracks (i)" packages,
;         because of lower quality graphics, unfinished AI support, ...)
;
;        copyright  : (C) 2011 onwards Jean-Philippe Meuret
;        $Id: speed-dreams-wip-cars-and-tracks.nsi 8079 2022-03-07 16:41:49Z beaglejoe $
;
;        This program is free software; you can redistribute it and/or modify
;        it under the terms of the GNU General Public License as published by
;        the Free Software Foundation; either version 2 of the License, or
;        (at your option) any later version.

; Common definitions.
!include "speed-dreams.nsh"

; MUI Settings
!define MUI_WELCOMEPAGE_TITLE "${GAME_NAME}\n     ${GAME_SUBTITLE}\nMore tracks, cars and AI drivers (WIP)"
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
Caption "Install WIP tracks, cars & AI drivers for ${GAME_VERSIONNED_NAME}"
OutFile "${OUT_PATH}\${GAME_SHORT_FS_NAME}-${WIPCARSTRACKS_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
InstallDir "$PROGRAMFILES\${INST_INST_DIR_SUFFIX}" ; TODO: Detect base package install dir
InstallDirRegKey ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section -CoreData

  SetOverwrite on

  ; Game core data files (racemanagers, ...)
  SetOutPath "$INSTDIR\data\config\raceman"
  File "${BUILD_INST_DIR}\data\config\raceman\singleevent-mp5.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship-24h.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship-67gp.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship-supercars.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\championship-mp5.xml"
  ;File "${BUILD_INST_DIR}\data\config\raceman\career.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\career-36GP.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\career-supercars.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\career_supercars.xmls"
  File "${BUILD_INST_DIR}\data\config\raceman\career_ls1.xmls"
  File "${BUILD_INST_DIR}\data\config\raceman\career_36GP.xmls"
  File "${BUILD_INST_DIR}\data\config\raceman\career_MPA1.xmls"

  ; Moved from 'speed-dreams-base.nsi' for version 2.1.0
  File "${BUILD_INST_DIR}\data\config\raceman\networkrace.xml"

SectionEnd

Section -Drivers

  SetOverwrite on

  ; Robots binaries
  SetOutPath "$INSTDIR\lib\drivers\simplix_mp5"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix_mp5\*.*"

  SetOutPath "$INSTDIR\lib\drivers\simplix_ls2"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix_ls2\*.*"

  SetOutPath "$INSTDIR\lib\drivers\simplix_67GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix_67GP\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr_ls2"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr_ls2\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr_rs"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr_rs\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr_67GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr_67GP\*.*"

  SetOutPath "$INSTDIR\lib\drivers\shadow_ls2"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow_ls2\*.*"

  ;SetOutPath "$INSTDIR\lib\drivers\shadow_rs"
  ;File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow_rs\*.*"

  ;SetOutPath "$INSTDIR\lib\drivers\shadow_mp1"
  ;File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow_mp1\*.*"

  SetOutPath "$INSTDIR\lib\drivers\shadow_67GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow_67GP\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_ls1"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_ls1\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_ls2"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_ls2\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_mpa1"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_mpa1\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_rs"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_rs\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_srw"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_srw\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_trb1"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_trb1\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_67GP"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_67GP\*.*"

  ; Robots data files (car and track setups)
  SetOutPath "$INSTDIR\data\drivers\simplix_mp5"
  File /r "${BUILD_INST_DIR}\data\drivers\simplix_mp5\*.*"

  SetOutPath "$INSTDIR\data\drivers\simplix_ls2"
  File /r "${BUILD_INST_DIR}\data\drivers\simplix_ls2\*.*"

  SetOutPath "$INSTDIR\data\drivers\simplix_67GP"
  File /r "${BUILD_INST_DIR}\data\drivers\simplix_67GP\*.*"

  SetOutPath "$INSTDIR\data\drivers\usr_ls2"
  File /r "${BUILD_INST_DIR}\data\drivers\usr_ls2\*.*"

  SetOutPath "$INSTDIR\data\drivers\usr_rs"
  File /r "${BUILD_INST_DIR}\data\drivers\usr_rs\*.*"

  SetOutPath "$INSTDIR\data\drivers\usr_67GP"
  File /r "${BUILD_INST_DIR}\data\drivers\usr_67GP\*.*"

  SetOutPath "$INSTDIR\data\drivers\shadow_ls2"
  File /r "${BUILD_INST_DIR}\data\drivers\shadow_ls2\*.*"

  ;SetOutPath "$INSTDIR\data\drivers\shadow_rs"
  ;File /r "${BUILD_INST_DIR}\data\drivers\shadow_rs\*.*"

  ;SetOutPath "$INSTDIR\data\drivers\shadow_mp1"
  ;File /r "${BUILD_INST_DIR}\data\drivers\shadow_mp1\*.*"

  SetOutPath "$INSTDIR\data\drivers\shadow_67GP"
  File /r "${BUILD_INST_DIR}\data\drivers\shadow_67GP\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_ls1"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_ls1\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_ls2"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_ls2\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_mpa1"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_mpa1\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_rs"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_rs\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_srw"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_srw\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_trb1"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_trb1\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_67GP"
  File /r "${BUILD_INST_DIR}\data\drivers\dandroid_67GP\*.*"


SectionEnd

; Lower quality / not AI-driven / work-in-progress car models
Section -Cars

  SetOverwrite on

  SetOutPath "$INSTDIR\data\cars\models\mp5-fmc"
  File /r "${BUILD_INST_DIR}\data\cars\models\mp5-fmc\*.*"

  SetOutPath "$INSTDIR\data\cars\models\mp5-fmc-drift"
  File /r "${BUILD_INST_DIR}\data\cars\models\mp5-fmc-drift\*.*"

  SetOutPath "$INSTDIR\data\cars\models\mp5-fmc-speedy"
  File /r "${BUILD_INST_DIR}\data\cars\models\mp5-fmc-speedy\*.*"

  SetOutPath "$INSTDIR\data\cars\models\rs1-citrus-cs4"
  File /r "${BUILD_INST_DIR}\data\cars\models\rs1-citrus-cs4\*.*"

  SetOutPath "$INSTDIR\data\cars\models\rs1-fmc-centre"
  File /r "${BUILD_INST_DIR}\data\cars\models\rs1-fmc-centre\*.*"

  SetOutPath "$INSTDIR\data\cars\models\rs1-kenji-08"
  File /r "${BUILD_INST_DIR}\data\cars\models\rs1-kenji-08\*.*"

  SetOutPath "$INSTDIR\data\cars\models\rs2-fai-dot"
  File /r "${BUILD_INST_DIR}\data\cars\models\rs2-fai-dot\*.*"

  SetOutPath "$INSTDIR\data\cars\models\rs2-lion-2m"
  File /r "${BUILD_INST_DIR}\data\cars\models\rs2-lion-2m\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-bavaria-g3gtr"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-bavaria-g3gtr\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-boxer-96rsr"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-boxer-96rsr\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-cavallo-360r"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-cavallo-360r\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-condor-c400r"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-condor-c400r\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-lynx-xkr"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-lynx-xkr\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-morley-r900"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-morley-r900\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-sprite-gts"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-sprite-gts\*.*"

  SetOutPath "$INSTDIR\data\cars\models\ls2-vortex-gt"
  File /r "${BUILD_INST_DIR}\data\cars\models\ls2-vortex-gt\*.*"

  ;SetOutPath "$INSTDIR\data\cars\models\ls1-cavallo-570s1"
  ;File /r "${BUILD_INST_DIR}\data\cars\models\ls1-cavallo-570s1\*.*"

  SetOutPath "$INSTDIR\data\cars\models\srw-boxer-ts"
  File /r "${BUILD_INST_DIR}\data\cars\models\srw-boxer-ts\*.*"

  SetOutPath "$INSTDIR\data\cars\models\srw-kanagawa-gtr"
  File /r "${BUILD_INST_DIR}\data\cars\models\srw-kanagawa-gtr\*.*"

  SetOutPath "$INSTDIR\data\cars\models\srw-kongei-kk8s"
  File /r "${BUILD_INST_DIR}\data\cars\models\srw-kongei-kk8s\*.*"

  SetOutPath "$INSTDIR\data\cars\models\srw-sector-p4"
  File /r "${BUILD_INST_DIR}\data\cars\models\srw-sector-p4\*.*"

  SetOutPath "$INSTDIR\data\cars\models\67gp-upping-ut19"
  File /r "${BUILD_INST_DIR}\data\cars\models\67gp-upping-ut19\*.*"

  SetOutPath "$INSTDIR\data\cars\models\67gp-cavallo-123"
  File /r "${BUILD_INST_DIR}\data\cars\models\67gp-cavallo-123\*.*"

  SetOutPath "$INSTDIR\data\cars\models\67gp-guthrie-eagle"
  File /r "${BUILD_INST_DIR}\data\cars\models\67gp-guthrie-eagle\*.*"

  SetOutPath "$INSTDIR\data\cars\models\67gp-kuiper-t81"
  File /r "${BUILD_INST_DIR}\data\cars\models\67gp-kuiper-t81\*.*"

  SetOutPath "$INSTDIR\data\cars\models\67gp-mango-ms7"
  File /r "${BUILD_INST_DIR}\data\cars\models\67gp-mango-ms7\*.*"

SectionEnd

Section -Tracks

  SetOverwrite on

  ; Lower quality / work-in-progress tracks
  SetOutPath "$INSTDIR\data\tracks\circuit\goldstone-sand"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\goldstone-sand\*.*"

  SetOutPath "$INSTDIR\data\tracks\circuit\tuna"
  File /r "${BUILD_INST_DIR}\data\tracks\circuit\tuna\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\torino"
  File /r "${BUILD_INST_DIR}\data\tracks\road\torino\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\e-track-4"
  File /r "${BUILD_INST_DIR}\data\tracks\road\e-track-4\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\spring"
  File /r "${BUILD_INST_DIR}\data\tracks\road\spring\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\street-1"
  File /r "${BUILD_INST_DIR}\data\tracks\road\street-1\*.*"

  SetOutPath "$INSTDIR\data\tracks\road\nordschleife"
  File /r "${BUILD_INST_DIR}\data\tracks\road\nordschleife\*.*"

  SetOutPath "$INSTDIR\data\tracks\dirt\dirt-1"
  File /r "${BUILD_INST_DIR}\data\tracks\dirt\dirt-1\*.*"

  SetOutPath "$INSTDIR\data\tracks\dirt\dirt-2"
  File /r "${BUILD_INST_DIR}\data\tracks\dirt\dirt-2\*.*"

  SetOutPath "$INSTDIR\data\tracks\dirt\dirt-4"
  File /r "${BUILD_INST_DIR}\data\tracks\dirt\dirt-4\*.*"

  SetOutPath "$INSTDIR\data\tracks\dirt\dirt-6"
  File /r "${BUILD_INST_DIR}\data\tracks\dirt\dirt-6\*.*"

  SetOutPath "$INSTDIR\data\tracks\dirt\mixed-1"
  File /r "${BUILD_INST_DIR}\data\tracks\dirt\mixed-1\*.*"

  SetOutPath "$INSTDIR\data\tracks\dirt\mixed-2"
  File /r "${BUILD_INST_DIR}\data\tracks\dirt\mixed-2\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\a-speedway"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\a-speedway\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\b-speedway"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\b-speedway\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\c-speedway"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\c-speedway\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\caddo"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\caddo\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\corn"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\corn\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\d-speedway"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\d-speedway\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\e-speedway"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\e-speedway\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\e-track-5"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\e-track-5\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\f-speedway"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\f-speedway\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\longpond"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\longpond\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\sunshine"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\sunshine\*.*"

  SetOutPath "$INSTDIR\data\tracks\speedway\tennessee"
  File /r "${BUILD_INST_DIR}\data\tracks\speedway\tennessee\*.*"

SectionEnd

; This function is called at installer startup.
Function .onInit

  ; Check if the base package is installed and OK, and abort if not.
  !insertmacro CheckIfBasePackageInstalled

FunctionEnd

