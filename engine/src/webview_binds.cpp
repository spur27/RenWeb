#include "../include/webview_binds.hpp"
#include "app.hpp"
#include "base_rw.hpp"
#include "config.hpp"
#include <boost/process/system.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <system_error>

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



// ------------------------------------------------------------
// ------------------------------------------------------------

void Vanitas::Bindings::Log::bind_logTrace(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logTrace";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        spdlog::trace(jsonLogBindingHandler(req));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logDebug(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logDebug";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        spdlog::debug(jsonLogBindingHandler(req));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logInfo(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logInfo";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        spdlog::info(jsonLogBindingHandler(req));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logWarn(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logWarn";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        spdlog::warn(jsonLogBindingHandler(req));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logError(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logError";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        spdlog::error(jsonLogBindingHandler(req));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logCritical(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logCritical";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        spdlog::critical(jsonLogBindingHandler(req));
        return "null";
    });
}

// ------------------------------------------------------------
// ------------------------------------------------------------

// BROKEN! FILES WITH CERTAIN CHARACTERS (\n) WONT BE PROCESSED
void Vanitas::Bindings::Filesystem::bind_readFile(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_readFile";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        if (param0.is_null()) {
            return "null";
        }
        std::string param0_str = jsonTostr(param0);
        if (!std::filesystem::exists(param0_str)) {
            spdlog::error(std::string("No file exists at path: ") + param0_str);
            return "null";
        }
        spdlog::debug(std::string("trying to read file of path: ") + param0_str);
        std::ifstream file;
        std::ostringstream str;
        file.open(param0_str);
        str << file.rdbuf();
        file.close();
        spdlog::debug(jsonStr(str.str()));
        return jsonStr(str.str());
    });
}
// void Vanitas::Bindings::Filesystem::bind_writeFile(Vanitas::Webview* w) { }
void Vanitas::Bindings::Filesystem::bind_exists(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_exists";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        std::string param0_str = jsonTostr(param0);
        if (std::filesystem::exists(param0_str)) {
            return "true";
        } else {
            return "false";
        }
    });
}
void Vanitas::Bindings::Filesystem::bind_mkDir(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_mkDir";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        std::string param0_str = jsonTostr(param0);
        std::error_code ec;
        bool result = std::filesystem::create_directory(param0_str, ec);
        if (result) {
            return "true";
        } else {
            spdlog::error(ec.message());
            return "false";
        }
    });
}
void Vanitas::Bindings::Filesystem::bind_rmDir(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_rmDir";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        std::string param0_str = jsonTostr(param0);
        std::error_code ec;
        bool result = std::filesystem::remove(param0_str, ec);
        if (result) {
            return "true";
        } else {
            spdlog::error(ec.message());
            return "false";
        }
    });
}
void Vanitas::Bindings::Filesystem::bind_ls(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_ls";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        std::string param0_str = jsonTostr(param0);
        if (!std::filesystem::is_directory(param0_str)) {
            spdlog::error("Entered path either isn't a directory or doesn't exist:");
            spdlog::error(param0_str);
            return "null";
        }
        std::error_code ec;
        json array = json::array();
        int index = 0;
        for (const auto& entry : std::filesystem::directory_iterator(param0_str)) {
            array[index] = entry.path();
            index++;
        }
        return array.dump();
    });
}
// void bind_rename(Vanitas::Webview*);
// void bind_move(Vanitas::Webview*);
// void bind_copyFile(Vanitas::Webview*);


// ------------------------------------------------------------
// ------------------------------------------------------------

void Vanitas::Bindings::Webview::bind_saveSettings(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_saveSettings";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        (void)req;
        w->info->save();
        return "null";
    });
}
// void bind_setSettings(Vanitas::Webview* w);
void Vanitas::Bindings::Webview::bind_readSettings(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_readSettings";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        (void)req;
        return Vanitas::Config::getSettings(w->info->html_doc_name).dump(2);
    });
}
void Vanitas::Bindings::Webview::bind_setHTML(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_setHTML";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        if (param0.is_string()) {
            std::string param0_str = param0.dump();
            w->setHTML(param0.dump().substr(1, param0_str.length()-2));
        } else {
            w->setTitle(param0.dump());
        }
        return "null";
    }); 
}
void Vanitas::Bindings::Webview::bind_setHTMLToDoc(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_setHTMLToDoc";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        json param1 = params[1];
        std::string param0_str = param0.dump();
        if (param0.is_string()) {
            param0_str = param0_str.substr(1, param0_str.length()-2);
        }
        w->setHTMLToFile(param0_str);
        if (param1.is_boolean() && (param1.dump() == "true")) {
            w->info->html_doc_name = param0_str;
            w->info->refresh();
            w->refreshTitle();
            // attempt to set size
            w->refreshSize();
        }
        return "null";
    }); 
}
void Vanitas::Bindings::Webview::bind_setSize(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_setSize";
    w->bindFunction(fn_name, [/*w*/](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        json param1 = params[1];
        json param2 = params[2];
        // if (param0.is_string()) {
        //     std::string param0_str = param0.dump();
        //     w->setTitle(param0.dump().substr(1, param0_str.length()-2));
        // } else {
        //     w->setTitle(param0.dump());
        // }
        return "null";
    }); 
}
void Vanitas::Bindings::Webview::bind_setTitle(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_setTitle";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        if (param0.is_string()) {
            std::string param0_str = param0.dump();
            w->setTitle(param0.dump().substr(1, param0_str.length()-2));
        } else {
            w->setTitle(param0.dump());
        }
        return "null";
    }); 
}
void Vanitas::Bindings::Webview::bind_terminate(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_terminate";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        (void)req;
        w->terminateWindow();
        return "null";
    }); 
}
void Vanitas::Bindings::Webview::bind_openWindow(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_openWindow";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        json param0 = params[0];
        spdlog::info(std::filesystem::current_path().string());
        boost::process::child* c;
        if (param0.is_null()) {
            spdlog::debug("Attempting to start duplicate process...");
            c = new boost::process::child("/home/spur/Programming/Frameworks/Vanitas/van_engine/build/debug/van_engine");
            Vanitas::App::addChild(std::string(MAIN_HTML_DOC_NAME) + " " + std::to_string(c->id()), c);
        } else {
            std::string param0_str = param0.dump();
            if (param0.is_string()) {
                param0_str = param0_str.substr(1, param0_str.length()-2);
            }
            spdlog::debug("Attempting to start process for HTML doc '" + param0_str + "'");
            c = new boost::process::child("/home/spur/Programming/Frameworks/Vanitas/van_engine/build/debug/van_engine", param0_str);
            Vanitas::App::addChild(param0_str, c);
        }
        return "null";
    }); 
}


// ------------------------------------------------------------
// ------------------------------------------------------------

void Vanitas::Bindings::Util::bind_getPID(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_getPID";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        (void)req;
        return std::to_string(getpid());
    });
}

void Vanitas::Bindings::Util::bind_getApplicationDirPath(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_getApplicationDirPath";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        (void)req;
        return jsonStr(Vanitas::BaseRW::getApplicationDirPath());
    });
}

void Vanitas::Bindings::Util::bind_getHTMLDocName(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_getHTMLDocName";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        (void)req;
        return jsonStr(w->info->html_doc_name);
    });
}