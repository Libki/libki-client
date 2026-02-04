#define AppVer GetFileVersion('libkiclient.exe')

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

Source: "windows\on_login.exe"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "windows\on_logout.exe"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "windows\on_startup.exe"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0
Source: "windows\clawPDF4Libki.ini"; DestDir: "{app}\windows"; Flags: ignoreversion; MinVersion: 0.0,5.0

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

// Regex helper
function IsYamlSafeKey(const S: String): Boolean;
var
  i: Integer;
  C: Char;
begin
  Result := Length(S) > 0;
  if not Result then Exit;
  // Must start with a letter
  C := S[1];
  if not (C in ['A'..'Z', 'a'..'z']) then
  begin
    Result := False;
    Exit;
  end;
  // Remaining characters must be letters, numbers, underscore, or dash
  for i := 2 to Length(S) do
  begin
    C := S[i];
    if not (C in ['A'..'Z','a'..'z','0'..'9','_','-']) then
    begin
      Result := False;
      Exit;
    end;
  end;
end;

procedure InitializeWizard;
begin
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
  StartupModePage.SelectedValueIndex := 0;

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
    'Printer Setup'
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
end;

function NextButtonClick(CurPageID: Integer): Boolean;
var
  i: Integer;
  Line, InvalidList: String;
begin
  Result := True;

  if CurPageID = PrintersPage.ID then
  begin
    InvalidList := '';

    for i := 0 to PrintersMemo.Lines.Count - 1 do
    begin
      Line := Trim(PrintersMemo.Lines[i]);
      if (Line <> '') and (not IsYamlSafeKey(Line)) then
      begin
        if InvalidList <> '' then InvalidList := InvalidList + ', ';
        InvalidList := InvalidList + Line;
      end;
    end;

    if InvalidList <> '' then
    begin
      MsgBox(
        'The following printer names are invalid for YAML keys:' + #13#10 + InvalidList + #13#10#13#10 + 'Names must start with a letter and contain only letters, numbers, _ or -.',
        mbError, MB_OK
      );
      Result := False;
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
begin
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


// Validate printer names before finishing
function NextButtonClick(CurPageID: Integer): Boolean;
var
  i: Integer;
  Line, InvalidList: String;
begin
  Result := True;

  if CurPageID = PrintersPage.ID then
  begin
    InvalidList := '';
    for i := 0 to PrintersMemo.Lines.Count-1 do
    begin
      Line := Trim(PrintersMemo.Lines[i]);
      if (Line <> '') and (not IsYamlSafeKey(Line)) then
      begin
        if InvalidList <> '' then InvalidList := InvalidList + ', ';
        InvalidList := InvalidList + Line;
      end;
    end;

    if InvalidList <> '' then
    begin
      MsgBox(
        'The following printer names are invalid for YAML keys:' + #13#10 + InvalidList + #13#10#13#10 + 'Names must start with a letter and contain only letters, numbers, _ or -.',
        mbError, MB_OK
      );
      Result := False; // stay on page
    end;
  end;
end;

// Post-install logic: create folders, update INI, install clawPDF, import config
procedure CurStepChanged(CurStep: TSetupStep);
var
  i: Integer;
  PrinterName: String;
  IniPath, ClawPDFIni, ClawPDFExe: String;
  Lines: TArrayOfString;
  HasPrinters, ClawPDFInstalled: Boolean;
  ResultCode: Integer;
  Printers: TArrayOfString;
begin
  if CurStep = ssPostInstall then
  begin
    Lines := PrintersMemo.Lines;
    HasPrinters := False;
    for i := 0 to GetArrayLength(Lines)-1 do
      if Trim(Lines[i]) <> '' then
      begin
        HasPrinters := True;
        Break;
      end;

    if not HasPrinters then Exit;

    // Paths
    IniPath := ExpandConstant('{commonappdata}\Libki\Libki Kiosk Management System.ini');
    ClawPDFIni := ExpandConstant('{app}\clawPDF4Libki.ini');

    // Create base printers folder
    ForceDirectories('C:\printers');
    AddToIniFile(IniPath, '[printer]');

    // Create each printer folder and INI entry
    for i := 0 to GetArrayLength(Lines)-1 do
    begin
      PrinterName := Trim(Lines[i]);
      if PrinterName <> '' then
      begin
        ForceDirectories('C:\printers\' + PrinterName);
        WriteIniString('printer', PrinterName, 'C:\printers\' + PrinterName, IniPath);
      end;
    end;

    // Check if clawPDF already installed
    ClawPDFInstalled :=
      RegKeyExists(HKLM, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\clawPDF') or
      RegKeyExists(HKLM, 'Software\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\clawPDF');

    if not ClawPDFInstalled then
    begin
      Exec(
        'msiexec.exe',
        '/i "clawPDF_0.9.3_setup.msi" /quiet /norestart',
        '',
        SW_HIDE,
        ewWaitUntilTerminated,
        ResultCode
      );
    end;

    // Import clawPDF4Libki.ini configuration
    ClawPDFExe := ExpandConstant('{app}\clawPDF.exe');
    if FileExists(ClawPDFExe) and FileExists(ClawPDFIni) then
    begin
      GetPrinterList(Printers);

      if GetArrayLength(Printers) > 0 then
      begin
        RewriteClawPDFPrinterMappings(
          ExpandConstant('{app}\clawPDF4Libki.ini'),
          Printers
        );
      end;

      { Now import into clawPDF }
      Exec(
        '"' + ClawPDFExe + '"',
        '/Config="' + ExpandConstant('{app}\clawPDF4Libki.ini') + '"',
        '',
        SW_HIDE,
        ewWaitUntilTerminated,
        ResultCode
      );
    end;
  end;
end;

// Helper to append a section header to INI
procedure AddToIniFile(const FilePath, Text: String);
var
  FileHandle: Integer;
begin
  FileHandle := FileOpen(FilePath, fmOpenReadWrite or fmShareDenyNone);
  if FileHandle < 0 then Exit;

  FileSeek(FileHandle, 0, fsFromEnd);
  FileWrite(FileHandle, PAnsiChar(#13#10 + Text + #13#10)^, Length(Text) + 2);
  FileClose(FileHandle);
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

procedure RewriteClawPDFPrinterMappings(const IniPath: String; const Printers: TArrayOfString);
var
  Lines: TArrayOfString;
  NewLines: TArrayOfString;
  i, j, Count: Integer;
  InMappings: Boolean;
begin
  LoadStringsFromFile(IniPath, Lines);

  SetArrayLength(NewLines, 0);
  InMappings := False;

  { Remove existing PrinterMappings sections }
  for i := 0 to GetArrayLength(Lines) - 1 do
  begin
    if Pos('[ApplicationSettings\PrinterMappings', Lines[i]) = 1 then
    begin
      InMappings := True;
      Continue;
    end;

    if InMappings then
    begin
      if (Length(Lines[i]) > 0) and (Lines[i][1] = '[') then
      begin
        InMappings := False;
      end
      else
        Continue;
    end;

    if not InMappings then
    begin
      j := GetArrayLength(NewLines);
      SetArrayLength(NewLines, j + 1);
      NewLines[j] := Lines[i];
    end;
  end;

  { Append new PrinterMappings block }
  Count := GetArrayLength(NewLines);
  SetArrayLength(NewLines, Count + 1);
  NewLines[Count] := '';

  Count := GetArrayLength(NewLines);
  SetArrayLength(NewLines, Count + 1);
  NewLines[Count] := '[ApplicationSettings\PrinterMappings]';

  Count := GetArrayLength(NewLines);
  SetArrayLength(NewLines, Count + 1);
  NewLines[Count] := 'numClasses=' + IntToStr(GetArrayLength(Printers));

  for i := 0 to GetArrayLength(Printers) - 1 do
  begin
    Count := GetArrayLength(NewLines);
    SetArrayLength(NewLines, Count + 1);
    NewLines[Count] := '';

    Count := GetArrayLength(NewLines);
    SetArrayLength(NewLines, Count + 1);
    NewLines[Count] :=
      '[ApplicationSettings\PrinterMappings\' + IntToStr(i) + ']';

    Count := GetArrayLength(NewLines);
    SetArrayLength(NewLines, Count + 1);
    NewLines[Count] := 'PrinterName=' + Printers[i];

    Count := GetArrayLength(NewLines);
    SetArrayLength(NewLines, Count + 1);
    NewLines[Count] := 'ProfileGuid=DefaultGuid';
  end;

  SaveStringsToFile(IniPath, NewLines, False);
end;

