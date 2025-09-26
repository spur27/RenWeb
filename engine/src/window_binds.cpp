#include "../include/window.hpp"
#include "../include/window_binds_helper.hpp"
#include <boost/process/environment.hpp>

using namespace RenWeb;
using namespace RenWeb::BindingHelpers;

RenWeb::Window* RenWeb::Window::bindAll() {
    RenWeb::Window* w = this;
  // LOGGING
    this->bindFunction("BIND_logTrace", [](const std::string& req) -> std::string {
            // (msg: Uint8array)
            std::string msg = jsonUint8arrToString(json::parse(req)[0]);
            spdlog::trace("[CLIENT] " + msg);
            return "null";
        })
        ->bindFunction("BIND_logDebug", [](const std::string& req) -> std::string {
            // (msg: Uint8array)
            std::string msg = jsonUint8arrToString(json::parse(req)[0]);
            spdlog::debug("[CLIENT] " + msg);
            return "null";
        })
        ->bindFunction("BIND_logInfo", [](const std::string& req) -> std::string {
            // (msg: Uint8array)
            std::string msg = jsonUint8arrToString(json::parse(req)[0]);
            spdlog::info("[CLIENT] " + msg);
            return "null";
        })
        ->bindFunction("BIND_logWarn", [](const std::string& req) -> std::string {
            // (msg: Uint8array)
            std::string msg = jsonUint8arrToString(json::parse(req)[0]);
            spdlog::warn("[CLIENT] " + msg);
            return "null";
        })
        ->bindFunction("BIND_logError", [](const std::string& req) -> std::string {
            // (msg: Uint8array)
            std::string msg = jsonUint8arrToString(json::parse(req)[0]);
            spdlog::error("[CLIENT] " + msg);
            return "null";
        })
        ->bindFunction("BIND_logCritical", [](const std::string& req) -> std::string {
            // (msg: Uint8array)
            std::string msg = jsonUint8arrToString(json::parse(req)[0]);
            spdlog::critical("[CLIENT] " + msg);
            return "null";
        })
  // FILE SYSTEM
        ->bindFunction("BIND_readFile", [](const std::string& req) -> std::string {
            // (path: string)
            std::filesystem::path path (jsonUint8arrToString(json::parse(req)[0]));
            if (!std::filesystem::exists(path)) {
                spdlog::error("No file exists at " + path.string());
                return "null";
            }
            else if (std::filesystem::is_directory(path)) {
                spdlog::error("readFile can't read directory contents. Use ls for that.");
                return "null";
            }
            std::ifstream file(path, std::ios::binary); // open in binary mode
            if (!file.good()) {
                spdlog::error("Failed to open file for reading: " + path.string());
                return "null";
            }
            std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
            file.close();
            spdlog::debug("Read " + std::to_string(buffer.size()) + " bytes from " + path.string());
            json uint8arr(buffer);
            return uint8arr.dump();
        })
        ->bindFunction("BIND_writeFile", [](const std::string& req) -> std::string {
            // (path: string, contents: Uint8array, settings: {append: boolean=false})
            json params = json::parse(req);     
            std::filesystem::path path (jsonUint8arrToString(params[0]));
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
        })
        ->bindFunction("BIND_exists", [](const std::string& req) -> std::string {
            // (path: string)
            std::filesystem::path path (jsonUint8arrToString(json::parse(req)[0]));
            return (std::filesystem::exists(path)) ? "true" : "false";
        })
        ->bindFunction("BIND_isDir", [](const std::string& req) -> std::string {
            // (path: string)
            std::filesystem::path path (jsonUint8arrToString(json::parse(req)[0]));
            return (std::filesystem::is_directory(path)) ? "true" : "false";
        })
        ->bindFunction("BIND_mkDir", [](const std::string& req) -> std::string {
            // (path: string)
            std::filesystem::path path (jsonUint8arrToString(json::parse(req)[0]));
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
        })
        ->bindFunction("BIND_rm", [](const std::string& req) -> std::string {
            // (path: string, settings: {recursive: boolean=false})
            json params = json::parse(req);     
            std::filesystem::path path (jsonUint8arrToString(params[0]));
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
        })
        ->bindFunction("BIND_ls", [](const std::string& req) -> std::string {
            // (path: string)
            std::filesystem::path path (jsonUint8arrToString(json::parse(req)[0]));
            if (!std::filesystem::is_directory(path)) {
                spdlog::error("Path entered to ls wasn't a dir: " + path.string());
                return "null";
            }
            std::error_code ec;
            json array = json::array();
            for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
                array.push_back(strToUint8arrVec(formatPath(entry.path().string())));
            }
            if (ec) {
                spdlog::error(ec.message());
                return "null";
            } else {
                return array.dump();
            }
        })
        ->bindFunction("BIND_rename", [](const std::string& req) -> std::string {
            // (orig_path: string, new_path: string, settings: {overwrite: boolean=false})
            json params = json::parse(req);     
            std::filesystem::path orig_path (jsonUint8arrToString(params[0]));
            std::filesystem::path new_path (jsonUint8arrToString(params[1]));
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
        })
        ->bindFunction("BIND_copy", [](const std::string& req) -> std::string {
            // (orig_path: string, new_path: string, settings: {overwrite: boolean=false})
            json params = json::parse(req);     
            std::filesystem::path orig_path (jsonUint8arrToString(params[0]));
            std::filesystem::path new_path (jsonUint8arrToString(params[1]));
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
        })
        ->bindFunction("BIND_chooseFiles",[w](const std::string& req) -> std::string {
            // ({multiple: boolean, dirs: boolean, (patterns | mimes): [name: string, rules: string[]]})
            bool multi = jsonToStr(json::parse(req)[0]) == "true";
            bool dirs = jsonToStr(json::parse(req)[1]) == "true";
            std::vector<std::string> filtration_vec = {};
            json filtration = json::parse(req)[2];
            if (filtration.is_array()) {
                for (auto& i : filtration) {
                    filtration_vec.push_back(jsonUint8arrToString(i));
                }
            }
            json initial_path = json::parse(req)[3];
            std::string initial_path_str = RenWeb::Info::File::dir;
            if (initial_path.is_string()) {
                initial_path_str = jsonToStr(initial_path);
            }
            json num_vec_vec = json::array();
            for (auto i : w->openChooseFilesDialog(multi, dirs, filtration_vec, initial_path_str)) {
                num_vec_vec.push_back(json(strToUint8arrVec(i)));
            }
            return num_vec_vec.dump();
        })
  // SETTINGS
        ->bindFunction("BIND_saveSettings", [](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            RenWeb::Info::App::save();
            return "null";
        })
        ->bindFunction("BIND_refreshSettings", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->refreshSettings();
            return "null";
        })
        ->bindFunction("BIND_reloadPage", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->reloadPage();
            return "null";
        })
        ->bindFunction("BIND_setSettings", [](const std::string& req) -> std::string {
            // (settings: (Settings & {})
            json settings = json::parse(req)[0];
            if (settings.is_object()) {
                RenWeb::Info::App::set(std::move(settings));
            } else {
                spdlog::error(std::string("An object is required to set settings. Recieved ") + settings.type_name());
            }
            return "null";
        })
        ->bindFunction("BIND_getSettings", [](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            return RenWeb::Info::App::get().dump(2);
        })
        ->bindFunction("BIND_resetSettingsToDefaults", [](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            RenWeb::Info::App::resetToDefaults();
            // w->refreshSettings();
            return "null";
        })
        ->bindFunction("BIND_updateSize", [w](const std::string& req) -> std::string {
            // (force: boolean=false)
            json params = json::parse(req);
            bool forced = (params[0].is_boolean() && (params[0].dump() == "true")) ? true : false;
            w->updateSize(forced);
            return "null";
        })
        ->bindFunction("BIND_close", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->terminateWindow();
            return "null";
        })
        ->bindFunction("BIND_openWindow", [w](const std::string& req) -> std::string {
            // (uri: string="_", settings: {single: boolean=false})
            json params = json::parse(req);
            std::string uri = jsonUint8arrToString(params[0]);
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
        })
        ->bindFunction("BIND_minimize", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->minimize();
            return "null";
        })
        ->bindFunction("BIND_maximize", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->maximize();
            return "null";
        })
        ->bindFunction("BIND_fullscreen", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->fullscreen();
            return "null";
        })
        ->bindFunction("BIND_hide", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->hide();
            return "null";
        })
        ->bindFunction("BIND_show", [w](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            w->show();
            return "null";
        })
  // SYSTEM
        ->bindFunction("BIND_getPID", [](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            return std::to_string(boost::this_process::get_id());
        })
        ->bindFunction("BIND_getOS", [](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
#if defined(_WIN32)
            return strToJsonStr("Windows");
#elif defined(__APPLE__)
            return strToJsonStr("Apple");
#elif defined(__linux__)
            return strToJsonStr("Linux");
#endif
        })
        ->bindFunction("BIND_getApplicationDirPath", [](const std::string& req) -> std::string {
            // ()
            boost::ignore_unused(req);
            return json{strToUint8arrVec(RenWeb::Info::File::dir)}[0].dump();
        })
        ->bindFunction("BIND_sendNotif", [w](const std::string& req) -> std::string {
            // ()
            json params = json::parse(req);
            if (!params[2].is_null()) {
                w->sendNotif(jsonUint8arrToString(params[0]), jsonUint8arrToString(params[1]), formatPath(jsonUint8arrToString(params[2])));
            } else if (!params[1].is_null()) {
                w->sendNotif(jsonUint8arrToString(params[0]), jsonUint8arrToString(params[1]));
            } else if (!params[0].is_null()) {
                w->sendNotif(jsonUint8arrToString(params[0]));
            } else {
                spdlog::error("No notification body given.");
            }
            return "null";
        })
        ->bindFunction("BIND_openURI", [w](const std::string& req) -> std::string {
            // ()
            json params = json::parse(req);
            std::string resource = jsonUint8arrToString(params[0]);
            if (RenWeb::WebServer::isURI(resource)) {
                for (int i = 0; i < resource.length(); i++) {
                    if (resource[i] == '\\') resource[i] = '/';
                }
                w->openURI(resource);
            } else {
                w->openURI(formatPath(resource));
            }
            return "null";
        });
    return this;
}


