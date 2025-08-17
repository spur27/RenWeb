// add webview stuff
import { readFileSync } from 'fs';
import Path from 'path';
import os from 'os';
import { LogLevel, Logger } from '../lib/logger/logger.ts';
import Chalk from 'chalk';
import { execSync, spawn } from 'child_process';

const logger = new Logger("Installer", false, LogLevel.TRACE, Chalk.bold.magenta);

const throwCriticalError = (msg: any) => {
    logger.critical(msg);
    throw new Error(msg);
}
const project_root_dir = Path.join(import.meta.dirname, "../../");

let info_cpp: string = "";
const info_cpp_path = Path.join(project_root_dir, "engine", "include", "info.hpp");
try {
    info_cpp = readFileSync(info_cpp_path, "utf8");
} catch (e) {
    logger.critical(e);
    console.log("renweb_app");
}

const name = ((regex_arr: (RegExpMatchArray | null)): string => {
        if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
            logger.critical(`#define RENWEB_INFO_DEFAULT_NAME not found in "${info_cpp_path}". Setting to "renweb_app"`);
            return "renweb_app";
        } else {
            return (regex_arr as RegExpMatchArray)[1] as string; 
        }
    })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_NAME\s\"(.*)\"/));

console.log(name.trim().replaceAll(/\s/g, "-").toLowerCase());

