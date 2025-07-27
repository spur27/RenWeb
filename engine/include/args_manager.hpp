#ifndef RENWEB_ARGS_MANAGER_H
#define RENWEB_ARGS_MANAGER_H
// #include "webview.hpp"
#include "info.hpp"
#include "logger_addons.hpp"
#include "window.hpp"
// #include <map>
// #include <algorithm>
#include <string>
#include <spdlog/spdlog.h>
#include <vector>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/process.hpp>
#include <stdexcept>


namespace RenWeb {
    class ArgsManager {
        private:
            ArgsManager();
            ~ArgsManager();
            inline static boost::program_options::options_description desc 
                = boost::program_options::options_description("Available Options");
            static void help();
            static void version();
            static void clear();
            inline static std::vector<std::string> pages_vec;
        public:
            static void parse(int, char**);
    };
};
#endif