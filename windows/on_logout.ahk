;
; AutoHotkey Version: 1.x
; Language:       English
; Platform:       Win9x/NT
; Author:         Kyle Hall <kyle.m.hall@gmail.com>
;
; Script Function:
;	This script is launched by the Libki client when a user logs into the client
;   on windows to disable some features that were enabled by the on_login script.
;
;
#SingleInstance force
#NoTrayIcon

#Include functions.ahk

DisableStartButton()
DisableTaskBar()
HideDesktopIcons()
DisableCtrlAltDel()