// add webview stuff
import { rmSync, lstatSync, existsSync, mkdirSync, writeFileSync, copyFile, cpSync, mkdir, copyFileSync, chmodSync, readFileSync } from 'fs';
import Path from 'path';
import os from 'os';
import { LogLevel, Logger } from '../lib/logger/logger.ts';
import Chalk from 'chalk';
import { execSync, spawn } from 'child_process';

const logger = new Logger("Installer", false, LogLevel.TRACE, Chalk.bold.magenta);

type Info = {
    name: string,
    simple_name: string,
    version: string,
    description: string,
    license: string,
    author: string
};

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
let info_cpp: string = "";
const info_cpp_path = Path.join(project_root_dir, "engine", "include", "info.hpp");
try {
    info_cpp = readFileSync(info_cpp_path, "utf8");
} catch (e) {
    logger.critical(e);
    process.exit(-1);
}

const info: Info = {
    name: ((regex_arr: (RegExpMatchArray | null)): string => {
        if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
            logger.error(`#define RENWEB_INFO_DEFAULT_NAME not found in "${info_cpp_path}". Setting to "RenWeb"`);
            return "RenWeb";
        } else {
            return (regex_arr as RegExpMatchArray)[1] as string; 
        }
    })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_NAME\s\"(.*)\"/)),
    simple_name: "",
    version: ((regex_arr: (RegExpMatchArray | null)): string => {
        if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
            logger.error(`#define RENWEB_INFO_DEFAULT_VERSION not found in "${info_cpp_path}". Setting to "v0.0 (default)"`);
            return "v0.0 (default)";
        } else {
            return (regex_arr as RegExpMatchArray)[1] as string; 
        }
    })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_VERSION\s\"(.*)\"/)),
    description: ((regex_arr: (RegExpMatchArray | null)): string => {
        if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
            logger.error(`#define RENWEB_INFO_DEFAULT_DESCRIPTION not found in "${info_cpp_path}". Setting to "I am a default description"`);
            return "I am a default description";
        } else {
            return (regex_arr as RegExpMatchArray)[1] as string; 
        }
    })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_DESCRIPTION\s\"(.*)\"/)),
    license: ((regex_arr: (RegExpMatchArray | null)): string => {
        if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
            logger.error(`#define RENWEB_INFO_DEFAULT_LICENSE not found in "${info_cpp_path}". Setting to "Unknown"`);
            return "Unknown";
        } else {
            return (regex_arr as RegExpMatchArray)[1] as string; 
        }
    })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_LICENSE\s\"(.*)\"/)),
    author: ((regex_arr: (RegExpMatchArray | null)): string => {
        if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
            logger.error(`#define RENWEB_INFO_DEFAULT_AUTHOR not found in "${info_cpp_path}". Setting to "Creature"`);
            return "Creature";
        } else {
            return (regex_arr as RegExpMatchArray)[1] as string; 
        }
    })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_AUTHOR\s\"(.*)\"/)),
};
info.simple_name = info.name.trim().replaceAll(/\s/g, "-").toLowerCase();

const linux = () => {
    const linux_installer_dir = Path.join(installer_dir, "linux");
    const icon_path = Path.join(project_root_dir, "engine", "ico", `${info.simple_name}.ico`);
    const debian_path = Path.join(linux_installer_dir, "debian");
    const debian_package_path = Path.join(debian_path,  info.simple_name);
    const debian_lib_path = Path.join(debian_package_path, "usr", "local", "lib", info.simple_name);
    const debian_app_path = Path.join(debian_package_path, "usr", "share", "applications");
    const debian_dir_path = Path.join(debian_package_path, "DEBIAN");
    const debian_icon_path = Path.join(debian_package_path, "usr", "share", "icons", `${info.simple_name}.ico`);
    const debian_bin_path = Path.join(debian_package_path, "usr", "local", "bin");
    if (!existsSync(linux_installer_dir)) {
        logger.warn(`Linux installer dir doesn't exist at '${linux_installer_dir}'. Making one...`);
        mkdirSync(linux_installer_dir);
    }
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

try {
    execSync("cd ../ && npm run script:build && npm run make clean && npm run make TARGET=release", { stdio: 'inherit' });
} catch (e) {
    logger.critical(e);
    process.exit(-1);
}

switch (os_type) {
    case "Linux":
        linux();
        break;
    case "Darwin":
        break;
    case "Windows_NT":
        break;
    default:
        logger.critical(`unknown OS type: ${os_type}`);
        break;
}


