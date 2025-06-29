#ifndef VAN_WEBVIEW_INFO_H
#define VAN_WEBVIEW_INFO_H
#include <webview/webview.h>
#include <spdlog/spdlog.h>
#include <sstream>
#include "config.hpp"

namespace Vanitas {
    class WebviewInfo {
        private:
        public:
            std::string html_doc_name = "default";
            std::string window_title = "Default Title";
            std::string window_hint = "NONE";
            unsigned short window_width = 690;
            unsigned short window_height = 460;
            bool window_save_resize_dimensions = true;
            bool window_resizable = true;
            WebviewInfo(std::string);
            ~WebviewInfo();
            void refresh();
            void save();
    };

};
#endif