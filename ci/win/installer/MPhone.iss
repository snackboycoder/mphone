#define AppName "MPhone"
#define AppVersion "0.1.0"
#define Publisher "MPhone"
#define SourceDir GetEnv('MPHONE_PACKAGE_DIR')
#define OutputDir GetEnv('MPHONE_INSTALLER_OUTPUT')
#define RepoDir GetEnv('GITHUB_WORKSPACE')

[Setup]
AppId={{79F98A10-5713-49E4-8E07-3B54791D1A90}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#Publisher}
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=MPhoneSetup-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
SetupIconFile={#RepoDir}\MPhone\res\MPhone.ico
UninstallDisplayIcon={app}\MPhone.exe
PrivilegesRequired=lowest

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\MPhone"; Filename: "{app}\MPhone.exe"; WorkingDir: "{app}"
Name: "{commondesktop}\MPhone"; Filename: "{app}\MPhone.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\MPhone.exe"; Description: "Launch MPhone"; Flags: nowait postinstall skipifsilent
