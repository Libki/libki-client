;
; AutoHotkey Version: 1.x
; Language:       English
; Platform:       Win9x/NT
; Author:         Kyle Hall <kyle.m.hall@gmail.com>
;
; Script Function:
;	This script is launched by the Libki client on startup on windows to prevent a user from
;	bypassing the client by opening another window before it runs.
;
#SingleInstance force
#NoTrayIcon

#Include functions.ahk

DisableTaskBar()
HideDesktopIcons()
DisableStartButton()
DisableCtrlAltDel()

;Disable Alt-Tab
!TAB::

;FIXME - Removed, DisableTaskBar makes this uneccessary
;Disable Windows Key
;LWin::
;RWin::
;#::

;Disable Win-L for locking computer
LWin::LCtrl

;Disable Ctrl-Escape to bring up start menu
^Escape::

;Disable Shift-F10
+F10::

;Disablt Ctrl-Shift-Esc
^+Escape::

;Disable Apps key
AppsKey::

;Disable Alt-F4
!F4::return
