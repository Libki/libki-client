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
  
  { Set default values, using settings that were stored last time if possible }
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
