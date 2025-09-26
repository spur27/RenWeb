const DEFAULT_INDENT = 2;
/* 
* -----------------------------------------------
* ------------------Exports----------------------
* -----------------------------------------------
*/ 
export function sync(fn: (...args: []) => Promise<{}>, ...args: []): {} { return (async (args: []) => { return await fn(...args); })(args); }
export namespace Log {
    export type Settings = {
        indent: number
    }
    // ---------------------------------------------  
    export async function trace(msg: {}, settings: Log.Settings={indent:DEFAULT_INDENT}): Promise<void> 
        { await BIND_logTrace(Util.toUint8array(msg, settings.indent)); }
    export async function debug(msg: {}, settings: Log.Settings={indent:DEFAULT_INDENT}): Promise<void> 
        { await BIND_logDebug(Util.toUint8array(msg, settings.indent)); }
    export async function info(msg: {}, settings: Log.Settings={indent:DEFAULT_INDENT}): Promise<void> 
        { await BIND_logInfo(Util.toUint8array(msg, settings.indent)); }
    export async function warn(msg: {}, settings: Log.Settings={indent:DEFAULT_INDENT}): Promise<void> 
        { await BIND_logWarn(Util.toUint8array(msg, settings.indent)); }
    export async function error(msg: {}, settings: Log.Settings={indent:DEFAULT_INDENT}): Promise<void> 
        { await BIND_logError(Util.toUint8array(msg, settings.indent)); }
    export async function critical(msg: {}, settings: Log.Settings={indent:DEFAULT_INDENT}): Promise<void> 
        { await BIND_logCritical(Util.toUint8array(msg, settings.indent)); }
};
export namespace FS {
    export type WriteSettings = {
        append: boolean,
        indent: number
    }
    export type RmSettings = {
        recursive: boolean
    }
    export type RenameCopySettings = {
        overwrite: boolean
    }
    // ---------------------------------------------  
    export async function readFile(path: string): Promise<(Uint8Array | null)> 
        { const arr = await BIND_readFile(Util.toUint8array(path)); if (arr == null) return null; else return new Uint8Array(arr as  number[]); }
    export async function writeFile(path: string, contents: {}, settings: FS.WriteSettings={indent:DEFAULT_INDENT, append:false}): Promise<boolean> 
        { return await BIND_writeFile(Util.toUint8array(path), Util.toUint8array(contents, settings.indent), settings) as boolean; }
    export async function exists(path: string): Promise<boolean> 
        { return await BIND_exists(Util.toUint8array(path)) as boolean; }
    export async function isDir(path: string): Promise<boolean> 
        { return await BIND_isDir(Util.toUint8array(path)) as boolean; }
    export async function mkDir(path: string): Promise<boolean> 
        { return await BIND_mkDir(Util.toUint8array(path)) as boolean; }
    export async function rm(path: string, settings: FS.RmSettings={recursive:false}): Promise<boolean> 
        { return await BIND_rm(Util.toUint8array(path), settings) as boolean; }
    export async function ls(path: string): Promise<(string[] | null)> 
        { const files = (await BIND_ls(Util.toUint8array(path))) as number[][]; if (files == null) return null; else { const uint8arr_files: Uint8Array[] = []; files.forEach((el) => uint8arr_files.push(new Uint8Array(el))); return Util.fromArrayUint8array(uint8arr_files); } }
    export async function rename(orig_path: string, new_path: string, settings: FS.RenameCopySettings={overwrite:false}): Promise<boolean> 
        { return await BIND_rename(Util.toUint8array(orig_path), Util.toUint8array(new_path), settings) as boolean; }
    export async function copy(orig_path: string, new_path: string, settings: FS.RenameCopySettings={overwrite:false}): Promise<boolean> 
        { return await BIND_copy(Util.toUint8array(orig_path), Util.toUint8array(new_path), settings) as boolean; }
    export async function chooseFiles(multi?: boolean, dirs?: boolean, filtration?: string[], initial_dir?: string): Promise<(string[] | null)> 
        { const files = await BIND_chooseFiles(multi, dirs, (filtration == null) ? null : Util.arrayToUint8array(filtration), (initial_dir == null) ? null : Util.toUint8array(initial_dir)); if (files == null) 
            return null; else { const conv_files: Uint8Array[] = []; (files as []).forEach(el => conv_files.push(new Uint8Array(el))); return Util.fromArrayUint8array(conv_files); } }
};
export namespace Window {
    export enum WebviewHint {
        NONE=0,
        MIN=1,
        MAX=2,
        FIXED=3
    };
    export type Settings = {
        name: string,
        version: string,
        description: string,
        license: string,
        author: string,
        hint: Window.WebviewHint,
        width: number,
        height: number,
        save_resize_dimensions: boolean,
        resizable: boolean,
        decorated: boolean,
        keep_above: boolean
    }
    export type OpenWindowSettings = {
        single: boolean
    }
    // ---------------------------------------------  
    export async function saveSettings(): Promise<void> 
        { await BIND_saveSettings(); }
    export async function refreshSettings(): Promise<void> 
        { await BIND_refreshSettings(); }
    export async function reloadPage(): Promise<void> 
        { await BIND_reloadPage(); }
    export async function setSettings(settings: (Settings & {})): Promise<void> 
        { await BIND_setSettings(settings); }
    export async function getSettings(): Promise<Window.Settings> 
        { return await BIND_getSettings() as Window.Settings; }
    export async function resetSettingsToDefaults(): Promise<void> 
        { await BIND_resetSettingsToDefaults(); }
    export async function updateSize(force: boolean=false): Promise<void> 
        { await BIND_updateSize(force); }
    export async function close(): Promise<void> 
        { await BIND_close(); }
    export async function openWindow(uri: string="_", settings: Window.OpenWindowSettings={single:false}): Promise<void> 
        { await BIND_openWindow(Util.toUint8array(uri), settings); }
    export async function minimize(): Promise<void> 
        { await BIND_minimize(); }
    export async function maximize(): Promise<void> 
        { await BIND_maximize(); }
    export async function fullscreen(): Promise<void> 
        { await BIND_fullscreen(); }
    export async function hide(): Promise<void> 
        { await BIND_hide(); }
    export async function show(): Promise<void> 
        { await BIND_show(); }
};
export namespace Util {
    export async function getPID(): Promise<number> 
        { return await BIND_getPID() as number; }
    export async function getOS(): Promise<string> 
        { return await BIND_getOS() as string; }
    export async function getApplicationDirPath(): Promise<string> 
        { return Util.fromUint8array(new Uint8Array(await BIND_getApplicationDirPath() as number[])) ?? "" as string; }
    export async function sendNotif(title: string, message?: string, icon_path?: string): Promise<void> 
        { await BIND_sendNotif(Util.toUint8array(title), (message == null) ? null : Util.toUint8array(message), (icon_path == null) ? null : Util.toUint8array(icon_path)); }
    export async function openURI(resource: string): Promise<void> 
        { await BIND_openURI(Util.toUint8array(resource)); }
    // -------------------NON BINDINGS--------------------------        
    export function isNullish(variable: {}): boolean 
        { return (variable == null || variable == undefined); }
    export function isString(variable: {}): boolean 
        { return (typeof variable == 'string'); }
    export function arrayToUint8array(variable: {}[], indent=2): Uint8Array[] 
        { const encoded_arr: Uint8Array[] = []; variable.forEach((el) => encoded_arr.push(Util.toUint8array(el, indent))); return encoded_arr}
    export function toUint8array(variable: {}, indent=2): Uint8Array 
        { return ((new TextEncoder()).encode((isString(variable)) ? variable as string : JSON.stringify(variable, null, indent))); }
    export function fromArrayUint8array(uint8arrays: Uint8Array[]): (string[] | null) 
        { try { const decoded_arr: string[] = []; uint8arrays.forEach((el) => { const result = Util.fromUint8array(el); if (result != null) decoded_arr.push(result); }); return decoded_arr; } catch (e) { Log.error((e as Error).message); return null } }
    export function fromUint8array(uint8array: Uint8Array): (string | null) 
        { try { return ((new TextDecoder()).decode(uint8array)) } catch (e) { Log.error((e as Error).message); return null; } }
};

/* 
* -----------------------------------------------
* ------------------Declares---------------------
* -----------------------------------------------
*/ 
declare const BIND_logTrace:    (...args: any[]) => Promise<{}>;
declare const BIND_logDebug:    (...args: any[]) => Promise<{}>;
declare const BIND_logInfo:     (...args: any[]) => Promise<{}>;
declare const BIND_logWarn:     (...args: any[]) => Promise<{}>;
declare const BIND_logError:    (...args: any[]) => Promise<{}>;
declare const BIND_logCritical: (...args: any[]) => Promise<{}>;

declare const BIND_readFile:    (...args: any[]) => Promise<{}>;
declare const BIND_writeFile:   (...args: any[]) => Promise<{}>;
declare const BIND_exists:      (...args: any[]) => Promise<{}>;
declare const BIND_isDir:       (...args: any[]) => Promise<{}>;
declare const BIND_mkDir:       (...args: any[]) => Promise<{}>;
declare const BIND_rm:          (...args: any[]) => Promise<{}>;
declare const BIND_ls:          (...args: any[]) => Promise<{}>;
declare const BIND_rename:      (...args: any[]) => Promise<{}>;
declare const BIND_copy:        (...args: any[]) => Promise<{}>;
declare const BIND_chooseFiles: (...args: any[]) => Promise<{}>;

declare const BIND_saveSettings:            (...args: any[]) => Promise<{}>;
declare const BIND_refreshSettings:         (...args: any[]) => Promise<{}>;
declare const BIND_reloadPage:              (...args: any[]) => Promise<{}>;
declare const BIND_setSettings:             (...args: any[]) => Promise<{}>;
declare const BIND_getSettings:             (...args: any[]) => Promise<{}>;
declare const BIND_resetSettingsToDefaults: (...args: any[]) => Promise<{}>;
declare const BIND_updateSize:              (...args: any[]) => Promise<{}>;
declare const BIND_close:                   (...args: any[]) => Promise<{}>;
declare const BIND_openWindow:              (...args: any[]) => Promise<{}>;
declare const BIND_minimize:                (...args: any[]) => Promise<{}>;
declare const BIND_maximize:                (...args: any[]) => Promise<{}>;
declare const BIND_fullscreen:              (...args: any[]) => Promise<{}>;
declare const BIND_hide:                    (...args: any[]) => Promise<{}>;
declare const BIND_show:                    (...args: any[]) => Promise<{}>;

declare const BIND_getPID:                (...args: any[]) => Promise<{}>;
declare const BIND_getOS:                 (...args: any[]) => Promise<{}>;
declare const BIND_getApplicationDirPath: (...args: any[]) => Promise<{}>;
declare const BIND_sendNotif:             (...args: any[]) => Promise<{}>;
declare const BIND_openURI:               (...args: any[]) => Promise<{}>;


