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
    process.exit(-1);
}

const rainbowify = (str: string) => {
    let rainbow_str: string = "";
    for (let i = 0; i < str.length; i++) {
        switch (i % 6) {
            case 0: rainbow_str += Chalk.red(str[i]); break;
            case 1: rainbow_str += Chalk.yellow(str[i]); break;
            case 2: rainbow_str += Chalk.green(str[i]); break;
            case 3: rainbow_str += Chalk.cyan(str[i]); break;
            case 4: rainbow_str += Chalk.blue(str[i]); break;
            case 5: rainbow_str += Chalk.magenta(str[i]); break;
        }
    }
    return rainbow_str;

}

const good = (msg: any, followup="was found.") => {
    console.log(`    ${Chalk.bold(msg)} ${Chalk.reset.green(followup)}`);
    return true;
}

const bad = (msg: any, followup="is missing or is unreachable.") => {
    console.log(`    ${Chalk.bold(msg)} ${Chalk.reset.red(followup)}`);
    return false;
}

const warn = (msg: any, followup="is missing or is unreachable.") => {
    console.log(`    ${Chalk.bold(msg)} ${Chalk.reset.yellow(followup)}`);
    return true;
}

const passive = (msg: any) => {
    console.log(`  ${Chalk.gray(msg)}${Chalk.reset('')}`);
    return true;
}

const header = (msg: any) => {
    const barrier = rainbowify('-----------');
    let str = barrier;
    str += ` ${Chalk.reset.bold(msg)}${Chalk.reset(' ')}`;
    str += barrier
    console.log(str);
}

const project_root_dir = Path.join(import.meta.dirname, "../../");

logger.info("Starting setup utility...");
// logger.trace(`Args are ${JSON.stringify(process.argv, null, 2)}`);
// logger.debug(`Project root dir is \n\t'${project_root_dir}'`);

const skip = process.argv.includes("skip");

const os_type: string = os.type();
const os_machine: string = os.machine();

logger.info(`Running for OS ${os_type} on machine ${os_machine}`);

const info: Info = getInfo();

const deb = () => {
    logger.info(`Checking for debian...`);
    header(`Essentials`);
    let essentials_tool_flag = true;
    let exists = (tool: string, bad_callback=bad, bad_msg="not found.", req=true) => {
        try { execSync(`command -v ${tool}`, {stdio: "ignore"}); good(tool); } 
        catch (e) { let res = bad_callback(tool, bad_msg); if (!essentials_tool_flag) essentials_tool_flag = res; }
    }
    exists('g++', warn, "change CXX in ./engine/makefile if you're using a different compiler.");
    exists('make');
    exists('git', warn, "make sure you install required submodules to ./engine/external if you haven't already.");
    exists('dpkg');
    exists('nvm', warn, "not found. [optional]");
    exists('bear', warn, "useful for generating ./engine/compile_commands.json");
    if (!essentials_tool_flag) {
        throwCriticalError(`Cannot progress until all essentials are installed and usable`);
    }
    header(`Libs`);
    let missing_packages = '';
    exists = (pkg: string, bad_callback=bad, bad_msg="is missing.", req=true) => {
        try { execSync(`dpkg -s ${pkg}`, {stdio: "ignore"}); good(pkg, "is installed."); } 
        catch (e) { bad_callback(pkg, bad_msg); if(req) missing_packages += `${pkg} `}
    }
    exists('libgtk-3-dev');
    exists('libwebkit2gtk-4.1-dev');
    exists('libgtk-3-0', warn, "is missing and is used when compiling for release if available. [not required]", false);
    exists('libwebkit2gtk-4.1-0', warn, "is missing and is used when compiling for release if available. [not required]", false);
    exists('pkg-config');
    exists('libboost-all-dev');
    exists('libboost-url-dev');
    if (missing_packages.length > 0) {
        throwCriticalError(`Missing essential packages. Install the following packages with your package manager:\n    ${missing_packages}`);
    }
    header(`Submodules`);
    let no_missing_submodule_flag = true;
    exists = (submodule: string) => {
        if (existsSync(Path.join(project_root_dir, 'engine', 'external', submodule))) good(submodule, `found at '${Path.join('.', 'engine', 'external', submodule).toString()}'`);
        else no_missing_submodule_flag = bad(submodule, `missing. Should be at '${Path.join('.', 'engine', 'external', submodule).toString()}'`);
    }
    exists('json');
    exists('spdlog');
    exists('termcolor');
    exists('webview');
    if (!no_missing_submodule_flag) {
        throwCriticalError("Required submodules not found. Make sure you clone the repo with '--recurse-submodules' as a flag!");
    }
    
    logger.info(`Checking for debian... [DONE]`);
}



try {
    switch (os_type) {
        case "Linux":
            switch (getLinuxPMType()) {
                case "deb":
                    deb();
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

