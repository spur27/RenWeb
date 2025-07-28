#ifndef RENWEB_LOGGER_ADDONS_H
#define RENWEB_LOGGER_ADDONS_H
#include "info.hpp"
#include <spdlog/common.h>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <termcolor/termcolor.hpp>
#include <sstream>


namespace spdlog {
    void refresh();
    void clear();
    boost::filesystem::path getLogFilePath();
    extern spdlog::level::level_enum actual_level;
};
#endif