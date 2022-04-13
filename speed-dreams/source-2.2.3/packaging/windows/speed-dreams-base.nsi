;
;        Windows NSIS configuration file for the base=main=minimal=core installer
;
;        copyright  : (C) 2005 Bernhard Wymann, 2009-2012 Xavier Bertaux, Jean-Philippe Meuret
;        $Id: speed-dreams.nsi 1855 2009-11-21 17:01:00Z pouillot $
;
;        This program is free software; you can redistribute it and/or modify
;        it under the terms of the GNU General Public License as published by
;        the Free Software Foundation; either version 2 of the License, or
;        (at your option) any later version.

; Common definitions.
!include "speed-dreams.nsh"

; MUI Settings
!define MUI_WELCOMEPAGE_TITLE "${GAME_NAME}\n     ${GAME_SUBTITLE}"
!define MUI_WELCOMEPAGE_TITLE_3LINES
;!define MUI_WELCOMEPAGE_TEXT "MUI welcome page text\n2nd line\n3rd line\n... etc ..."

!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\${GAME_FS_NAME}.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED

; Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${BUILD_INST_DIR}\data\COPYING.txt"
Page custom EnterReinstallPage LeaveReinstallPage
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
;!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;Reserve Files (these files should be inserted before other files in the data block)
ReserveFile "speed-dreams.ini"
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

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
Caption "Install ${GAME_VERSIONNED_NAME}"
UninstallCaption "Uninstall ${GAME_VERSIONNED_NAME}"
OutFile "${OUT_PATH}\${GAME_SHORT_FS_NAME}-${BASE_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
InstallDir "$PROGRAMFILES\${INST_INST_DIR_SUFFIX}"
InstallDirRegKey ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "!Base System" SEC01

  SetOverwrite on

  ; License and readme files
  SetOutPath "$INSTDIR"
  File /oname=licence.txt "${BUILD_INST_DIR}\data\COPYING.txt"
  File /oname=changes.txt "${BUILD_INST_DIR}\data\CHANGES.txt"
  ;File /oname=todo.txt "${BUILD_INST_DIR}\data\TODO.txt"
  File /oname=readme.txt readme_for_user.txt

  ; Executable and DLLs ... but no mod tool (see later)
  SetOutPath "$INSTDIR\bin"
  File /x sd2-*.* /x *.lib "${BUILD_INST_DIR}\bin\*.*"
  File /x libzlib.dll /x zlib1.dll "${BUILD_INST_DIR}\bin\*.*"

  ; Core loadable modules (physics and graphics engines, track loader, ...)
  SetOutPath "$INSTDIR\lib\modules\graphic"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\modules\graphic\*.*"
  SetOutPath "$INSTDIR\lib\modules\simu"
  File "${BUILD_INST_DIR}\lib\modules\simu\simuv4.dll"
  SetOutPath "$INSTDIR\lib\modules\telemetry"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\modules\telemetry\*.*"
  SetOutPath "$INSTDIR\lib\modules\track"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\modules\track\*.*"
  SetOutPath "$INSTDIR\lib\modules\userinterface"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\modules\userinterface\*.*"
  SetOutPath "$INSTDIR\lib\modules\sound"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\modules\sound\*.*"
  SetOutPath "$INSTDIR\lib\modules\racing"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\modules\racing\*.*"
  ;SetOutPath "$INSTDIR\lib\modules\networking"
  ;File /r /x *.lib "${BUILD_INST_DIR}\lib\modules\networking\*.*"
  SetOutPath "$INSTDIR\lib\simulatedDrivingAssistance\backend\*.*"

  ; might need to make dll modules in the future
  File /r "${BUILD_INST_DIR}\lib\simulatedDrivingAssistance\backend\*.*"
  SetOutPath "$INSTDIR\lib\simulatedDrivingAssistance\frontend\*.*"
  File /r "${BUILD_INST_DIR}\lib\simulatedDrivingAssistance\frontend\*.*"

  ; Core data files (sound, textures, menus, car categories, ...)
  SetOutPath "$INSTDIR\data"
  File "${BUILD_INST_DIR}\data\*.*"

  SetOutPath "$INSTDIR\data\cars\categories"
  File /r "${BUILD_INST_DIR}\data\cars\categories\*.*"

  SetOutPath "$INSTDIR\data\config"
  File "${BUILD_INST_DIR}\data\config\*.*"

  SetOutPath "$INSTDIR\data\config\raceman\extra"
  File /r "${BUILD_INST_DIR}\data\config\raceman\extra\*.*"

  SetOutPath "$INSTDIR\data\config\raceman"
  File "${BUILD_INST_DIR}\data\config\raceman\practice.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\quickrace.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\singleevent-challenge.xml"
  File "${BUILD_INST_DIR}\data\config\raceman\singleevent-endurance.xml"

  ; Moved to 'speed-dreams-wip-cars-and-tracks.nsi' for version 2.1.0
  ;File "${BUILD_INST_DIR}\data\config\raceman\networkrace.xml"

  SetOutPath "$INSTDIR\data\data"
  File /r "${BUILD_INST_DIR}\data\data\*.*"

  ; Documentation
  SetOutPath "$INSTDIR\doc"
  File /r /x .svn "${BUILD_INST_DIR}\doc\*.*"

  ; Base robots binaries
  SetOutPath "$INSTDIR\lib\drivers\human"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\human\*.*"

  SetOutPath "$INSTDIR\lib\drivers\assistedhuman"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\assistedhuman\*.*"

  SetOutPath "$INSTDIR\lib\drivers\networkhuman"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\networkhuman\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid_sc\*.*"

  SetOutPath "$INSTDIR\lib\drivers\simplix_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix_sc\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr_sc\*.*"

  SetOutPath "$INSTDIR\lib\drivers\shadow_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow_sc\*.*"

  ; Base robots data files (car and track setups)
  SetOutPath "$INSTDIR\data\drivers\human"
  File /r /x *.lib "${BUILD_INST_DIR}\data\drivers\human\*.*"

  SetOutPath "$INSTDIR\data\drivers\assistedhuman"
  File /r /x *.lib "${BUILD_INST_DIR}\data\drivers\assistedhuman\*.*"

  SetOutPath "$INSTDIR\data\drivers\networkhuman"
  File /r /x *.lib "${BUILD_INST_DIR}\data\drivers\networkhuman\*.*"

  SetOutPath "$INSTDIR\data\drivers\dandroid_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\data\drivers\dandroid_sc\*.*"

  SetOutPath "$INSTDIR\data\drivers\simplix_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\data\drivers\simplix_sc\*.*"

  SetOutPath "$INSTDIR\data\drivers\usr_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\data\drivers\usr_sc\*.*"

  SetOutPath "$INSTDIR\data\drivers\shadow_sc"
  File /r /x *.lib "${BUILD_INST_DIR}\data\drivers\shadow_sc\*.*"

  ; Base car models (only Supercars)
  SetOutPath "$INSTDIR\data\cars\models\sc-boxer-96"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-boxer-96\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-cavallo-360"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-cavallo-360\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-deckard-conejo"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-deckard-conejo\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-fmc-gt4"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-fmc-gt4\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-kanagawa-z35gts"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-kanagawa-z35gts\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-kongei-kk8s"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-kongei-kk8s\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-lynx-220"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-lynx-220\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-murasama-nsx"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-murasama-nsx\*.*"

  SetOutPath "$INSTDIR\data\cars\models\sc-spirit-300"
  File /r /x *.lib "${BUILD_INST_DIR}\data\cars\models\sc-spirit-300\*.*"

  ; Base tracks (only Espie)
  SetOutPath "$INSTDIR\data\tracks\circuit\espie"
  File /r /x *.lib "${BUILD_INST_DIR}\data\tracks\circuit\espie\*.*"

  ; Write the install dir to the registry so that optional installs can find it.
  WriteRegStr   ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "" "$INSTDIR"

SectionEnd

Section /o "Basic mod Tools" SEC02

  SetOverwrite on

  ; Customizable robots binaries
  SetOutPath "$INSTDIR\lib\drivers\simplix"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\simplix\*.*"

  SetOutPath "$INSTDIR\lib\drivers\usr"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\usr\*.*"

  SetOutPath "$INSTDIR\lib\drivers\shadow"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\shadow\*.*"

  SetOutPath "$INSTDIR\lib\drivers\dandroid"
  File /r /x *.lib "${BUILD_INST_DIR}\lib\drivers\dandroid\*.*"

  ; Customizable robot data files (car and track setups)
  SetOutPath "$INSTDIR\data\drivers\simplix"
  File /r "${BUILD_INST_DIR}\data\drivers\simplix\*.*"

  ; Tools executables
  SetOutPath "$INSTDIR\bin"
  File "${BUILD_INST_DIR}\bin\sd2-*.*"
  File "${BUILD_INST_DIR}\bin\*.lib"

  ; Cmake macros for robot developers
  SetOutPath "$INSTDIR\data\cmake"
  File /r "${BUILD_INST_DIR}\data\cmake\*.*"

  ; Includes for robot developers
  SetOutPath "$INSTDIR\include"
  File /r "${BUILD_INST_DIR}\include\*.*"

SectionEnd

Section /o "HQ cars and tracks (1)" SEC03

  StrCpy $1 "speed-dreams-${HQCARSTRACKS_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
  StrCpy $2 "HQ cars and tracks (1)"
  !insertmacro DownloadPackageIfNeededAndThenInstall $1 $2 475

SectionEnd

Section /o "HQ cars and tracks (2)" SEC04

  StrCpy $1 "speed-dreams-${MOREHQCARSTRACKS_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
  StrCpy $2 "HQ cars and tracks (2)"
  !insertmacro DownloadPackageIfNeededAndThenInstall $1 $2 480

SectionEnd

Section /o "WIP cars and tracks" SEC05

  StrCpy $1 "speed-dreams-${WIPCARSTRACKS_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
  StrCpy $2 "WIP cars and tracks"
  !insertmacro DownloadPackageIfNeededAndThenInstall $1 $2 600

SectionEnd

Section /o "Unmaintained" SEC06

  StrCpy $1 "speed-dreams-${UNMAINTAINED_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe"
  StrCpy $2 "Unmaintained contents"
  !insertmacro DownloadPackageIfNeededAndThenInstall $1 $2 1

SectionEnd

Section -Shortcuts

  SetShellVarContext all
  ; Start menu entries
  CreateDirectory "$SMPROGRAMS\${GAME_VERSIONNED_NAME}"

  SetOutPath "$INSTDIR\bin"

  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\${GAME_VERSIONNED_NAME}.lnk" "$INSTDIR\bin\${GAME_FS_NAME}.exe" "" "" 0 SW_SHOWNORMAL "" "an Open Motorsport Sim"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\User Manual.lnk" "$INSTDIR\doc\how_to_drive.html"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\Frequently Asked Questions.lnk" "$INSTDIR\doc\faq.html"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\Read me.lnk" "$INSTDIR\readme.txt"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\Licence.lnk" "$INSTDIR\licence.txt"
  WriteIniStr "$INSTDIR\${GAME_NAME}.url" "InternetShortcut" "URL" "${GAME_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\Web site.lnk" "$INSTDIR\${GAME_NAME}.url"
  WriteIniStr "$INSTDIR\${GAME_NAME} Community.url" "InternetShortcut" "URL" "${COMMUNITY_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\Community.lnk" "$INSTDIR\${GAME_NAME} Community.url"
  WriteIniStr "$INSTDIR\BugTracker.url" "InternetShortcut" "URL" "${GAME_BUG_TRACKER}"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\Bug tracker.lnk" "$INSTDIR\BugTracker.url"
  CreateShortCut "$SMPROGRAMS\${GAME_VERSIONNED_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"

  ; Desktop shortcut
  CreateShortCut "$DESKTOP\${GAME_VERSIONNED_NAME}.lnk" "$INSTDIR\bin\${GAME_FS_NAME}.exe" "" "" 0 SW_SHOWNORMAL "" "an Open Motorsport Sim"

  ; Game shortcuts console settings for more accurate/easy bug reports (120x9999 history, 120x40 window)
  WriteRegDWORD HKCU "Console\${GAME_VERSIONNED_NAME}" "ScreenBufferSize" 0x270f0078
  WriteRegDWORD HKCU "Console\${GAME_VERSIONNED_NAME}" "WindowSize" 0x280078
  WriteRegDWORD HKCU "Console\${GAME_VERSIONNED_NAME}" "QuickEdit" 0x1


SectionEnd

Section -Post

  WriteUninstaller "$INSTDIR\uninst.exe"

  ; Standard Windows-conformant (?) registry keys
  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_INST_REGKEY}" "" "$INSTDIR\bin\${GAME_FS_NAME}.exe"
  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_INST_REGKEY}" "Path" "$INSTDIR\bin"

  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}" "DisplayVersion" "${GAME_VERSION}"
  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}" "DisplayIcon" "$INSTDIR\bin\${GAME_FS_NAME}.exe"
  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}" "URLInfoAbout" "${GAME_WEB_SITE}"
  WriteRegStr ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}" "Publisher" "${GAME_PUBLISHER}"

  ; Registry keys for detecting already installed older/newer version
  ; and other secondary installers.
  WriteRegStr   ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "" "$INSTDIR"
  WriteRegDword ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionMajor" "${VER_MAJOR}"
  WriteRegDword ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionMinor" "${VER_MINOR}"
  WriteRegDword ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionPatch" "${VER_PATCH}"
  WriteRegStr   ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionExtra" "${VER_EXTRA}"
  WriteRegDword ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionDate" "${VER_RELDATE}"

SectionEnd

; Description of the installer sections (for the "Components" page)
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN

  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Game core components, Supercars set, AI drivers, and Espie track$\nA kind of minimal and / or demo package"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Basic tools for building / moding cars, tracks, robots, menus."
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "HQ tracks, fine-tuned LS1 + 36GP cars and AI drivers$\n(will be downloaded if needed and agreed)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "More HQ tracks, fine-tuned TRB1 cars and AI drivers$\n(will be downloaded if needed and agreed)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC05} "Work-in-progress cars (LS2, RS, MP5) and tracks$\n(will be downloaded if needed and agreed)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC06} "Unmaintained contents (Old physics engines)$\n(will be downloaded if needed and agreed)"

!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section Uninstall

   SetShellVarContext all
  ;SetAutoClose true

  ; Start menu entries.
  RMDir /r "$SMPROGRAMS\${GAME_VERSIONNED_NAME}"

  ; Desktop shortcut.
  Delete "$DESKTOP\${GAME_VERSIONNED_NAME}.lnk"

  ; Registry keys.
  DeleteRegKey ${GAME_ROOT_REGKEY} "${GAME_INST_REGKEY}"
  DeleteRegKey ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}"
  DeleteRegKey ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}"

  ; The whole installation folder
  ; (Need to be do that way - inside first, outside last -
  ;  because when the uninstaller is called by LeaveReinstallPage,
  ;  $INSTDIR\uninst.exe is in the way, and  RMDir /r "$INSTDIR" fails).
  RMDir /r "$INSTDIR\bin"
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\data"
  RMDir /r "$INSTDIR\doc"
  RMDir /r "$INSTDIR\include"
  RMDir /r "$INSTDIR"

SectionEnd

;----------------------------------------------------------------------------
;Installer Functions

; This one is called at installer startup.
Function .onInit

  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "speed-dreams.ini"

FunctionEnd

; This one is called when entering the "Reinstall" page,
; in order to check if another older/newer version is already installed,
; and ask the user what he really wants to do.
Function EnterReinstallPage

  ReadRegStr $R0 ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" ""

  StrCmp $R0 "" 0 other_version_present
    Abort

  other_version_present:

  ;Detect version
    ReadRegDWORD $R0 ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionMajor"
    IntCmp $R0 ${VER_MAJOR} minor_check older_version newer_version
  minor_check:
    ReadRegDWORD $R0 ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionMinor"
    IntCmp $R0 ${VER_MINOR} patch_check older_version newer_version
  patch_check:
    ReadRegDWORD $R0 ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionPatch"
    IntCmp $R0 ${VER_PATCH} build_check older_version newer_version
  build_check:
    ReadRegDWORD $R0 ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" "VersionDate"
    IntCmp $R0 ${VER_RELDATE} same_version older_version newer_version

  older_version:

    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 1" "Text" "An older version of ${GAME_NAME} is installed on your system. It is recommended that you uninstall the current version before installing. Select the operation you want to perform and click Next to continue."
    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 2" "Text" "Un-install before installing"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 3" "Text" "Do not un-install"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install ${GAME_VERSIONNED_NAME}."
    StrCpy $R0 "1"

    Goto reinst_start

  newer_version:

    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 1" "Text" "A newer version of ${GAME_NAME} is already installed! It is not recommended that you install an older version. If you really want to install this older version, it's better to uninstall the current version first. Select the operation you want to perform and click Next to continue."
    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 2" "Text" "Un-install before installing"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 3" "Text" "Do not un-install"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose how you want to install ${GAME_VERSIONNED_NAME}."
    StrCpy $R0 "1"

    Goto reinst_start

  same_version:

    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 1" "Text" "${GAME_VERSIONNED_NAME} is already installed. Select the operation you want to perform and click Next to continue."
    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 2" "Text" "Add / re-install components"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "speed-dreams.ini" "Field 3" "Text" "Un-install ${GAME_VERSIONNED_NAME}"
    !insertmacro MUI_HEADER_TEXT "Already Installed" "Choose the maintenance option to perform."
    StrCpy $R0 "2"

  reinst_start:

    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "speed-dreams.ini"

FunctionEnd

; This one is called when leaving the "Reinstall" page,
; after the user has answered what he really wanted to do.
; Here we do what the user decided.
Function LeaveReinstallPage

  !insertmacro MUI_INSTALLOPTIONS_READ $R1 "speed-dreams.ini" "Field 2" "State"

  StrCmp $R0 "1" 0 +2
    StrCmp $R1 "1" reinst_uninstall reinst_done

  StrCmp $R0 "2" 0 +3
    StrCmp $R1 "1" reinst_done reinst_uninstall

  reinst_uninstall:
  ReadRegStr $R1 ${GAME_ROOT_REGKEY} "${GAME_UNINST_REGKEY}" "UninstallString"

  ;Run uninstaller
  HideWindow

    ClearErrors
    ExecWait '$R1 _?=$INSTDIR'

    IfErrors no_remove_uninstaller
    IfFileExists "$INSTDIR\bin\${GAME_FS_NAME}.exe" no_remove_uninstaller

      Delete $R1
      RMDir $INSTDIR

    no_remove_uninstaller:

  StrCmp $R0 "2" 0 +2
    Quit

  BringToFront

  reinst_done:

FunctionEnd

; This one is called at uninstaller startup.
Function un.onInit

  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you really want to uninstall $(^Name) and all its components ?" IDYES +2
  Abort

FunctionEnd

; This one is called at uninstaller end, when it was successfull.
Function un.onUninstSuccess

  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully uninstalled."

FunctionEnd

