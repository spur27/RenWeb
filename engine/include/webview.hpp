#ifndef VAN_WEBVIEW_H
#define VAN_WEBVIEW_H
#include "app_info.hpp"
#include "html.hpp"
#include <spdlog/spdlog.h>
#include "webview_info.hpp"
#include <webview/webview.h>
#include <map>
#include <algorithm>


#ifndef DEBUG
#define DEBUG_V true
#else
#define DEBUG_V false 
#endif

namespace Vanitas {
    class Webview : private webview::webview {
        private:
            std::map<std::string, sync_binding_t*> bind_cache;
        public:
            Webview(std::string);
            ~Webview();
            Vanitas::WebviewInfo* info;
            Vanitas::Webview* refreshSize();
            Vanitas::Webview* setSize(int, int, webview_hint_t);
            Vanitas::Webview* refreshTitle();
            Vanitas::Webview* setTitle(std::string);
            Vanitas::Webview* setHTML();
            Vanitas::Webview* setHTML(std::string);
            Vanitas::Webview* setHTMLToFile(std::string);
            Vanitas::Webview* bindFunction(std::string, sync_binding_t);
            Vanitas::Webview* unbindFunction(std::string);
            Vanitas::Webview* bindAll();
            Vanitas::Webview* unbindAll();
            Vanitas::Webview* runWindow();
            Vanitas::Webview* terminateWindow();
            webview_hint_t getHintType(std::string);
            void start();
    };
};
#endif