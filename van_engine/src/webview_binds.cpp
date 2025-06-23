#include "../include/webview_binds.hpp"
#include "app.hpp"
#include "base_rw.hpp"
#include "config.hpp"
#include <boost/process/system.hpp>
#include <filesystem>

std::string s(std::string str) {
    return std::string('"' + str + '"');
}

// ------------------------------------------------------------
// ------------------------------------------------------------

void Vanitas::Bindings::Log::bind_logTrace(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logTrace";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        spdlog::trace(params[0].dump(0));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logDebug(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logDebug";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        spdlog::debug(params[0].dump(0));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logInfo(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logInfo";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        spdlog::info(params[0].dump(0));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logWarn(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logWarn";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        spdlog::warn(params[0].dump(0));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logError(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logError";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        spdlog::error(params[0].dump(0));
        return "null";
    });
}
void Vanitas::Bindings::Log::bind_logCritical(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_logCritical";
    w->bindFunction(fn_name, [](const std::string& req) -> std::string {
        json params = json::parse(req);
        spdlog::critical(params[0].dump(0));
        return "null";
    });
}

// ------------------------------------------------------------
// ------------------------------------------------------------

// void bind_saveSettings(Vanitas::Webview* w);
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
// void bind_setSize(Vanitas::Webview* w);
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
        return s(Vanitas::BaseRW::getApplicationDirPath());
    });
}

void Vanitas::Bindings::Util::bind_getHTMLDocName(Vanitas::Webview* w) {
    const std::string fn_name = "BIND_getHTMLDocName";
    w->bindFunction(fn_name, [w](const std::string& req) -> std::string {
        (void)req;
        return s(w->info->html_doc_name);
    });
}