#include "../include/webview_binds.hpp"

// Surrounds a string with quotes
std::string jsonStr(std::string str) {
    return std::string('"' + str + '"');
}

// Removes the surrounding quotes from a JSON dumped string
std::string jsonDestr(std::string str) {
    return str.substr(1, str.length()-2);
} 

// Gets JSON contents as a usable std::string
std::string jsonTostr(json json_v, int indent=-1) {
    std::string json_str = json_v.dump(indent);
    if (json_v.is_string()) {
        json_str = jsonDestr(json_str);
    }
    return json_str;
}

// Returns the contents parsed from the params to be output in the log
std::string jsonLogBindingHandler(const std::string& req) {
    json params = json::parse(req);
    json param0 = params[0];
    return jsonTostr(param0, 0);
}

std::vector<char> jsonUint8arrToVec(json json_v) {
    try {
        if (!json_v.is_object() && !json_v.is_array()) {
            throw std::runtime_error(std::string("Variable passed isn't a uint8array and can't be converted to one! A ") + json_v.type_name() + " was recieved.");
        }
        size_t n = json_v.size();
        std::vector<char> buffer(n);
        for (const auto& [key, value] : json_v.items()) {
            size_t index = static_cast<size_t>(std::stoul(key));
            if (index >= n) continue; // Safety check in case of malformed data
            int val = json_v.value(key, -1);
            if (val == -1) {
                throw std::runtime_error("Invalid value in binary string: " + jsonTostr(value));
            }
            buffer[index] = static_cast<char>(val);
        }
        return buffer;
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        return std::vector<char>();
    }
}


std::string jsonUint8arrToString(json json_v) {
    std::vector<char> vec = jsonUint8arrToVec(json_v);
    if (vec.empty()) return "";
    else return std::string{vec.begin(), vec.end()};
}



// ------------------------------------------------------------
// ------------------------------------------------------------

void RenWeb::Bindings::Log::bind_logTrace(RenWeb::Window* w) {
    const std::string fn_name = "BIND_logTrace";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (msg: Uint8array)
        std::string msg = jsonUint8arrToString(json::parse(req)[0]);
        spdlog::trace("[CLIENT] " + msg);
        return "null";
    });
}
void RenWeb::Bindings::Log::bind_logDebug(RenWeb::Window* w) {
    const std::string fn_name = "BIND_logDebug";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (msg: Uint8array)
        std::string msg = jsonUint8arrToString(json::parse(req)[0]);
        spdlog::debug("[CLIENT] " + msg);
        return "null";
    });
}
void RenWeb::Bindings::Log::bind_logInfo(RenWeb::Window* w) {
    const std::string fn_name = "BIND_logInfo";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (msg: Uint8array)
        std::string msg = jsonUint8arrToString(json::parse(req)[0]);
        spdlog::info("[CLIENT] " + msg);
        return "null";
    });
}
void RenWeb::Bindings::Log::bind_logWarn(RenWeb::Window* w) {
    const std::string fn_name = "BIND_logWarn";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (msg: Uint8array)
        std::string msg = jsonUint8arrToString(json::parse(req)[0]);
        spdlog::warn("[CLIENT] " + msg);
        return "null";
    });
}
void RenWeb::Bindings::Log::bind_logError(RenWeb::Window* w) {
    const std::string fn_name = "BIND_logError";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (msg: Uint8array)
        std::string msg = jsonUint8arrToString(json::parse(req)[0]);
        spdlog::error("[CLIENT] " + msg);
        return "null";
    });
}
void RenWeb::Bindings::Log::bind_logCritical(RenWeb::Window* w) {
    const std::string fn_name = "BIND_logCritical";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (msg: Uint8array)
        std::string msg = jsonUint8arrToString(json::parse(req)[0]);
        spdlog::critical("[CLIENT] " + msg);
        return "null";
    });
}

// ------------------------------------------------------------
// ------------------------------------------------------------

void RenWeb::Bindings::Filesystem::bind_readFile(RenWeb::Window* w) {
    const std::string fn_name = "BIND_readFile";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (path: string)
        std::filesystem::path path (jsonTostr(json::parse(req)[0]));
        if (!std::filesystem::exists(path)) {
            spdlog::error("No file exists at " + path.string());
            return "null";
        }
        else if (std::filesystem::is_directory(path)) {
            spdlog::error("readFile can't read directory contents. Use ls for that.");
            return "null";
        }
        std::ifstream file(path, std::ios::binary); // open in binary mode
        if (!file) {
            spdlog::error("Failed to open file for reading: " + path.string());
            return "null";
        }
        std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
        file.close();
        spdlog::debug("Read " + std::to_string(buffer.size()) + " bytes from " + path.string());
        json uint8arr(buffer);
        return uint8arr.dump();
    });
}

void RenWeb::Bindings::Filesystem::bind_writeFile(RenWeb::Window* w) { 
    const std::string fn_name = "BIND_writeFile";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (path: string, contents: Uint8array, settings: {append: boolean=false})
        json params = json::parse(req);     
        std::filesystem::path path (jsonTostr(params[0]));
        std::vector<char> uint8array = (jsonUint8arrToVec(params[1]));
        bool append = (params[2]["append"].is_boolean() && (params[2]["append"].dump() == "true")) ? true : false;
        std::ios::openmode mode = std::ios::binary;
        if (append) {
            mode |= std::ios::app;
        } else {
            mode |= std::ios::trunc;
        }
        std::filesystem::path parent_path = path.parent_path();
        if (std::filesystem::is_directory(path)) {
            spdlog::error(std::string("Can't write to a directory ") + path.string());
            return "false";
        } else if (!std::filesystem::exists(parent_path)) {
            spdlog::error(std::string("Directory '") + parent_path.string() + "' doesn't exist.");
            return "false";
        }
        std::ofstream file(path, mode);
        if (file.bad()) {
            spdlog::error(std::string("Bad file ") + path.string());
            return "false";
        }
        if (uint8array.empty()) {
            spdlog::debug("Input content empty. Attempting empty write");
        }
        file.write(uint8array.data(), uint8array.size());
        file.close();
        spdlog::debug(((append) ? "Appended " : "Wrote ") + std::to_string(uint8array.size()) + " bytes to " + path.string());
        return "true";
    });
}
void RenWeb::Bindings::Filesystem::bind_exists(RenWeb::Window* w) {
    const std::string fn_name = "BIND_exists";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (path: string)
        std::filesystem::path path (jsonTostr(json::parse(req)[0]));
        return (std::filesystem::exists(path)) ? "true" : "false";
    });
}
void RenWeb::Bindings::Filesystem::bind_isDir(RenWeb::Window* w) {
    const std::string fn_name = "BIND_isDir";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (path: string)
        std::filesystem::path path (jsonTostr(json::parse(req)[0]));
        return (std::filesystem::is_directory(path)) ? "true" : "false";
    });
}
void RenWeb::Bindings::Filesystem::bind_mkDir(RenWeb::Window* w) {
    const std::string fn_name = "BIND_mkDir";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (path: string)
        std::filesystem::path path (jsonTostr(json::parse(req)[0]));
        if (std::filesystem::exists(path)) {
            spdlog::error("File/dir already exists at '" + path.string() + "'");
            return "false";
        }
        std::error_code ec;
        std::filesystem::create_directory(path, ec);
        if (ec) {
            spdlog::error(ec.message());
            return "false";
        } else {
            return "true";
        }
    });
}
void RenWeb::Bindings::Filesystem::bind_rm(RenWeb::Window* w) {
    const std::string fn_name = "BIND_rm";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (path: string, settings: {recursive: boolean=false})
        json params = json::parse(req);     
        std::filesystem::path path (jsonTostr(params[0]));
        bool recursive = (params[1]["recursive"].is_boolean() && (params[1]["recursive"].dump() == "true")) ? true : false;
        std::error_code ec;
        if (!std::filesystem::exists(path)) {
            spdlog::error("Cannot delete file/dir that doesn't exist: " + path.string());
            return "false";
        } else if (std::filesystem::is_directory(path)) {
            if (recursive) {
                std::filesystem::remove_all(path, ec);
            } else {
                std::filesystem::remove(path, ec);
            }
            if (ec) {
                spdlog::error(ec.message());
                return "false";
            } else {
                return "true";
            }
        }
        std::filesystem::remove(path, ec);
        if (ec) {
            spdlog::error(ec.message());
            return "false";
        } else {
            return "true";
        }
    });
}
void RenWeb::Bindings::Filesystem::bind_ls(RenWeb::Window* w) {
    const std::string fn_name = "BIND_ls";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (path: string)
        std::filesystem::path path (jsonTostr(json::parse(req)[0]));
        if (!std::filesystem::is_directory(path)) {
            spdlog::error("Path entered to ls wasn't a dir: " + path.string());
            return "null";
        }
        std::error_code ec;
        json array = json::array();
        for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
            array.push_back(entry.path().string());
        }
        if (ec) {
            spdlog::error(ec.message());
            return "null";
        } else {
            return array.dump();
        }
    });
}
void RenWeb::Bindings::Filesystem::bind_rename(RenWeb::Window* w) {
    const std::string fn_name = "BIND_rename";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (orig_path: string, new_path: string, settings: {overwrite: boolean=false})
        json params = json::parse(req);     
        std::filesystem::path orig_path (jsonTostr(params[0]));
        std::filesystem::path new_path (jsonTostr(params[1]));
        bool overwrite = (params[2]["overwrite"].is_boolean() && (params[2]["overwrite"].dump() == "true")) ? true : false;
        std::error_code ec;
        if (!std::filesystem::exists(orig_path)) {
            spdlog::error("Can't rename path that doesn't exist: " + orig_path.string());
            return "null";
        } else if (std::filesystem::exists(new_path) && !overwrite) {
            spdlog::error("Can't overwrite already-existing new path if settings.overwrite is false: " + new_path.string());
            return "null";
        } else if (std::filesystem::exists(new_path)) {
            if (std::filesystem::is_directory(new_path)) {
                std::filesystem::remove_all(new_path, ec);
            } else {
                std::filesystem::remove(new_path, ec);
            }
            if (ec) {
                spdlog::error(ec.message());
                return "false";
            }
        }
        std::filesystem::rename(orig_path, new_path, ec);
        if (ec) {
            spdlog::error(ec.message());
            return "false";
        } else {
            return "true";
        }
    });
}
void RenWeb::Bindings::Filesystem::bind_copy(RenWeb::Window* w) {
    const std::string fn_name = "BIND_copy";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // (orig_path: string, new_path: string, settings: {overwrite: boolean=false})
        json params = json::parse(req);     
        std::filesystem::path orig_path (jsonTostr(params[0]));
        std::filesystem::path new_path (jsonTostr(params[1]));
        bool overwrite = (params[2]["overwrite"].is_boolean() && (params[2]["overwrite"].dump() == "true")) ? true : false;
        std::error_code ec;
        if (!std::filesystem::exists(orig_path)) {
            spdlog::error("Can't copy path that doesn't exist: " + orig_path.string());
            return "null";
        } else if (std::filesystem::exists(new_path) && !overwrite) {
            spdlog::error("Can't overwrite already-existing new path if settings.overwrite is false: " + new_path.string());
            return "null";
        } else if (std::filesystem::exists(new_path)) {
            if (std::filesystem::is_directory(new_path)) {
                std::filesystem::remove_all(new_path, ec);
            } else {
                std::filesystem::remove(new_path, ec);
            }
            if (ec) {
                spdlog::error(ec.message());
                return "false";
            }
        }
        if (std::filesystem::is_directory(orig_path)) {
            std::filesystem::copy(orig_path, new_path, std::filesystem::copy_options::recursive, ec);
        } else {
            std::filesystem::copy(orig_path, new_path, ec);
        }
        if (ec) {
            spdlog::error(ec.message());
            return "false";
        } else {
            return "true";
        }
    });
}
void RenWeb::Bindings::Filesystem::bind_chooseFiles(RenWeb::Window* w) {
    const std::string fn_name = "BIND_chooseFiles";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ({multiple: boolean, dirs: boolean, (patterns | mimes): [name: string, rules: string[]]})
        json settings = (json::parse(req)[0]);
        std::vector<std::string> filepaths;
        if (settings.is_object()) {
            bool multi = false;
            bool dir = false;
            multi = settings.value("multiple", false);
            dir = settings.value("dirs", false);
            spdlog::info("multiple: " + std::to_string(multi) + " dirs: " + std::to_string(dir));
            RenWeb::ChooseFileDialogSettings* filteration = nullptr;
            json filter_arr;
            auto set_filters = [&](const std::string filter_name){
                if ((filter_arr = settings[filter_name]).is_array()) {
                    if (filteration == nullptr) filteration = new ChooseFileDialogSettings();
                    for (auto& i : filter_arr) {
                        if (i.is_array() && i[1].is_array()) {
                            auto filter_pair = std::make_pair<std::string, std::vector<std::string>>(jsonTostr(i[0]), {});
                            for (auto& j : i[1]) {
                                filter_pair.second.push_back(jsonTostr(j));
                            }
                            filteration->patterns.push_back(filter_pair);
                        }
                    }
                }
            };
            set_filters("patterns");
            set_filters("mimes");
            filepaths = w->openChooseFilesDialog(multi, dir, filteration);
            if (filteration != nullptr) delete filteration;
        } else {
            filepaths = w->openChooseFilesDialog();
        }
        if (filepaths.empty()) {
            spdlog::info("It seems no file was chosen");
            return "null";
        }
        json filenames_arr = json::array();
        for (auto& i : filepaths) {
            filenames_arr.push_back(i);
        }
        return filenames_arr.dump();
    });
}


// ------------------------------------------------------------
// ------------------------------------------------------------

void RenWeb::Bindings::Webview::bind_saveSettings(RenWeb::Window* w) {
    const std::string fn_name = "BIND_saveSettings";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        RenWeb::Info::App::save();
        return "null";
    });
}
void RenWeb::Bindings::Webview::bind_refreshSettings(RenWeb::Window* w) {
    const std::string fn_name = "BIND_refreshSettings";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->refreshSettings();
        return "null";
    });
}
void RenWeb::Bindings::Webview::bind_reloadPage(RenWeb::Window* w) {
    const std::string fn_name = "BIND_reloadPage";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->reloadPage();
        return "null";
    });
}

void RenWeb::Bindings::Webview::bind_setSettings(RenWeb::Window* w) {
    const std::string fn_name = "BIND_setSettings";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // (settings: (Settings & {})
        json settings = json::parse(req)[0];
        if (settings.is_object()) {
            RenWeb::Info::App::set(std::move(settings));
        } else {
            spdlog::error(std::string("An object is required to set settings. Recieved ") + settings.type_name());
        }
        return "null";
    });
}
void RenWeb::Bindings::Webview::bind_getSettings(RenWeb::Window* w) {
    const std::string fn_name = "BIND_getSettings";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        return RenWeb::Info::App::get().dump(2);
    });
}
void RenWeb::Bindings::Webview::bind_resetSettingsToDefaults(RenWeb::Window* w) {
    const std::string fn_name = "BIND_resetSettingsToDefaults";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        RenWeb::Info::App::resetToDefaults();
        // w->refreshSettings();
        return "null";
    });
}

void RenWeb::Bindings::Webview::bind_updateSize(RenWeb::Window* w) {
    const std::string fn_name = "BIND_updateSize";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // (force: boolean=false)
        json params = json::parse(req);
        bool forced = (params[0].is_boolean() && (params[0].dump() == "true")) ? true : false;
        w->updateSize(forced);
        return "null";
    }); 
}

void RenWeb::Bindings::Webview::bind_close(RenWeb::Window* w) {
    const std::string fn_name = "BIND_close";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->terminateWindow();
        return "null";
    }); 
}

void RenWeb::Bindings::Webview::bind_openWindow(RenWeb::Window* w) {
    const std::string fn_name = "BIND_openWindow";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // (uri: string="_", settings: {single: boolean=false})
        json params = json::parse(req);
        std::string uri = jsonTostr(params[0]);
        bool single = (params[1]["single"].is_boolean() && (params[1]["single"].dump() == "true")) ? true : false;
        if (uri.empty() || (uri == "_")) {
            spdlog::debug(std::string("Attempting to start duplicate process. Single? ") + ((single) ? "true" : "false"));
            uri = RenWeb::Info::App::page;
        }
        if (single) {
            if (!w->process_manager.hasProcess(uri)) {
                spdlog::debug("Attempting to start single process for uri '" + uri + "'");
                w->process_manager.startProcess(uri);
            } else {
                spdlog::debug("Process of name '" + uri + "' is already running. Bringing it to foreground...");
                w->process_manager.bringToForeground(uri);
            }
        } else {
            spdlog::debug("Attempting to start process for uri '" + uri + "'");
            w->process_manager.startProcess(uri);
        }
        return "null";
    }); 
}
void RenWeb::Bindings::Webview::bind_minimize(RenWeb::Window* w) {
    const std::string fn_name = "BIND_minimize";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->minimize();
        return "null";
    }); 
}
void RenWeb::Bindings::Webview::bind_maximize(RenWeb::Window* w) {
    const std::string fn_name = "BIND_maximize";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->maximize();
        return "null";
    }); 
}
void RenWeb::Bindings::Webview::bind_fullscreen(RenWeb::Window* w) {
    const std::string fn_name = "BIND_fullscreen";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->fullscreen();
        return "null";
    }); 
}
void RenWeb::Bindings::Webview::bind_hide(RenWeb::Window* w) {
    const std::string fn_name = "BIND_hide";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->hide();
        return "null";
    }); 
}
void RenWeb::Bindings::Webview::bind_show(RenWeb::Window* w) {
    const std::string fn_name = "BIND_show";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        w->show();
        return "null";
    }); 
}
// ------------------------------------------------------------
// ------------------------------------------------------------

void RenWeb::Bindings::Util::bind_getPID(RenWeb::Window* w) {
    const std::string fn_name = "BIND_getPID";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        return std::to_string(getpid());
    });
}

void RenWeb::Bindings::Util::bind_getApplicationDirPath(RenWeb::Window* w) {
    const std::string fn_name = "BIND_getApplicationDirPath";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        // ()
        boost::ignore_unused(req);
        return jsonStr(RenWeb::Info::File::dir);
    });
}

void RenWeb::Bindings::bind_all(RenWeb::Window* w) {
    RenWeb::Bindings::Log::bind_logTrace(w);
    RenWeb::Bindings::Log::bind_logDebug(w);
    RenWeb::Bindings::Log::bind_logInfo(w);
    RenWeb::Bindings::Log::bind_logWarn(w);
    RenWeb::Bindings::Log::bind_logError(w);
    RenWeb::Bindings::Log::bind_logCritical(w);

    RenWeb::Bindings::Filesystem::bind_readFile(w);
    RenWeb::Bindings::Filesystem::bind_writeFile(w);
    RenWeb::Bindings::Filesystem::bind_exists(w);
    RenWeb::Bindings::Filesystem::bind_isDir(w);
    RenWeb::Bindings::Filesystem::bind_mkDir(w);
    RenWeb::Bindings::Filesystem::bind_rm(w);
    RenWeb::Bindings::Filesystem::bind_ls(w);
    RenWeb::Bindings::Filesystem::bind_rename(w);
    RenWeb::Bindings::Filesystem::bind_copy(w);
    RenWeb::Bindings::Filesystem::bind_chooseFiles(w);

    RenWeb::Bindings::Webview::bind_saveSettings(w);
    RenWeb::Bindings::Webview::bind_refreshSettings(w);
    RenWeb::Bindings::Webview::bind_reloadPage(w);
    RenWeb::Bindings::Webview::bind_setSettings(w);
    RenWeb::Bindings::Webview::bind_getSettings(w);
    RenWeb::Bindings::Webview::bind_resetSettingsToDefaults(w);
    RenWeb::Bindings::Webview::bind_updateSize(w);
    RenWeb::Bindings::Webview::bind_close(w);
    RenWeb::Bindings::Webview::bind_openWindow(w);
    RenWeb::Bindings::Webview::bind_minimize(w);
    RenWeb::Bindings::Webview::bind_maximize(w);
    RenWeb::Bindings::Webview::bind_fullscreen(w);
    RenWeb::Bindings::Webview::bind_hide(w);
    RenWeb::Bindings::Webview::bind_show(w);

    RenWeb::Bindings::Util::bind_getPID(w);
    RenWeb::Bindings::Util::bind_getApplicationDirPath(w);
}