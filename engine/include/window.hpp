#ifndef RENWEB_WINDOW_H
#define RENWEB_WINDOW_H
#if defined(_WIN32)
#include <windows.h>
using WindowHandle = HWND;
#elif defined(__APPLE__)
#include <Cocoa/Cocoa.h>
using WindowHandle = NSWindow*;
#elif defined(__linux__)
#include <gtk/gtk.h>
using WindowHandle = GtkWidget*;  // GtkWindow*
#endif
#include "info.hpp"
#include "process_manager.hpp"
#include "logger_addons.hpp"
#include "web_server.hpp"
#include "process_manager.hpp"
#include <spdlog/spdlog.h>
#include <string>
#include <spdlog/spdlog.h>
#include <boost/process.hpp>
#include <boost/regex.hpp>
#include <webview/webview.h>
#include <map>
#include <algorithm>
#include <string>


namespace RenWeb {
    struct ChooseFileDialogSettings {
        std::vector<std::pair<std::string, std::vector<std::string>>> patterns = {};
        std::vector<std::pair<std::string, std::vector<std::string>>> mimes = {};
    };
    class Window : private webview::webview {
        private:
            std::map<std::string, sync_binding_t> bind_cache;
            bool isURI(std::string);
        public:
            RenWeb::ProcessManager process_manager;
            RenWeb::WebServer web_server;
            Window(unsigned short, unsigned short);
            Window(unsigned short, unsigned short, std::string);
            ~Window();

            RenWeb::Window* setSize(unsigned int, unsigned int, webview_hint_t);
            std::pair<unsigned int, unsigned int> getSize();
            RenWeb::Window* updateSize(bool=false);

            RenWeb::Window* bindFunction(std::string, sync_binding_t);
            RenWeb::Window* unbindFunction(std::string);
            RenWeb::Window* bindAll();
            RenWeb::Window* unbindAll();

            RenWeb::Window* reloadPage();
            RenWeb::Window* navigatePage(std::string);

            RenWeb::Window* refreshSettings();

            RenWeb::Window* runWindow();
            RenWeb::Window* terminateWindow();

            RenWeb::Window* setDecorated(bool);
            RenWeb::Window* setResizable(bool);
            RenWeb::Window* setKeepAbove(bool);

            RenWeb::Window* minimize();
            RenWeb::Window* maximize();
            RenWeb::Window* fullscreen();
            RenWeb::Window* hide();
            RenWeb::Window* show();

            std::vector<std::string> openChooseFilesDialog(bool=false, bool=false, RenWeb::ChooseFileDialogSettings* =nullptr);

            void start();
    };
};
#endif