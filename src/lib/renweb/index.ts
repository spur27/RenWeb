/* 
* -----------------------------------------------
* ------------------Bindings---------------------
* -----------------------------------------------
*/ 
export namespace RenWeb {
    export namespace Log {
        export function trace   (msg: string): void { BIND_logTrace   (msg); }
        export function debug   (msg: string): void { BIND_logDebug   (msg); }
        export function info    (msg: string): void { BIND_logInfo    (msg); }
        export function warn    (msg: string): void { BIND_logWarn    (msg); }
        export function error   (msg: string): void { BIND_logError   (msg); }
        export function critical(msg: string): void { BIND_logCritical(msg); }
    };
    export namespace FS {
        export function readFile  (path_to_file: string):                (string    | RenWeb_Error) { return BIND_readFile(path_to_file)             as (string    | RenWeb_Error); }
        export function writeFile (path_to_file: string, contents: {}):  (undefined | RenWeb_Error) { return BIND_writeFile(path_to_file, contents)  as (undefined | RenWeb_Error); }
        export function appendFile(path_to_file: string, contents: {}):  (undefined | RenWeb_Error) { return BIND_appendFile(path_to_file, contents) as (undefined | RenWeb_Error); }
        export function exists    (path: string):                        (undefined | RenWeb_Error) { return BIND_exists(path)                       as (undefined | RenWeb_Error); }
        export function mkDir     (path: string):                        (undefined | RenWeb_Error) { return BIND_mkDir(path)                        as (undefined | RenWeb_Error); }
        export function rm        (path: string, recursively=false):     (undefined | RenWeb_Error) { return BIND_rm(path, recursively)              as (undefined | RenWeb_Error); }
        export function ls        (path: string):                        (string[]  | RenWeb_Error) { return BIND_ls(path)                           as (string[]  | RenWeb_Error); }
        export function move      (curr_path: string, new_path: string): (undefined | RenWeb_Error) { return BIND_move(curr_path, new_path)          as (undefined | RenWeb_Error); }
        export function copy      (curr_path: string, new_path: string): (undefined | RenWeb_Error) { return BIND_copy(curr_path, new_path)          as (undefined | RenWeb_Error); }
    };
    export namespace Window {
        export function setHTML       (html: string):                        (undefined | RenWeb_Error) { return BIND_setHTML(html)                           as (undefined | RenWeb_Error); }
        export function setHTMLToDoc  (html_doc_name: string):               (undefined | RenWeb_Error) { return BIND_setHTMLToDoc(html_doc_name)             as (undefined | RenWeb_Error); }
        export function setSize       (width: number, height: number,          
                                       window_hint=RenWeb_WindowHint.FIXED): (undefined | RenWeb_Error) { return BIND_setSize(width, height, 
                                                                                                                              RenWeb_WindowHint[window_hint]) as (undefined | RenWeb_Error); }
        export function setTitle      (title: string):                       (undefined | RenWeb_Error) { return BIND_setTitle(title)                         as (undefined | RenWeb_Error); }
        export function getHTMLDocName():                                    (string    | RenWeb_Error) { return BIND_getHTMLDocName()                        as (string    | RenWeb_Error); }
        export function open          ():                                    (number    | RenWeb_Error) { return BIND_open()                                  as (number    | RenWeb_Error); }
        export function terminate     ():                                     void                      {        BIND_terminate(); }
    };
    export namespace Process {
        export function getPID():            number { return BIND_getPID()            as number; }
        export function getProcessDirPath(): string { return BIND_getProcessDirPath() as string; }
    };
    export namespace Settings {
        export function save():                          (undefined       | RenWeb_Error) { return BIND_saveSettings()        as (undefined       | RenWeb_Error); }
        export function set (settings: RenWeb_Settings): (undefined       | RenWeb_Error) { return BIND_setSettings(settings) as (undefined       | RenWeb_Error); }
        export function read():                          (RenWeb_Settings | RenWeb_Error) { return BIND_readSettings()        as (RenWeb_Settings | RenWeb_Error); }
    }

};



/* 
* -----------------------------------------------
* -----------------Functions---------------------
* -----------------------------------------------
*/ 
export function isNullish(variable: {}): boolean {
    return (variable == null || variable == undefined);
}
export function isString(variable: {}): boolean {
    return (typeof variable == 'string');
}


/* 
* -----------------------------------------------
* --------------------Types----------------------
* -----------------------------------------------
*/ 
export enum RenWeb_WindowHint {
    NONE=0,
    MIN=1,
    MAX=2,
    FIXED=3
};
export type RenWeb_Settings = {
    html_doc_name: string,
    window_title: string,
    window_hint: RenWeb_WindowHint,
    window_width: number,
    window_height: number,
    window_save_resize_dimensions: boolean,
    window_resizable: boolean
}
export type RenWeb_Error = {
    name: string,
    where: string,
    code?: number,
    msg: string
}


/* 
* -----------------------------------------------
* ------------------Declares---------------------
* -----------------------------------------------
*/ 
declare const BIND_logTrace:    (...args: any[]) => {};
declare const BIND_logDebug:    (...args: any[]) => {};
declare const BIND_logInfo:     (...args: any[]) => {};
declare const BIND_logWarn:     (...args: any[]) => {};
declare const BIND_logError:    (...args: any[]) => {};
declare const BIND_logCritical: (...args: any[]) => {};

declare const BIND_readFile:   (...args: any[]) => {};
declare const BIND_writeFile:  (...args: any[]) => {};
declare const BIND_appendFile: (...args: any[]) => {};
declare const BIND_exists:     (...args: any[]) => {};
declare const BIND_mkDir:      (...args: any[]) => {};
declare const BIND_rm:         (...args: any[]) => {};
declare const BIND_ls:         (...args: any[]) => {};
declare const BIND_move:       (...args: any[]) => {};
declare const BIND_copy:       (...args: any[]) => {};

declare const BIND_setHTML:        (...args: any[]) => {};
declare const BIND_setHTMLToDoc:   (...args: any[]) => {};
declare const BIND_setSize:        (...args: any[]) => {};
declare const BIND_setTitle:       (...args: any[]) => {};
declare const BIND_getHTMLDocName: (...args: any[]) => {};
declare const BIND_open:           (...args: any[]) => {};
declare const BIND_terminate:      (...args: any[]) => {};

declare const BIND_getPID:            (...args: any[]) => {};
declare const BIND_getProcessDirPath: (...args: any[]) => {};

declare const BIND_saveSettings: (...args: any[]) => {};
declare const BIND_setSettings:  (...args: any[]) => {};
declare const BIND_readSettings: (...args: any[]) => {};


