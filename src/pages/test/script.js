import {sync, Log, Util, Window, FS} from '../../dist/lib/renweb/index.js'
document.addEventListener("keydown", async (e) => {
    if (e.ctrlKey) {
        if (e.key === 'q') {
            await Log.debug("CTRL + q was pressed.");
            await Window.close();
            return;
        } else if (e.key === 'r') {
            await Log.debug("CTRL + r was pressed.");
            await Window.reloadPage();
            return;
        } else if (e.key === 's') {
            await Log.debug("CTRL + s was pressed.");
            await Window.saveSettings();
            return;
        }
    }
});
console.log = (async (msg) => await Log.debug(msg));
window.onresize = async () => {
    await Window.updateSize();
}
window.onload = async () => {
    await Log.info("Window content has been loaded.");
    await Window.show();
}
document.querySelector(".log_trace").onclick = async () => {
    await Log.trace(document.querySelector(".log_msg").value);
};
document.querySelector(".log_debug").onclick = async () => {
    await Log.debug(document.querySelector(".log_msg").value);
};
document.querySelector(".log_info").onclick = () => {
    sync(Log.info(document.querySelector(".log_msg").value));
};
document.querySelector(".log_warn").onclick = async () => {
    await Log.warn(document.querySelector(".log_msg").value);
};
document.querySelector(".log_error").onclick = async () => {
    await Log.error(document.querySelector(".log_msg").value);
};
document.querySelector(".log_critical").onclick = async () => {
    await Log.critical(document.querySelector(".log_msg").value);
};

document.querySelector(".read_file").onclick = async () => {
    const filename = document.querySelector(".read_file_msg").value;
    await Log.debug(`Reading file "${filename}"`);
    const contents = await FS.readFile(filename);
    if (contents == null) {
        document.querySelector(".read_file").style.backgroundColor = "red";
        document.querySelector(".read_file_output").textContent = "[ERROR] file either doesn't exist or can't be opened";
    } else {
        document.querySelector(".read_file").style.backgroundColor = "green";
        document.querySelector(".read_file_output").textContent = Util.fromUint8array(contents);
    }
};

document.querySelector(".write_file").onclick = async () => {
    const filename = document.querySelector(".write_file_msg").value;
    await Log.debug(`Writing to file "${filename}"`);
    const contents = document.querySelector(".write_file_input").value;
    const append = document.querySelector(".append").checked;
    const res = await FS.writeFile(filename, contents, {append: append});
    if (res) {
        document.querySelector(".write_file").style.backgroundColor = "green";
    } else {
        document.querySelector(".write_file").style.backgroundColor = "red";
    }
};


document.querySelector(".exists_file").onclick = async () => {
    const filename = document.querySelector(".write_file_msg").value;
    await Log.debug(`Does file/dir "${filename}" exist?`);
    const res = await FS.exists(filename);
    if (res) {
        document.querySelector(".exists_file").style.backgroundColor = "green";
    } else {
        document.querySelector(".exists_file").style.backgroundColor = "red";
    }
};

document.querySelector(".remove_file").onclick = async () => {
    const filename = document.querySelector(".write_file_msg").value;
    await Log.debug(`Removing file "${filename}"`);
    const recursive = document.querySelector(".recursive").checked;
    const res = await FS.rm(filename, {recursive: recursive});
    if (res) {
        document.querySelector(".remove_file").style.backgroundColor = "green";
    } else {
        document.querySelector(".remove_file").style.backgroundColor = "red";
    }

};

document.querySelector(".is_dir").onclick = async () => {
    const filename = document.querySelector(".write_file_msg").value;
    await Log.debug(`Is "${filename}" a dir?`);
    const res = await FS.isDir(filename);
    if (res) {
        document.querySelector(".is_dir").style.backgroundColor = "green";
    } else {
        document.querySelector(".is_dir").style.backgroundColor = "red";
    }
};

document.querySelector(".mk_dir").onclick = async () => {
    const filename = document.querySelector(".write_file_msg").value;
    await Log.debug(`Making dir "${filename}"`);
    const res = await FS.mkDir(filename);
    if (res) {
        document.querySelector(".mk_dir").style.backgroundColor = "green";
    } else {
        document.querySelector(".mk_dir").style.backgroundColor = "red";
    }
};

document.querySelector(".ls_dir").onclick = async () => {
    const filename = document.querySelector(".write_file_msg").value;
    await Log.debug(`Listing dir "${filename}"`);
    const dirs = await FS.ls(filename);
    if (dirs == null) {
        document.querySelector(".ls_dir").style.backgroundColor = "red";
        document.querySelector(".ls_dir_output").textContent = "[ERROR] dir listed either doesn't exist or isn't a dir";
    } else {
        document.querySelector(".ls_dir").style.backgroundColor = "green";
        if (dirs.length == 0) {
            document.querySelector(".ls_dir_output").textContent = "empty";
            return;
        }
        let str = (filename.endsWith("/")) ? `${filename}\n` : `${filename}\\\n`;
        for (const i of dirs) {
            str += ` ├─ ${i}\n`;
        }
        document.querySelector(".ls_dir_output").textContent = str;
    }
};

document.querySelector(".rename_file").onclick = async () => {
    const orig_filename = document.querySelector(".write_file_msg").value;
    const new_filename = document.querySelector(".new_file_msg").value;
    const overwrite = document.querySelector(".overwrite").checked;
    await Log.debug(`Renaming to "${new_filename}"`);
    const res = await FS.rename(orig_filename, new_filename, {overwrite: overwrite});
    if (res) {
        document.querySelector(".rename_file").style.backgroundColor = "green";
    } else {
        document.querySelector(".rename_file").style.backgroundColor = "red";
    }
};

document.querySelector(".copy_file").onclick = async () => {
    const orig_filename = document.querySelector(".write_file_msg").value;
    const new_filename = document.querySelector(".new_file_msg").value;
    const overwrite = document.querySelector(".overwrite").checked;
    await Log.debug(`Copying to to "${new_filename}"`);
    const res = await FS.copy(orig_filename, new_filename, {overwrite: overwrite});
    if (res) {
        document.querySelector(".copy_file").style.backgroundColor = "green";
    } else {
        document.querySelector(".copy_file").style.backgroundColor = "red";
    }
};

document.querySelector(".choose_files").onclick = async () => {
    const dirs = document.querySelector(".dirs").checked;
    const multiple = document.querySelector(".multiple").checked;
    await Log.debug(`Opening file dialog...`);
    const files = await FS.chooseFiles({multiple: multiple, dirs: dirs});
    if (files == null) {
        document.querySelector(".choose_files").style.backgroundColor = "red";
        document.querySelector(".choose_files_output").textContent = "[ERROR] File dialog cancelled";
    } else {
        document.querySelector(".choose_files").style.backgroundColor = "green";
        if (files.length == 0) {
            document.querySelector(".choose_files_output").textContent = "empty";
            return;
        }
        let str = `Files chosen:\n`;
        for (const i of files) {
            str += ` ├─ ${i}\n`;
        }
        document.querySelector(".choose_files_output").textContent = str;
    }
};

async function getSettings() {
    const settings = await Window.getSettings();
    // await Log.debug(settings);
    document.querySelector(".settings_output").value = JSON.stringify(settings, null, 2);
    return settings;
}

document.querySelector(".get_settings").onclick = async () => {
    await Log.debug(`Getting settings...`);
    await getSettings();
};
document.querySelector(".set_settings").onclick = async () => {
    await Log.debug(`Setting settings...`);
    const settings = JSON.parse(document.querySelector(".settings_output").value);
    await Log.debug(settings);
    await Window.setSettings(settings);
};
document.querySelector(".refresh_settings").onclick = async () => {
    await Log.debug(`Refreshing settings...`);
    await Window.refreshSettings();
};
document.querySelector(".save_settings").onclick = async () => {
    await Log.debug(`Saving settings...`);
    await Window.saveSettings();
};
document.querySelector(".reset_settings").onclick = async () => {
    await Log.debug(`Resetting settings...`);
    await Window.resetSettingsToDefaults();
};


document.querySelector(".reload").onclick = async () => {
    await Log.debug(`Reloading...`);
    await Window.reloadPage();
};
document.querySelector(".close").onclick = async () => {
    await Log.debug(`Closing...`);
    await Window.close();
};
document.querySelector(".minimize").onclick = async () => {
    await Log.debug(`Minimizing...`);
    await Window.minimize();
};
document.querySelector(".maximize").onclick = async () => {
    await Log.debug(`Maximizing...`);
    await Window.maximize();
};
document.querySelector(".fullscreen").onclick = async () => {
    await Log.debug(`Fullscreen...`);
    await Window.fullscreen();
};

document.querySelector(".open_window").onclick = async () => {
    const window_name = document.querySelector(".open_window_msg").value;
    const single = document.querySelector(".single").checked;
    await Log.debug(`Opening window "${window_name}"`);
    Window.openWindow(window_name, {single: single});
};





(async () => {
    const path = await Util.getApplicationDirPath();
    document.querySelector(".read_file_msg").value = path;
    document.querySelector(".write_file_msg").value = path;
    document.querySelector(".new_file_msg").value = path;
    document.querySelector(".read_file_msg").value = path;
    await getSettings();
})();