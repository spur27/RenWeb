#ifndef RENWEB_ARGS_MANAGER_H
#define RENWEB_ARGS_MANAGER_H
// #include "webview.hpp"
#include "info.hpp"
#include "logger_addons.hpp"
#include "window.hpp"
// #include <map>
// #include <algorithm>
#include <map>
#include <string>
#include <spdlog/spdlog.h>
#include <vector>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/process.hpp>
#include <stdexcept>


namespace RenWeb {
    class App {
        private:
            boost::program_options::options_description desc 
                = boost::program_options::options_description("Available Options");
            std::vector<std::string> pages;
            void addArgs();
        public:
            App();
            ~App();
            void run(int, char**);
    };
};
#endif