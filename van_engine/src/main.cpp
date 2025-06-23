#include "../include/app_info.hpp"
#include "../include/app.hpp"
#include "../include/cache.hpp"
#include "../include/config.hpp"
#include "../include/custom.hpp"
#include "../include/html.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <webview/backends.hh>
#include <webview/types.h>

int main(int argc, char** argv) {
#ifdef DEBUG 
spdlog::set_level(spdlog::level::trace);
#endif
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%t] [%^%l%$] %v");
    spdlog::info("Starting program...");
    []() -> void {
        Vanitas::Cache::getInstance();
        Vanitas::Config::getInstance();
        Vanitas::Custom::getInstance();
        Vanitas::HTML::getInstance();
    }();
    Vanitas::App::run((argc > 1) ? std::string(argv[1]) : std::string(MAIN_HTML_DOC_NAME));
    []() -> void {
        Vanitas::Cache::deleteInstance();
        Vanitas::Config::deleteInstance();
        Vanitas::Custom::deleteInstance();
        Vanitas::HTML::deleteInstance();
    }();
    spdlog::info("Exiting program...");
    return 0;
}
