DisableCtrlAltDel(){
	Regwrite, REG_SZ, HKEY_LOCAL_MACHINE,SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\taskmgr.exe, Debugger, Hotkey Disabled
}

EnableCtrlAltDel(){
	RegDelete,HKEY_LOCAL_MACHINE,SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\taskmgr.exe
}

DisableStartButton(){
	WinHide,Start ahk_class Button
}

EnableStartButton(){
	WinShow,Start ahk_class Button
}

DisableTaskBar(){
	WinHide,ahk_class Shell_TrayWnd
}

EnableTaskBar(){
	WinShow,ahk_class Shell_TrayWnd
}

HideDesktopIcons(){
	winhide, Program Manager
}

ShowDesktopIcons(){
	winshow, Program Manager
}

GetSetting( key ){
	IniRead, OutputVar, %A_AppDataCommon%\Libki\Libki Kiosk Management System.ini, windows, %key%
	return OutputVar
}
