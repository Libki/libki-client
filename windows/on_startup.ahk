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

;RegWrite, REG_SZ, HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\WinLogon, Shell, Explorer.exe
ctt := "Windows Registry Editor Version 5.00`n`n[HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\WinLogon]`n""Shell""=""Explorer.exe"""
FileAppend, %ctt%, %filename%
Run, regedit.exe /s %filename%

Clipboard := filename

DisableTaskBar()
HideDesktopIcons()
DisableStartButton()
DisableCtrlAltDel()

if ( GetSetting( "disableInput" ) ) {
    BlockInput MouseMove
    Sleep 1000 * GetSetting( "disableInput" )
    BlockInput MouseMoveOff
}

;Disable Ctrl-Escape to bring up start menu
^Escape::Return

;Disable Shift-F10
+F10::Return

;Disablt Ctrl-Shift-Esc
^+Escape::Return

;Disable Apps key
AppsKey::Return

;Disable Alt-F4
!F4::Return

; Disable Alt+Tab
!Tab::Return

; Disable Windows Key + Tab
#Tab::Return

; Disable Left Windows Key
$LWin::Return

; Disable Right Windows Key
$RWin::Return

;Run taskkill /im explorer.exe /f

ExitApp
