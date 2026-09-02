; Inno Setup script for the P4 Windows installer.
;
; Built by .github/workflows/build-windows.yml from the already-assembled
; dist\ directory, so the installer always contains exactly the same files as
; the zip:
;
;   iscc /DAppVersion=7.3.1 packaging\p4.iss
;
; Users of the original P4 expected an installer; the plain zip left them
; copying files by hand into a directory that has no resemblance to the old
; layout, with no Start menu entry and no way to uninstall.

#ifndef AppVersion
  #define AppVersion "0.0.0"
#endif

[Setup]
; Never change AppId: it is what lets a new version replace an old one
; instead of installing beside it.
AppId={{6B2A9E4C-1D3F-4A87-9E10-5C7F2B8A4D61}
AppName=P4
AppVersion={#AppVersion}
AppVerName=P4 {#AppVersion}
AppPublisher=P4 contributors
AppPublisherURL=https://github.com/JeeerryZ/p4
AppSupportURL=https://github.com/JeeerryZ/p4/issues
AppUpdatesURL=https://github.com/JeeerryZ/p4/releases
DefaultDirName={autopf}\P4
DefaultGroupName=P4
LicenseFile=..\COPYING.LESSER
SetupIconFile=..\help\newp4icon.ico
UninstallDisplayIcon={app}\p4.exe
OutputDir=..
OutputBaseFilename=P4-windows-x86_64-setup
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
DisableProgramGroupPage=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; dist\ already holds p4.exe, the three helper binaries, the Qt plugins and
; the full runtime DLL set assembled by the workflow.
Source: "..\dist\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs ignoreversion

[Dirs]
; lyapunov writes its summation tables here on first use.  Under
; C:\Program Files a standard user cannot write to the install directory, and
; the write failing is exactly what makes Maple report
; "Error (readlyapunovresult) invalid subscript selector" -- so this one
; subdirectory has to be user-writable.
Name: "{app}\sumtables"; Permissions: users-modify

[Icons]
Name: "{group}\P4"; Filename: "{app}\p4.exe"
Name: "{group}\{cm:UninstallProgram,P4}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\P4"; Filename: "{app}\p4.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\p4.exe"; Description: "{cm:LaunchProgram,P4}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
; Generated at run time, so Inno does not know about them.
Type: filesandordirs; Name: "{app}\sumtables"
