// add webview stuff
import { rmSync, lstatSync, existsSync, mkdirSync, writeFileSync, copyFile, cpSync, mkdir, copyFileSync, chmodSync, readFileSync, renameSync, createWriteStream } from 'fs';
import Path from 'path';
import os from 'os';
import { LogLevel, Logger } from '../lib/logger.ts';
import Chalk from 'chalk';
import { execSync, spawn } from 'child_process';
import { getInfo, getLinuxPMType, Info } from '../lib/info.ts';
import archiver from 'archiver';

const logger = new Logger("Setup", false, LogLevel.TRACE, Chalk.bold.magenta);

const throwCriticalError = (msg: any) => {
    logger.critical(msg);
    throw new Error(msg);
}

const good = (msg: any) => {
    console.log(Chalk.green(msg));
}

const bad = (msg: any) => {
    console.log(Chalk.red(msg));
}

const warn = (msg: any) => {
    console.log(Chalk.yellow(msg));
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

const skip = process.argv.includes("skip");

const os_type: string = os.type();
const os_machine: string = os.machine();

logger.info(`OS is of type ${Chalk.bold(os_type)} of machine ${Chalk.bold(os_machine)}`);

const info: Info = getInfo();

try {
    switch (os_type) {
        case "Linux":
            switch (getLinuxPMType()) {
                case "deb":
                    logger.warn("deb is a WIP! Not doing anything...");
                    break;
                case "rpm":
                    logger.warn("rpm is a WIP! Not doing anything...");
                    break;
                default:
                    throwCriticalError("You're using a version of linux with an unsupported package manager! RenWeb currently only supports debian and rpm.");
            }
            break;
        case "Darwin":
            logger.warn(`OS not supported yet! Doing nothing...`);
            break;
        case "Windows_NT":
            logger.warn("Windows is a WIP! Not doing anything...");
            break;
        default:
            throw new Error(`unknown OS type: ${os_type}`);
            break;
    }
} catch (e) {
    logger.critical(e);
}

