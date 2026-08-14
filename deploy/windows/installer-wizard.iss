#define AppVer GetFileVersion('libkiclient.exe')
#define ProfileGuid '5857eb3f-b8d0-42c5-8cdd-c6910a13f317'

[Setup]
AppName=Libki Kiosk Management System Client
AppVersion={#AppVer}
AppPublisher=Kyle M Hall
AppPublisherURL=http://kylehall.info/
AppSupportURL=http://libki.org/
AppUpdatesURL=http://libki.org/
DefaultDirName={pf}\Libki
DefaultGroupName=Libki Client
OutputBaseFilename=Libki_Client_Installer
Compression=lzma
AllowNoIcons=yes

[Files]
Source: "libkiclient.exe"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "LibkiPrintDialog.exe"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0

Source: "windows\on_login.exe"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "windows\on_logout.exe"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "windows\on_startup.exe"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0

Source: "clawPDF\clawPDF4Libki.ini"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0

Source: "C:\Qt\5.5\mingw492_32\plugins\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion; MinVersion: 0.0,5.0

Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Multimedia.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5MultimediaWidgets.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5OpenGL.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Positioning.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5PrintSupport.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Qml.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Quick.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Script.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Sensors.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Sql.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5WebChannel.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5WebKit.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5WebKitWidgets.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\icudt54.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\icuin54.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\icuuc54.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\5.5\mingw492_32\bin\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\Tools\mingw492_32\opt\bin\libeay32.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "C:\Qt\Tools\mingw492_32\opt\bin\ssleay32.dll"; DestDir: "{app}"; Flags: ignoreversion; MinVersion: 0.0,5.0

[Registry]
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueType: String; ValueName: "Libki"; ValueData: "{app}\libkiclient.exe"; Flags: createvalueifdoesntexist uninsdeletekey; MinVersion: 0.0,5.0; Check: CheckStartAfterShell
Root: "HKLM32"; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Winlogon"; ValueType: String; ValueName: "Shell"; ValueData: "{app}\libkiclient.exe"; MinVersion: 0.0,5.0; Check: CheckShellReplacement and not isWin64
Root: "HKLM64"; Subkey: "Software\Microsoft\Windows NT\CurrentVersion\Winlogon"; ValueType: String; ValueName: "Shell"; ValueData: "{app}\libkiclient.exe"; MinVersion: 0.0,5.0; Check: CheckShellReplacement and IsWin64
Root: "HKLM"; Subkey: "Software\Microsoft\Windows\CurrentVersion\Explorer\Serialize"; ValueType: Dword; ValueName: "StartupDelayInMSec"; ValueData: "0"; MinVersion: 0.0,5.0;

[CustomMessages]
NameAndVersion=%1 version %2
AdditionalIcons=Additional icons:
CreateDesktopIcon=Create a &desktop icon
CreateQuickLaunchIcon=Create a &Quick Launch icon
ProgramOnTheWeb=%1 on the Web
UninstallProgram=Uninstall %1
LaunchProgram=Launch %1
AssocFileExtension=&Associate %1 with the %2 file extension
AssocingFileExtension=Associating %1 with the %2 file extension...

[INI]
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "server"; Key: "host"; String: "{code:GetHost}"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "server"; Key: "port"; String: "{code:GetPort}"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "server"; Key: "scheme"; String: "{code:GetScheme}"

Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "windows"; Key: "EnableStartButton"; String: "1"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "start_user_shell"; String: "C:\Windows\explorer.exe"; Check: CheckShellReplacement
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "logoutAction"; String: "{code:GetLogoutAction}"
;logout, reboot, noaction
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "onlyStopFor"; String: "{code:GetOnlyStopFor}"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "onlyRunFor"; String: "{code:GetOnlyRunFor}"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "location"; String: "{code:GetLocation}"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "password"; String: "{code:GetPassword}"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "username"; String: "Username or library card number"
Filename: "{commonappdata}\Libki\Libki Kiosk Management System.ini"; Section: "node"; Key: "name"; String: "{code:GetNodeName}"

[Code]
var
  ServerPage: TInputQueryWizardPage;
  ClientPage: TInputQueryWizardPage;
  StartupModePage: TInputOptionWizardPage;
  RebootActionPage: TInputOptionWizardPage;
  PasswordPage: TInputQueryWizardPage;
  PrintersPage: TWizardPage;
  PrintersMemo: TNewMemo;
  IgnoreFile: boolean;
  PrintersExisting: TArrayOfString;

function FirstSubstring(const Name: String; const Sep: String): String;
var
  Count, i: Integer;
  s: String;
begin
  Count := 0;
  for i := 1 to Length(Name) do
  begin
    if CompareText(Name[i], Sep) = 0 then
      break;
    Inc(Count);
  end;
  s := Copy(Name, 1, Count);
  Result := s;
end;

function HasCommandLineSwitch(const Name: String): Boolean;
var
  Comp: String;
  I: Integer;
begin
  Result := False;
  for I := 1 to ParamCount do
  begin
    Comp := FirstSubstring(ParamStr(I), '=');
    if CompareText(Comp, '/' + Name) = 0 then
    begin
      Result := True;
      Break;
    end;
  end;
end;

function CreateMemoString(const Strings: TArrayOfString; const Memo: TMemo): Boolean;
var
  i: Integer;
begin
  Result := False;
  for i := low(Strings) to high(Strings) do
  begin
    Memo.Lines.Append(Strings[i]);
  end;
  Result := True;
end;

function ParseExistingPrinters(Name: string): TArrayOfString;
var
  Line, A: String;
  Count, i: Integer;
  Config: TArrayOfString;
  InSection: Boolean;
begin
  Count := 0;
  InSection := False;
  SetArrayLength(Result, 0);
  if LoadStringsFromFile(Name, Config) then begin
    for i := low(Config) to high(Config) do
    begin
      Line := Trim(Config[i])
      if Length(Line) = 0 then
        continue;
      if CompareText(Line[1], '[') = 0 then
        InSection := False;
      if InSection then
      begin
        A := FirstSubstring(Line, '=');
        if Trim(A) <> '' then
        begin
          SetArrayLength(Result, Count + 1);
          Result[Count] := Trim(A);
          Inc(Count);
        end;
      end;
      if CompareText(Line, '[printers]') = 0 then
        InSection := True;
    end;
  end;
end;
  

procedure InitializeWizard;
var
  IniPath: String;
begin
  IniPath := ExpandConstant('{commonappdata}\Libki\Libki Kiosk Management System.ini');
  { Create the pages }
  
  ServerPage := CreateInputQueryPage(wpWelcome,
    'Server Information', 'Libki server data',
    'Please specify the Libki server data.');
  ServerPage.Add('Scheme:', False);
  ServerPage.Add('Host:', False);
  ServerPage.Add('Port:', False);

  ClientPage := CreateInputQueryPage(ServerPage.ID,
    'Client Information', 'Libki client data',
    'Please specify the Libki client data.');
  ClientPage.Add('Location:', False);
  ClientPage.Add('Run only for this user:', False);
  ClientPage.Add('Run for all users but this one:', False);
  ClientPage.Add('Client name:', False);

  StartupModePage := CreateInputOptionPage(ClientPage.ID,
    'Startup mode', 'Specify how to start the client',
    'Please specify how the Libki client should be started',
    True, False);
  StartupModePage.Add('Automatically start Libki client after normal user shell');
  StartupModePage.Add('Automatically start Libki instead of user shell (shell replacement)');
  StartupModePage.Add('Do not start Libki client automatically');

  RebootActionPage := CreateInputOptionPage(StartupModePage.ID,
    'Logout Action', 'Specify action on logout?',
    'Please specify how the Libki client should act on logout.',
    True, False);
  RebootActionPage.Add('Reboot (best for Deep Freeze)');
  RebootActionPage.Add('Log out of operating system (best for Clean Slate)');
  RebootActionPage.Add('Nothing (just redisplay the Libki login screen)');

  PasswordPage := CreateInputQueryPage(RebootActionPage.ID,
    'Client Disable', 'Libki client disabling password',
    'Please specify the password for disabling the Libki client.');
  PasswordPage.Add('Password:', True);
  
  PrintersPage := CreateCustomPage(
    PasswordPage.ID,
    'Printer Configuration',
    'List the printers you wish to install on this Client, one per line, or leave blank to skip printer installation'
  );

  PrintersMemo := TNewMemo.Create(PrintersPage);
  PrintersMemo.Parent := PrintersPage.Surface;
  PrintersMemo.Left := ScaleX(0);
  PrintersMemo.Top := ScaleY(0);
  PrintersMemo.Width := PrintersPage.SurfaceWidth;
  PrintersMemo.Height := PrintersPage.SurfaceHeight;
  PrintersMemo.ScrollBars := ssVertical;
  PrintersMemo.WordWrap := False;
  PrintersMemo.WantReturns := True;

  { Set default values, using settings that were stored last time if possible }
  { Test if the ignore file param is present }
  IgnoreFile := HasCommandLineSwitch('ignorefile');

  { Read command line parameters and set them as default values for installer UI pages. }
  { Unless ignore file is present, read existing values from the ini. }
  if HasCommandLineSwitch('scheme') then
    ServerPage.Values[0] := ExpandConstant('{param:scheme}')
  else if not IgnoreFile then
    ServerPage.Values[0] := GetIniString('server', 'scheme', '', IniPath);

  if HasCommandLineSwitch('host') then
    ServerPage.Values[1] := ExpandConstant('{param:host}')
  else if not IgnoreFile then
    ServerPage.Values[1] := GetIniString('server', 'host', '', IniPath);

  if HasCommandLineSwitch('port') then
    ServerPage.Values[2] := ExpandConstant('{param:port}')
  else if not IgnoreFile then
    ServerPage.Values[2] := GetIniString('server', 'port', '', IniPath);

  if HasCommandLineSwitch('location') then
    ClientPage.Values[0] := ExpandConstant('{param:location}')
  else if not IgnoreFile then
    ClientPage.Values[0] := GetIniString('node', 'location', '', IniPath);

  if HasCommandLineSwitch('runonly') then
    ClientPage.Values[1] := ExpandConstant('{param:runonly}')
  else if not IgnoreFile then
    ClientPage.Values[1] := GetIniString('node', 'onlyRunFor', '', IniPath);
    
  if HasCommandLineSwitch('stoponly') then
    ClientPage.Values[2] := ExpandConstant('{param:stoponly}')
  else if not IgnoreFile then
    ClientPage.Values[2] := GetIniString('node', 'onlyStopFor', '', IniPath);
    
  if HasCommandLineSwitch('nodename') then
    ClientPage.Values[3] := ExpandConstant('{param:nodename}')
  else if not IgnoreFile then
    ClientPage.Values[3] := GetIniString('node', 'name', '', IniPath);
  
  if HasCommandLineSwitch('password') then
    PasswordPage.Values[0] := ExpandConstant('{param:password}');

  { Parse RebootAction string. Can be one of 'reboot', 'logout', or 'none'. Defaults to 'reboot' }
  if HasCommandLineSwitch('rebootaction') then begin
    if CompareText(ExpandConstant('{param:rebootaction}'), 'logout') = 0 then
      RebootActionPage.SelectedValueIndex := 1
    else if CompareText(ExpandConstant('{param:rebootaction}'), 'none') = 0 then
      RebootActionPage.SelectedValueIndex := 2
    else
      RebootActionPage.SelectedValueIndex := 0;
  end
  else if not IgnoreFile then begin
    if CompareText(GetIniString('node', 'logoutAction', '', IniPath), 'logout') = 0 then
      RebootActionPage.SelectedValueIndex := 1
    else if CompareText(GetIniString('node', 'logoutAction', '', IniPath), 'no_action') = 0 then
      RebootActionPage.SelectedValueIndex := 2
    else
      RebootActionPage.SelectedValueIndex := 0;
  end;

  { Parse StartupMode string. Can be one of 'normal', 'shell', or 'none'. Defaults to 'normal' }
  if HasCommandLineSwitch('startupmode') then begin
    if CompareText(ExpandConstant('{param:startupmode}'), 'shell') = 0 then
      StartupModePage.SelectedValueIndex := 1
    else if CompareText(ExpandConstant('{param:startupmode}'), 'none') = 0 then
      StartupModePage.SelectedValueIndex := 2
    else
      StartupModePage.SelectedValueIndex := 0;
  end
  else
    StartupModePage.SelectedValueIndex := 0;
  end;
  
  if not IgnoreFile then begin 
    PrintersExisting := ParseExistingPrinters(IniPath);
    if (GetArrayLength(PrintersExisting) > 0) then begin
      CreateMemoString(PrintersExisting, PrintersMemo);
    end;
  end;
end;

function GetScheme(Param: String): String;
begin
  Result := ServerPage.Values[0];
end;

function GetHost(Param: String): String;
begin
  Result := ServerPage.Values[1];
end;

function GetPort(Param: String): String;
begin
  Result := ServerPage.Values[2];
end;

function GetLocation(Param: String): String;
begin
  Result := ClientPage.Values[0];
end;

function GetOnlyRunFor(Param: String): String;
begin
  Result := ClientPage.Values[1];
end;

function GetOnlyStopFor(Param: String): String;
begin
  Result := ClientPage.Values[2];
end;

function GetNodeName(Param: String): String;
begin
  Result := ClientPage.Values[3];
end;

function GetLogoutAction(Param: String): String;
begin
  case RebootActionPage.SelectedValueIndex of
    0: Result := 'reboot';
    1: Result := 'logout';
    2: Result := 'no_action';
  end;
end;

function GetPassword(Param: String): String;
var
  IniPath, Original: String;
begin
  Result := '';
  IniPath := ExpandConstant('{commonappdata}\Libki\Libki Kiosk Management System.ini');
  Original := GetIniString('node', 'password', '', IniPath);
  if Original <> '' then
    Result := Original;
  if PasswordPage.Values[0] <> '' then
    Result := GetMD5OfString( PasswordPage.Values[0] );
end;

function CheckStartAfterShell(): Boolean;
begin
  Result := (StartupModePage.SelectedValueIndex = 0);
end;

function CheckShellReplacement(): Boolean;
begin
  Result := (StartupModePage.SelectedValueIndex = 1);
end;

procedure GetPrinterList(var Printers: TArrayOfString);
var
  i, Count: Integer;
  Line: String;
begin
  Count := 0;
  SetArrayLength(Printers, 0);

  for i := 0 to PrintersMemo.Lines.Count - 1 do
  begin
    Line := Trim(PrintersMemo.Lines[i]);
    if Line <> '' then
    begin
      SetArrayLength(Printers, Count + 1);
      Printers[Count] := Line;
      Inc(Count);
    end;
  end;
end;

{ Post-install logic: create folders, update INI, install clawPDF, import config }
procedure CurStepChanged(CurStep: TSetupStep);
var
  i: Integer;
  PrinterName: String;
  IniPath, ClawPDFIni, ClawPDFExe, SetupHelperExe: String;
  HasPrinters: Boolean;
  ResultCode: Integer;
  Printers: TArrayOfString;
begin
  if CurStep = ssPostInstall then
  begin
    HasPrinters := False;

    for i := 0 to PrintersMemo.Lines.Count - 1 do
    begin
      if Trim(PrintersMemo.Lines[i]) <> '' then
      begin
        HasPrinters := True;
        Break;
      end;
    end;
    if not HasPrinters then Exit;

    { Configure ClawPDF }
    ClawPDFExe := ExpandConstant('{pf}\clawpdf\clawPDF.exe');
    SetupHelperExe := ExpandConstant('{pf}\clawpdf\SetupHelper.exe');
    ClawPDFIni := ExpandConstant('{app}\windows\clawPDF4Libki.ini');
 
    if FileExists(ClawPDFExe) and FileExists(SetupHelperExe) then
    begin
      ForceDirectories('C:\printers\clawpdf');
      GetPrinterList(Printers);

      { Update ClawPDFIni with printer info }
      for i := 0 to PrintersMemo.Lines.Count - 1 do
      begin
        PrinterName := Trim(PrintersMemo.Lines[i]);
        if PrinterName <> '' then
        begin
          SetIniString('ApplicationSettings\PrinterMappings\' + IntToStr(i), 'PrinterName', PrinterName, ClawPDFIni);
          SetIniString('ApplicationSettings\PrinterMappings\' + IntToStr(i), 'ProfileGuid', '{#ProfileGuid}', ClawPDFIni);
          Exec(SetupHelperExe, '/Printer=Add /Name="' + PrinterName + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
        end;
      end;
      SetIniString('ApplicationSettings\PrinterMappings', 'numClasses', IntToStr(PrintersMemo.Lines.Count), ClawPDFIni);
      SetIniString('ApplicationSettings', 'PrimaryPrinter', Trim(PrintersMemo.Lines[0]), ClawPDFIni);
      SetIniString('ApplicationSettings', 'LastUsedProfileGuid', '{#ProfileGuid}', ClawPDFIni);
      
      { apply configs }
      if Exec(ClawPDFExe, '/Config="' + ClawPDFIni + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
      begin
        { configs ok }
      end
      else
      begin
        MsgBox('Could not import configs', mbError, MB_OK);
      end
    end
    else
    begin
      MsgBox(
        'clawPDF executable not found at ' + ClawPDFExe,
        mbError,
        MB_OK
      );
    end;
  end;
end;

