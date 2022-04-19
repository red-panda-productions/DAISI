;
;        Common definitions for all the Speed Dreams installers
;
;        copyright  : (C) 2011 onwards Jean-Philippe Meuret
;        $Id: speed-dreams.nsh 7524 2021-05-24 12:20:38Z torcs-ng $
;
;        This program is free software; you can redistribute it and/or modify
;        it under the terms of the GNU General Public License as published by
;        the Free Software Foundation; either version 2 of the License, or
;        (at your option) any later version.

!ifndef VER_MAJOR
   !define VER_MAJOR 2
!endif

!ifndef VER_MINOR
   !define VER_MINOR 2
!endif

!ifndef VER_PATCH
   !define VER_PATCH 3
!endif

!ifndef VER_EXTRA
   !define VER_EXTRA ""
!endif

;!echo "VER_EXTRA = ${VER_EXTRA}"

!ifndef VER_REVISION
   !define VER_REVISION ""
!endif

; Create TODAY variable containing today's date in the format we use
!define /date TODAY "%Y%m%d"

!ifndef VER_RELDATE
   !define VER_RELDATE ${TODAY} ; Initial guess ;-)
   ;!define VER_RELDATE 20131102 ; Initial guess ;-)
!endif

!ifndef OUT_PATH
   !define OUT_PATH "."
!endif

!define GAME_SHORT_NAME "Speed Dreams"
!define GAME_NAME "${GAME_SHORT_NAME} ${VER_MAJOR}"
!define GAME_SHORT_FS_NAME "speed-dreams"
!define GAME_FS_NAME "${GAME_SHORT_FS_NAME}-${VER_MAJOR}"
!define GAME_SUBTITLE "'an Open Motorsport Sim'"
!define GAME_VERSION "${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}${VER_EXTRA}"
!define GAME_VERSIONNED_NAME "${GAME_SHORT_NAME} ${GAME_VERSION}"
!define GAME_LONG_VERSION "${GAME_VERSION}-${VER_REVISION}"
!define GAME_LONG_NAME "${GAME_SHORT_NAME} ${GAME_LONG_VERSION}"
!define GAME_PUBLISHER "The Speed Dreams Team"
!define GAME_WEB_SITE "http://www.speed-dreams.org"
!define GAME_BUG_TRACKER "http://sourceforge.net/p/speed-dreams/tickets/"
!define GAME_DOWNLOAD_URL "http://sourceforge.net/projects/speed-dreams/files/${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}"
!define COMMUNITY_WEB_SITE "https://sourceforge.net/p/speed-dreams/discussion/"

!define GAME_ROOT_REGKEY "HKLM"
!define GAME_INST_REGKEY "Software\Microsoft\Windows\CurrentVersion\${GAME_FS_NAME}.exe"
!define GAME_UNINST_REGKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GAME_NAME}"
!define GAME_DIR_REGKEY "Software\${GAME_PUBLISHER}\${GAME_NAME}"

; Folder where the build system installed everything
!ifndef BUILD_INST_DIR
   !define BUILD_INST_DIR "\${GAME_FS_NAME}-build"
!endif

; Default target installation folder
!define INST_INST_DIR_SUFFIX "${GAME_SHORT_FS_NAME}-${GAME_VERSION}"

; Core name for each installer
!define BASE_SETUP_KEY "base"
!define HQCARSTRACKS_SETUP_KEY "hq-cars-and-tracks"
!define MOREHQCARSTRACKS_SETUP_KEY "more-hq-cars-and-tracks"
!define WIPCARSTRACKS_SETUP_KEY "wip-cars-and-tracks"
!define UNMAINTAINED_SETUP_KEY "unmaintained"

; Common installer settings
SetCompressor /SOLID lzma ; 1-block compression, smaller but longer (generation, extraction)
;SetCompressor lzma ; A bit bigger, but faster generation and extraction

; Usefull libs.
!include "LogicLib.nsh"

; Common MUI Settings
!include "MUI.nsh" ; TODO: Move to MUI2

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "..\..\data\data\img\header.bmp"

!define MUI_WELCOMEFINISHPAGE_BITMAP "..\..\data\data\img\header-vert.bmp"

!define MUI_ABORTWARNING

!define MUI_ICON "..\..\data\data\icons\icon.ico"
!define MUI_UNICON "..\..\data\data\icons\icon.ico"

!define MUI_LICENSEPAGE_CHECKBOX

; StrLoc function ===========================================================
; This function outputs the relative index position from the start or end of a string,
; where a substring is located.
; Adapted from StrLoc 2.0 by Diego Pedroso (http://nsis.sourceforge.net/StrLoc)

!define StrLoc "!insertmacro _StrLoc"
 
!macro _StrLoc Result String SubString StartPoint
  Push `${String}`
  Push `${SubString}`
  Push `${StartPoint}`
  Call StrLoc
  Pop `${Result}`
!macroend
 
Function StrLoc

  ;Get arguments from the stack
  Exch $R0 ; R0 : StartPoint
  Exch 1
  Exch $R1 ; R1 : SubString
  Exch 2
  Exch $R2 ; R2 : String
  Push $R3
  Push $R4
  Push $R5
  Push $R6
 
  ;Get "String" and "SubString" length
  StrLen $R3 $R1 ; R3 : SubStringLen
  StrLen $R4 $R2 ; R4 : StringLen

  ;Start "StartCharPos" counter
  StrCpy $R5 0 ; R5 : StartCharPos
 
  ;Loop until "SubString" is found or "String" reaches its end
  ${Do}

    ;Remove everything before and after the searched part ("TempStr")
    StrCpy $R6 $R2 $R3 $R5 ; R6 : TmpString
 
    ;Compare "TempStr" with "SubString"
    ${If} $R6 == $R1
      ${If} $R0 == "<"
        IntOp $R6 $R3 + $R5
        IntOp $R0 $R4 - $R6
      ${Else}
        StrCpy $R0 $R5
      ${EndIf}
      ${ExitDo}
    ${EndIf}
    ;If not "SubString", this could be "String"'s end
    ${If} $R5 >= $R4
      StrCpy $R0 ""
      ${ExitDo}
    ${EndIf}
    ;If not, continue the loop
    IntOp $R5 $R5 + 1
  ${Loop}
 
  ;Return
  Pop $R6
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Exch
  Pop $R1
  Exch $R0

FunctionEnd

; Parse the given SF.net download page ========================================
; and retrieve the URL of the target file on the default (auto-select) mirror.
!define GetAutoSelectMirrorURL "!insertmacro _GetAutoSelectMirrorURL"
 
!macro _GetAutoSelectMirrorURL ResultURL HTMLFile
  Push "${HTMLFile}"
  Call GetAutoSelectMirrorURL
  Pop "${ResultURL}"
!macroend
 
Function GetAutoSelectMirrorURL

  ;Get arguments from the stack
  Exch $R0 ; R0 : HTMLFile
  Push $R1
  Push $R2
  Push $R3
  Push $R4
  Push $R5
 
  ;DetailPrint "GetAutoSelectMirrorURL:" 
  ;DetailPrint "  HTMLFile='$R0' ..." 

  StrCpy $R2 "" ; R2 : ResultURL

  ClearErrors
  FileOpen $R1 $R0 r ; R1 : FileHdle
  IfErrors Done
 
  ;DetailPrint "  File open ..." 

  StrCpy $R3 "" ; R3 : Line

  ${Do}

    ClearErrors
    StrCpy $R4 $R3 ; R4 : PrevLine
    FileRead $R1 $R3
    IfErrors DoneRead
 
    ;DetailPrint "  Line read ..." 
    ;DetailPrint "  '$R3'" 

    ${StrLoc} $R5 $R3 "direct link</a>, or try another" 0 ; R5 : StringPos
    ;DetailPrint "Found direct link ? $R5"
    ${If} $R5 != ""
      ${StrLoc} $R5 $R4 "<a href=" 0
      ;DetailPrint "Found href in '$R4' ? $R5"
      ;DetailPrint "(in '$R4')"
      ${If} $R5 != ""
        ${StrLoc} $R5 $R4 '"' 0 ; First " in '<a href="..."'
        ;DetailPrint "Found 1st $\" ? $R5"
        ;DetailPrint "(in '$R4')"
        IntOp $R5 $R5 + 1
        StrCpy $R2 $R4 200 $R5
        ${StrLoc} $R5 $R2 '"' 0 ;  Last " in '<a href="..."'
        ;DetailPrint "Found 1st $\" ? $R5"
        ;DetailPrint "(in '$R2')"
        StrCpy $R2 $R2 $R5
        ;DetailPrint "Result='$R2'"
        ${ExitDo}
      ${EndIf}
    ${EndIf}

  ${Loop}

  DoneRead:
    FileClose $R1

  Done:

  ; Return
  StrCpy $R0 $R2 ; R0 : ResultURL
  Pop $R5
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0 ;Mirror URL found (empty if not found)

FunctionEnd

; Download a file from SF.net using the default mirror =========================
; ${Status} = "success" on success, or else "<error message>"
!define DownloadFromSFNet "!insertmacro _DownloadFromSFNet"
 
!macro _DownloadFromSFNet StatusVar SourceURL TargetPathName
  Push "${SourceURL}"
  Push "${TargetPathName}"
  Call DownloadFromSFNet
  Pop "${StatusVar}"
!macroend
 
Function DownloadFromSFNet

  ;Get arguments from the stack
  Exch $R0 ; R0 : TargetPathName
  Exch 1
  Exch $R1 ; R1 : SourceURL
  Push $R2
  Push $R3
  Push $R4

  StrCpy $R2 $R0 ; R2 : TargetPathName

  StrCpy $R3 "success" ; R3 : Status

  DetailPrint "1) Downloading default mirror URL (auto-selected by SF.net) ..." 
  inetc::get "$R1" "$R2.html" /end
  Pop $R0
  ${If} $R0 != "OK"
    
    ${If} $R0 == "cancel"
      StrCpy $R3 "Mirror selection page download was cancelled"
      DetailPrint "... cancelled." 
    ${Else}
      StrCpy $R3 "Mirror selection page download failed$\n($R0)"
      DetailPrint "... failed." 
    ${EndIf}
    MessageBox MB_OK|MB_ICONSTOP "$R3"

  ${Else}

    ${GetAutoSelectMirrorURL} $R4 $R2.html ; R4 : RealURL
    ;DetailPrint "  File='$R2.html' ..." 
    ;DetailPrint "  URL='$R4' ..." 

    ${If} $R4 == ""
    
        StrCpy $R3 "Failed to get file URL on default mirror$\nfrom $R1"
        MessageBox MB_OK|MB_ICONSTOP "$R3"
        DetailPrint "... failed to extract default mirror URL." 

    ${Else}

      DetailPrint "2) Downloading file from $R4 ..." 
      inetc::get "$R4" "$R2" /end
      Pop $R0
      ${If} $R0 != "OK"
    
        ${If} $R0 == "cancel"
          StrCpy $R3 "File download was cancelled"
          DetailPrint "... cancelled." 
        ${Else}
          StrCpy $R3 "Failed to download file$\nfrom $R4$\n($R0)"
          DetailPrint "... failed." 
        ${EndIf}
        Delete "$R2"
        MessageBox MB_OK|MB_ICONSTOP "$R3"

      ${Else}

        DetailPrint "3) Done in $R2." 

      ${EndIf}

    ${EndIf}

  ${EndIf}

  Delete "$R2.html"

  ; Return
  StrCpy $R0 $R3 ; R0 : Status
  Pop $R4
  Pop $R3
  Pop $R2
  Pop $R1
  Exch $R0 ; "success" on success, or else "<error message>"

FunctionEnd


!macro DownloadPackageIfNeededAndThenInstall PackageFileName PackageTitle PackageSizeMb

  ; Check if the target installer file is already here, next to the current one.
  ${IfNot} ${FileExists} "$EXEDIR\${PackageFileName}"
      
    ; If not, ask the user if he really wants to download.
    MessageBox MB_OKCANCEL|MB_ICONQUESTION "${PackageFileName} not found$\nnext to $EXEFILE on your disk.$\n$\nClick OK to download it from ${GAME_DOWNLOAD_URL},$\nor Cancel to skip.$\n$\nNote: This might take several minutes, as it's about ${PackageSizeMb} megabytes." IDCANCEL SkipDownload

    ; Download
    DetailPrint "Downloading ${PackageTitle} ..." 
    ${DownloadFromSFNet} $0 "${GAME_DOWNLOAD_URL}/${PackageFileName}" "$EXEDIR\${PackageFileName}"

    ${If} $0 != "success"

      MessageBox MB_OK|MB_ICONSTOP "Skipping installation of ${PackageTitle}." 

      SetAutoClose false

      Return

    ${EndIf}

  ${EndIf}

  DetailPrint "Installing ${PackageTitle} ..." 
  ExecWait '$EXEDIR\${PackageFileName}' $0
  DetailPrint "Done (exit code $0)."
  ${If} $0 != "0"
    SetAutoClose false
  ${EndIf}

  Return

  SkipDownload:

    DetailPrint "Skipping installation of ${PackageTitle} on user's request." 

!macroend

!macro CheckIfBasePackageInstalled

  ; Check if the base package is installed and OK, and abort if not.
  ReadRegStr $R0 ${GAME_ROOT_REGKEY} "${GAME_DIR_REGKEY}" ""
  ${If} $R0 == ""
    MessageBox MB_OK|MB_ICONSTOP "No base package found for ${GAME_NAME}$\n$\nPlease, first download and run ${GAME_SHORT_FS_NAME}-${BASE_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe$\nfrom ${GAME_DOWNLOAD_URL}"
    Abort
  ${EndIf}

  ${If} ${FileExists} "$R0\bin\${GAME_FS_NAME}.exe"
    Nop
  ${Else}
    MessageBox MB_OK|MB_ICONSTOP "A base package for ${GAME_NAME} was found, but it is no more usable and/or corrupted$\n$\nPlease, first (download and) run again ${GAME_SHORT_FS_NAME}-${BASE_SETUP_KEY}-${GAME_LONG_VERSION}-win32-setup.exe$\nfrom ${GAME_DOWNLOAD_URL}"
    Abort
  ${EndIf}

!macroend

