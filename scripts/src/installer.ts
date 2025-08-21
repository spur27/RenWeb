// add webview stuff
import { rmSync, lstatSync, existsSync, mkdirSync, writeFileSync, copyFile, cpSync, mkdir, copyFileSync, chmodSync, readFileSync, renameSync, createWriteStream } from 'fs';
import Path from 'path';
import os from 'os';
import { LogLevel, Logger } from '../lib/logger.ts';
import Chalk from 'chalk';
import { execSync, spawn } from 'child_process';
import { getInfo, Info } from '../lib/info.ts';
import archiver from 'archiver';

const logger = new Logger("Installer", false, LogLevel.TRACE, Chalk.bold.magenta);

const throwCriticalError = (msg: any) => {
    logger.critical(msg);
    throw new Error(msg);
}

const project_root_dir = Path.join(import.meta.dirname, "../../");
const installer_dir = Path.join(project_root_dir, 'installers');
if (!existsSync(installer_dir)) {
    logger.warn(`Installer dir not found at "${installer_dir}". Making one...`);
    mkdirSync(installer_dir);
}

logger.info("Starting install creator...");
logger.trace(`Args are ${JSON.stringify(process.argv, null, 2)}`);
logger.debug(`Project root dir is \n\t'${project_root_dir}'`);
logger.debug(`Install dir is \n\t'${installer_dir}'`);

const os_type: string = os.type();
const os_machine: string = os.machine();

logger.info(`OS is of type ${Chalk.bold(os_type)} of machine ${Chalk.bold(os_machine)}`);

const info: Info = getInfo();

const linux = () => {
    const linux_installer_dir = Path.join(installer_dir, "linux");
    const icon_path = Path.join(project_root_dir, "engine", "resource", `app.ico`);
    const debian_path = Path.join(linux_installer_dir, "debian");
    const debian_package_path = Path.join(debian_path,  info.simple_name);
    const debian_lib_path = Path.join(debian_package_path, "usr", "local", "lib", info.simple_name);
    const debian_app_path = Path.join(debian_package_path, "usr", "share", "applications");
    const debian_dir_path = Path.join(debian_package_path, "DEBIAN");
    const debian_icon_path = Path.join(debian_package_path, "usr", "share", "icons", `${info.simple_name}.ico`);
    const debian_bin_path = Path.join(debian_package_path, "usr", "local", "bin");
    if (existsSync(linux_installer_dir)) {
        logger.warn(`Linux installer dir already exist at '${linux_installer_dir}'. Clearing...`);
        rmSync(linux_installer_dir, { recursive: true });
    }
    mkdirSync(linux_installer_dir);
  // DEBIAN
    logger.info("Creating debian package...");
    if (existsSync(debian_path)) {
        logger.warn(`path '${debian_path}' already exists. Removing...`);
        rmSync(debian_path, { recursive: true });
    }
    // see if you can change this to install locally rather than for all users (would make more sense)where
    // idk if this is even being installed in the right place
    mkdirSync(debian_lib_path, { recursive: true });
    mkdirSync(debian_dir_path);
    writeFileSync(Path.join(debian_dir_path, "control"), 
        `Package: ${info.simple_name}\nVersion: ${info.version}\nArchitecture: all\nMaintainer: ${info.author}\nDescription: ${info.description}\n`
    );
    mkdirSync(debian_app_path, { recursive: true });
    writeFileSync(Path.join(debian_app_path, `${info.simple_name}.desktop`), 
        `[Desktop Entry]\nEncoding=UTF-8\nVersion=${info.version}\nComment=${info.description}\nType=Application\nTerminal=false\nExec=/usr/local/lib/${info.simple_name}/${info.simple_name}\nName=${info.name}\nIcon=/usr/share/icons/${info.simple_name}.ico`
    )
    if (existsSync(icon_path)) {
        // if (existsSync(Path.join(debian_path, "renweb", "usr", "share", "icons"))) {
        //     mkdirSync(Path.join(debian_path, "renweb", "usr", "share", "icons"), { recursive: true });
        // }
        cpSync(icon_path, debian_icon_path, { recursive: true });
    } else {
        logger.warn("No icon found.");
    }
    mkdirSync(debian_bin_path, { recursive: true });
    writeFileSync(Path.join(debian_bin_path, info.simple_name), `exec ${Path.join("/", "usr", "local", "lib", info.simple_name, info.simple_name).toString()} "$@"`);
    execSync(`chmod +x ${Path.join(debian_bin_path, info.simple_name).toString()}`);
    // chmodSync(Path.join(debian_path, "renweb", "usr", "local", "bin", "renweb"), 755);
    cpSync(Path.join(project_root_dir, "build"), debian_lib_path, { recursive: true });
    try {
        execSync(`dpkg-deb --build ${debian_package_path}`, { stdio: 'inherit' });
    } catch (e) {
        logger.critical(e);
    }
    rmSync(debian_package_path, { recursive: true });
}

const windows = () => {
    const windows_installer_dir = Path.join(installer_dir, "windows");
    const nsis_dir = Path.join(windows_installer_dir, "nsis");
    if (existsSync(windows_installer_dir)) {
        logger.warn(`Windows installer dir already exist at '${windows_installer_dir}'. Clearing...`);
        rmSync(windows_installer_dir, { recursive: true });
    }
    mkdirSync(windows_installer_dir);
    mkdirSync(nsis_dir);
    logger.info(`Writing setup wizard to '${Path.join(nsis_dir, 'setup_wizard.nsi')}`);
writeFileSync(Path.join(nsis_dir, 'setup_wizard.nsi'),
`
;--------------------------------
; ${info.name} NSIS Setup Wizard
;--------------------------------

${(existsSync(Path.join(project_root_dir, 'engine', 'resource', 'app.ico')))
    ?
`
!define MUI_ICON "../../../engine/resource/app.ico"       ; Installer icon
!define MUI_UNICON "../../../engine/resource/app.ico"     ; Uninstaller icon
`
    : ""}

!include "MUI2.nsh"
!include "nsDialogs.nsh"
!include "WinMessages.nsh"

Var MAIN_EXE 
Var CREATE_DESKTOP_SHORTCUT

Name "${info.name}"
OutFile "${info.simple_name}-installer.exe"
InstallDir "$LOCALAPPDATA\\${info.name}"
RequestExecutionLevel user
!insertmacro MUI_PAGE_WELCOME
${(existsSync(Path.join(project_root_dir, 'engine', 'resource', 'LICENSE.txt')))
    ? `"!insertmacro MUI_PAGE_LICENSE "../../../engine/resource/LICENSE.txt"` : ""
}
!insertmacro MUI_PAGE_DIRECTORY
Page custom DesktopShortcutPage DesktopShortcutPageLeave
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\\${info.simple_name}.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Run ${info.name}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Function DesktopShortcutPage
  nsDialogs::Create 1018
  Pop $0
  \${If} $0 == error
    Abort
  \${EndIf}
  !insertmacro MUI_HEADER_TEXT "Shortcut Options" "Choose which shortcuts to create"
  \${NSD_CreateCheckBox} 0u 0u 100% 12u "Create a desktop shortcut"
  Pop $CREATE_DESKTOP_SHORTCUT
  \${NSD_SetState} $CREATE_DESKTOP_SHORTCUT \${BST_CHECKED}
  nsDialogs::Show
FunctionEnd
Function DesktopShortcutPageLeave
    \${NSD_GetState} $CREATE_DESKTOP_SHORTCUT $CREATE_DESKTOP_SHORTCUT
FunctionEnd

; ----------------------------
Section "Install"
  StrCpy $MAIN_EXE "${info.simple_name}.exe"

  SetOutPath "$INSTDIR"

  File /r "../../../build\\*.*"

  CreateDirectory "$SMPROGRAMS\\${info.name}"
  \${If} $CREATE_DESKTOP_SHORTCUT == \${BST_CHECKED}
    CreateShortCut "$DESKTOP\\${info.name}.lnk" "$INSTDIR\\$MAIN_EXE" "" "" "" SW_SHOWNORMAL "" "${info.author}.${info.name_no_whitespace}"
  \${EndIf}

  CreateShortCut "$SMPROGRAMS\\${info.name}\\${info.name}.lnk" "$INSTDIR\\$MAIN_EXE" "" "" "" SW_SHOWNORMAL "" "${info.author}.${info.name_no_whitespace}"

  WriteUninstaller "$INSTDIR\\uninstall.exe"

  WriteRegStr HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${info.name_no_whitespace}" "DisplayName" "${info.name}"
  WriteRegStr HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${info.name_no_whitespace}" "DisplayVersion" "${info.version}"
  WriteRegStr HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${info.name_no_whitespace}" "Publisher" "${info.author}"
  WriteRegStr HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${info.name_no_whitespace}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${info.name_no_whitespace}" "UninstallString" "$INSTDIR\\uninstall.exe"

SectionEnd
; ----------------------------
Section "Uninstall"

  ; Remove all installed files and folders
  RMDir /r "$INSTDIR"

  ; Remove shortcuts
  Delete "$DESKTOP\\${info.name}.lnk"
  Delete "$SMPROGRAMS\\${info.name}\\${info.name}.lnk"
  RMDir "$SMPROGRAMS\\${info.name}"

  ; Remove registry uninstall info
  DeleteRegKey HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\${info.name_no_whitespace}"

SectionEnd
; ----------------------------

!define MUI_ABORTWARNING
`
);
    execSync("makensis ../installers/windows/nsis/setup_wizard.nsi", { stdio: 'inherit' });
    renameSync(Path.join(nsis_dir, `${info.simple_name}-installer.exe`), Path.join(windows_installer_dir, `${info.simple_name}-installer.exe`));
    rmSync(nsis_dir, { recursive: true });
    let output = createWriteStream(Path.join(windows_installer_dir, `${info.simple_name}.zip`));
    let archive = archiver('zip');
    output.on('close', () => {
        logger.info(`Saved ${archive.pointer()} bytes to ./windows/${info.simple_name}.zip`);
    });
    archive.on('error', (err) => {throw err});
    archive.pipe(output);
    archive.directory(Path.join(project_root_dir, 'build'), false);
    archive.finalize();
}

try {
    switch (os_type) {
        case "Linux":
            execSync("cd ../ && npm run script:build && npm run make clean && npm run make TARGET=release", { stdio: 'inherit' });
            linux();
            break;
        case "Darwin":
            break;
        case "Windows_NT":
            execSync("cd ..\\ && npm run script:build && npm run make clean && npm run make TARGET=release", { stdio: 'inherit' });
            windows();
            break;
        default:
            logger.critical(`unknown OS type: ${os_type}`);
            break;
    }
} catch (e) {
    logger.critical(e);
}

