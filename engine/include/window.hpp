#ifndef RENWEB_WINDOW_H
#define RENWEB_WINDOW_H
#include "info.hpp"
#include "process_manager.hpp"
#include "logger_addons.hpp"
#include "web_server.hpp"
#include "process_manager.hpp"
#include <spdlog/spdlog.h>
#include <string>
#include <spdlog/spdlog.h>
#include <boost/process.hpp>
#include <webview/webview.h>
#include <map>
#include <algorithm>
#include <string>
#include <cstdlib>  
#include <portable-file-dialogs.h>
#if defined(_WIN32)
#include <windows.h>
#include <commdlg.h> 
using WindowHandle = HWND;
#elif defined(__APPLE__)
#include <Cocoa/Cocoa.h>
using WindowHandle = NSWindow*;
#elif defined(__linux__)
#include <gtk/gtk.h>
#include <libnotify/notify.h>
using WindowHandle = GtkWidget*;  // GtkWindow*
#endif


namespace RenWeb {
    class Window : private webview::webview {
        private:
            std::map<std::string, sync_binding_t> bind_cache;
            RenWeb::Window* processContents();
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

            std::vector<std::string> openChooseFilesDialog(bool=false, bool=false, std::vector<std::string> filteration=std::vector<std::string>(), std::string=RenWeb::Info::File::dir);
            RenWeb::Window* sendNotif(std::string, std::string="Summary", std::string=std::filesystem::path(RenWeb::Info::File::dir).append("resource").append("app.ico").string());
            RenWeb::Window* openURI(std::string);

            void start();
    };
};
#endif