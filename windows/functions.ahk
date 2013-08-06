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

TryUnlock(){
	FileRead, UnlockPwd, %A_AppDataCommon%\Libki\keylock
	if ( UnlockPwd ) {
		DisableTaskBar()
		InputBox, UserInput, Enter Password, Enter Password, hide
		UserHash := HashPassword( UserInput ) 
		if UserHash = %UnlockPwd%
		{
			MsgBox, Computer Is Now Unlocked
			EnableStartButton()
			EnableCtrlAltDel()
			EnableTaskBar()
			ExitApp
		} 
		else
		{
			MsgBox, Password Incorrect
			EnableTaskBar()
			return
		}
	}
}

HashPassword(x)
{
   return CRC32( String2Hex( x ) )
}

CRC32(x)
{
   L := StrLen(x)>>1          ; length in bytes
   StringTrimLeft L, L, 2     ; remove leading 0x
   L = 0000000%L%
   StringRight L, L, 8        ; 8 hex digits
   x = %x%%L%                 ; standard pad
   R =  0xFFFFFFFF            ; initial register value
   Loop Parse, x
   {
      y := "0x" A_LoopField   ; one hex digit at a time
      Loop 4
      {
         R := (R << 1) ^ ( (y << (A_Index+28)) & 0x100000000)
         IfGreater R,0xFFFFFFFF
            R := R ^ 0x104C11DB7
      }
   }
   Return ~R                  ; ones complement is the CRC
}

String2Hex(x)                 ; Convert a string to hex digits
{                             ; needs SetFormat Integer, H
   Loop Parse, x
   {
      y := ASC(A_LoopField)   ; 2 digit ASCII code of chars of x, 15 < y < 256
      StringTrimLeft y, y, 2  ; Remove leading 0x
      hex = %hex%%y%
   }
   Return hex
}