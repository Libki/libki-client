;
; AutoHotkey Version: 1.x
; Language:       English
; Platform:       Win9x/NT
; Author:         Kyle Hall <kyle.m.hall@gmail.com>
;
; Script Function:
;	This script is launched by the Libki client when a user logs into the client
;   on windows to re-enable some features that were disabled by the on_startup script.
;
#SingleInstance force
#NoTrayIcon
;#Persistent
;#NoEnv
;SetBatchLines, -1
OnExit("RestoreRegistry")

#Include functions.ahk

ShowDesktopIcons()
EnableTaskBar()

if ( GetSetting( "EnableStartButton" ) ) {
	EnableStartButton()
} else {
	DisableStartButton()
}

RegRead, originalShell, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon, Shell
if (ErrorLevel) {
    MsgBox, Error reading the registry key!
    ExitApp
}

; Change Shell to Explorer.exe
RegWrite, REG_SZ, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon, Shell, Explorer.exe
if (ErrorLevel) {
    MsgBox, Error writing to registry!
    ExitApp
}

; Start Explorer
Run, explorer.exe
Sleep, 1000  ; Give Explorer time to start

RestoreRegistry() ; Restore original registry value immediately

ExitApp

RestoreRegistry() {
    global originalShell
    RegWrite, REG_SZ, HKEY_LOCAL_MACHINE, SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon, Shell, %originalShell%
}
