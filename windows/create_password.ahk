;
; AutoHotkey Version: 1.x
; Language:       English
; Platform:       Win9x/NT
; Author:         Kyle M Hall <kyle.m.hall@gmail.com>
;
; Script Function:
;	This script creates the password file for the password protected keylock scripts
;	The file needs to be moved to the same directory as the client's ini file.
;

#Include functions.ahk

#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent starting directory.

Envget, vAPPDATA, APPDATA

InputBox, UserInput, Create Libki Unlocker Password, Enter Password, hide 
UserHash := HashPassword( UserInput )
FileDelete, %vAPPDATA%\Libki\keylock
FileAppend, %UserHash%, %vAPPDATA%\Libki\keylock
