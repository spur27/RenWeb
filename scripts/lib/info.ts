// add webview stuff
import { readFileSync } from 'fs';
import Path from 'path';
import { LogLevel, Logger } from './logger.ts';
import Chalk from 'chalk';
import { execSync } from 'child_process';


export type Info = {
    name: string,
    simple_name: string,
    name_no_whitespace: string,
    version: string,
    description: string,
    license: string,
    author: string
};

export function getInfo(): Info {
    const project_root_dir = Path.join(import.meta.dirname, "../../");
    let info_cpp: string = "";
    const info_cpp_path = Path.join(project_root_dir, "engine", "include", "info.hpp");
    const logger = new Logger("GetInfo", false, LogLevel.TRACE, Chalk.bold.magenta);
    const default_info: Info = {
        name: "RenWeb",
        simple_name: "renweb",
        name_no_whitespace: "RenWeb",
        version: "0.0",
        description: "I am a default description",
        license: "Default Licesne",
        author: "DefaultAuthor"
    };
    try {
        info_cpp = readFileSync(info_cpp_path, "utf8");
    } catch (e) {
        logger.critical(e);
        return default_info;
    }
    const info: Info = {
        name: ((regex_arr: (RegExpMatchArray | null)): string => {
            if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
                logger.error(`#define RENWEB_INFO_DEFAULT_NAME not found in "${info_cpp_path}". Setting to "${default_info.name}"`);
                return default_info.name;
            } else {
                return (regex_arr as RegExpMatchArray)[1] as string; 
            }
        })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_NAME\s\"(.*)\"/)),
        simple_name: "",
        name_no_whitespace: "",
        version: ((regex_arr: (RegExpMatchArray | null)): string => {
            if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
                logger.error(`#define RENWEB_INFO_DEFAULT_VERSION not found in "${info_cpp_path}". Setting to "${default_info.version}"`);
                return default_info.version;
            } else {
                return (regex_arr as RegExpMatchArray)[1] as string; 
            }
        })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_VERSION\s\"(.*)\"/)),
        description: ((regex_arr: (RegExpMatchArray | null)): string => {
            if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
                logger.error(`#define RENWEB_INFO_DEFAULT_DESCRIPTION not found in "${info_cpp_path}". Setting to "${default_info.description}"`);
                return default_info.description;
            } else {
                return (regex_arr as RegExpMatchArray)[1] as string; 
            }
        })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_DESCRIPTION\s\"(.*)\"/)),
        license: ((regex_arr: (RegExpMatchArray | null)): string => {
            if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
                logger.error(`#define RENWEB_INFO_DEFAULT_LICENSE not found in "${info_cpp_path}". Setting to "${default_info.license}"`);
                return default_info.license;
            } else {
                return (regex_arr as RegExpMatchArray)[1] as string; 
            }
        })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_LICENSE\s\"(.*)\"/)),
        author: ((regex_arr: (RegExpMatchArray | null)): string => {
            if (regex_arr == null || (regex_arr as RegExpMatchArray)[1] == null) {
                logger.error(`#define RENWEB_INFO_DEFAULT_AUTHOR not found in "${info_cpp_path}". Setting to "${default_info.author}"`);
                return default_info.author;
            } else {
                return (regex_arr as RegExpMatchArray)[1] as string; 
            }
        })(info_cpp.match(/#define\sRENWEB_INFO_DEFAULT_AUTHOR\s\"(.*)\"/)),
    } as Info;
    info.simple_name = info.name.trim().replaceAll(/\s/g, "-").toLowerCase();
    info.name_no_whitespace = info.name.trim().replaceAll(/\s/g, "");
    return info;
}

//
export function getLinuxPMType(): string {
    try {
        if (execSync("dpkg --version")) {
            return 'deb';
        }
    } catch (e) { }
    try {
        if (execSync("rpm --version")) {
            return 'rpm';
        }
    } catch (e) { }
    return "";
}