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

#Include functions.ahk

ShowDesktopIcons()
EnableTaskBar()

if ( GetSetting( "EnableStartButton" ) ) {
	EnableStartButton()
} else {
	DisableStartButton()
}

Run cmd.exe /c tasklist /nh /fi "imagename eq explorer.exe" | find /i "explorer.exe" > nul || (start explorer.exe)


ExitApp
