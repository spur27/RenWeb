#ifndef RENWEB_HTML_H
#define RENWEB_HTML_H
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#include <string>
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <exception>
#include <istream>
#include <stdexcept>
#include <webview/types.h>

// DONT RENAME OR REMOVE DEFINES!!
// You can still change the define values and add new defines
#define RENWEB_INFO_DEFAULT_PAGE "test"
#define RENWEB_INFO_DEFAULT_NAME "RenWeb" 
#define RENWEB_INFO_DEFAULT_VERSION "0.0.1"
#define RENWEB_INFO_DEFAULT_DESCRIPTION "I am an app that does things."
#define RENWEB_INFO_DEFAULT_LICENSE "LISC"
#define RENWEB_INFO_DEFAULT_AUTHOR "@YOU"
#define RENWEB_INFO_DEFAULT_HINT WEBVIEW_HINT_NONE
#define RENWEB_INFO_DEFAULT_WIDTH 720
#define RENWEB_INFO_DEFAULT_HEIGHT 480
#define RENWEB_INFO_DEFAULT_SAVE_RESIZE_DIMENSIONS true
#define RENWEB_INFO_DEFAULT_DECORATED true
#define RENWEB_INFO_DEFAULT_RESIZABLE true
#define RENWEB_INFO_DEFAULT_KEEP_ABOVE false


using json = nlohmann::json;


namespace RenWeb {
    namespace Info {
        class File {
            private: 
            public:
                static inline std::string name 
                    = boost::dll::program_location().filename().string();
                static inline std::string dir 
                    = boost::dll::program_location().parent_path().string();
                static inline std::string path 
                    = boost::dll::program_location().string();
                static void refresh();
        };
        class App {
            private:
            public:
                inline static std::string page = RENWEB_INFO_DEFAULT_PAGE;
                inline static std::string name = RENWEB_INFO_DEFAULT_NAME;
                inline static std::string version = RENWEB_INFO_DEFAULT_VERSION;
                inline static std::string description = RENWEB_INFO_DEFAULT_DESCRIPTION;
                inline static std::string license = RENWEB_INFO_DEFAULT_LICENSE;
                inline static std::string author = RENWEB_INFO_DEFAULT_AUTHOR;
                inline static webview_hint_t hint = RENWEB_INFO_DEFAULT_HINT;
                inline static unsigned int width = RENWEB_INFO_DEFAULT_WIDTH;
                inline static unsigned int height = RENWEB_INFO_DEFAULT_HEIGHT;
                inline static bool save_resize_dimensions = RENWEB_INFO_DEFAULT_SAVE_RESIZE_DIMENSIONS;
                inline static bool decorated = RENWEB_INFO_DEFAULT_DECORATED;
                inline static bool resizable = RENWEB_INFO_DEFAULT_RESIZABLE;
                inline static bool keep_above = RENWEB_INFO_DEFAULT_KEEP_ABOVE;
                static void refresh(json&&);
                static void loadFromConfigFile();
                static void resetPageToDefault();
                static void resetToDefaults();
                static void save();
                static json get();
                static void set(json&&); 
        };
    };
};
#endif