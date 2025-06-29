import Chalk from 'chalk';
import { appendFileSync, existsSync, mkdirSync, PathLike, writeFileSync } from 'fs';
import Path from 'path';

export enum LogLevel {
    CRITICAL=0,
    ERROR=1,
    WARN=2,
    INFO=3,
    DEBUG=4,
    TRACE=5
};

export class Logger {
    private readonly script_name;
    private readonly color_fn;
    private get getDate() { return new Date().toISOString().replace("T", " ").replace("Z", ""); }
    private log_to_file: boolean;
    private log_level: LogLevel;
    private log_file_path: string;
    public get getLogLevel() { return this.log_level; }
    public set setLogLevel(log_level: LogLevel) { this.log_level = log_level; }

    constructor(script_name: string, log_to_file=false, log_level=LogLevel.DEBUG, color?: (...args: any[]) => {}) {
        this.script_name = script_name;
        this.log_file_path = Path.join(import.meta.dirname, `../../../../build/log_${this.script_name.trim().toLocaleLowerCase().replace(/\s/, "_")}.txt`);
        this.log_to_file = log_to_file;
        this.log_level = log_level;
        if (color != null && color != undefined) {
            this.color_fn = color;
        } else {
            this.color_fn = Chalk.reset;
        }
        if (this.log_to_file) {
            if (!existsSync(Path.parse(this.log_file_path).dir)) {
                mkdirSync(Path.parse(this.log_file_path).dir, { recursive: true });
            }
            writeFileSync(this.log_file_path, "");
        }
    }
    private log(msg: any, log_type: LogLevel) {
        const [log_text, log_text_colored] = ((log_type: LogLevel) => {
            switch (log_type) {
                case LogLevel.CRITICAL:
                    return ["CRITICAL", Chalk.bold.bgRed.white("CRITICAL")];
                case LogLevel.ERROR:
                    return ["Error", Chalk.bold.red("Error")];
                case LogLevel.WARN:
                    return ["Warn", Chalk.bold.yellow.italic("Warn")];
                case LogLevel.INFO:
                    return ["Info", Chalk.bold.green("Info")];
                case LogLevel.DEBUG:
                    return ["Debug", Chalk.bold.blue("Debug")];
                case LogLevel.TRACE:
                    return ["Trace", Chalk.bold.dim.italic("Trace")];
                default:
                    return ["UNKNOWN", Chalk.bold.italic.cyan("UNKNOWN")]
            }
        })(log_type);
        console.log(`[${this.color_fn(this.script_name)}] ${Chalk.gray(this.getDate)} [${Chalk.italic(process.pid)}] [${log_text_colored}] `, msg);
        if (this.log_to_file) {
            try {
                appendFileSync(this.log_file_path, `[${this.script_name}] ${this.getDate} [${process.pid}] [${log_text}] ${msg}\n`);
            } catch (e) {
                const log_file_dir_path = Path.parse(this.log_file_path).dir;
                if (!existsSync(log_file_dir_path)) {
                    mkdirSync(log_file_dir_path, { recursive: true });
                    this.warn(`Directory of log file not found! Made new one at '${log_file_dir_path}'`);
                } else if (!existsSync(this.log_file_path)) {
                    try {
                        writeFileSync(this.log_file_path, "");
                        this.warn(`Log file not found! Made new one at '${this.log_file_path}'`);
                    } catch (e) {
                        this.log_to_file = false;
                        this.error(`Log file inaccessible at path '${this.log_file_path}'`);
                        this.error(e);
                    }
                } else {
                    this.log_to_file = false;
                    this.error(`Something really weird happened to the log file at '${this.log_file_path}'`);
                    this.error(e);
                }
            }
        }
    }
    public critical(msg: any) {
        if (this.log_level >= LogLevel.CRITICAL)
            this.log(msg, LogLevel.CRITICAL);
    }
    public error(msg: any) {
        if (this.log_level >= LogLevel.ERROR)
            this.log(msg, LogLevel.ERROR);
    }
    public warn(msg: any) {
        if (this.log_level >= LogLevel.WARN)
            this.log(msg, LogLevel.WARN);
    }
    public info(msg: any) {
        if (this.log_level >= LogLevel.INFO)
            this.log(msg, LogLevel.INFO);
    }
    public debug(msg: any) {
        if (this.log_level >= LogLevel.DEBUG)
            this.log(msg, LogLevel.DEBUG);
    }
    public trace(msg: any) {
        if (this.log_level >= LogLevel.TRACE)
            this.log(msg, LogLevel.TRACE);
    }
}