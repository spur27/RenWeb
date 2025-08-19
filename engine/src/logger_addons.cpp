#include "../include/logger_addons.hpp"

spdlog::level::level_enum spdlog::actual_level = spdlog::level::info;

void spdlog::refresh() {
    const std::string date_str = "%Y-%m-%d %H:%M:%S.%e";
    const std::string thread_str = "%t";
    const std::string log_type_str = "%^%l%$";
    const std::string msg_str = "%v";
    std::stringstream colored_log_str, boring_log_str;
    colored_log_str << termcolor::colorize << "[" << termcolor::bold << termcolor::dark << termcolor::red << RenWeb::Info::App::name << termcolor::reset << "] ";
    if (!RenWeb::Info::App::page.empty()) {
        colored_log_str << "[" << termcolor::magenta << RenWeb::Info::App::page << termcolor::reset << "] ";
    }
    colored_log_str << termcolor::blue << date_str << termcolor::reset
                << " [" << termcolor::italic << thread_str << termcolor::reset << "] "
                << "[" << log_type_str << "] "
                << msg_str;
    boring_log_str << "[" << RenWeb::Info::App::name << "] ";
    if (!RenWeb::Info::App::page.empty()) {
        boring_log_str << "[" << RenWeb::Info::App::page << "] ";
    }
    boring_log_str << date_str
                << " [" << thread_str << "] "
                << "[" << log_type_str << "] "
                << msg_str;
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::actual_level);
    console_sink->set_pattern(colored_log_str.str());

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::filesystem::path(RenWeb::Info::File::dir + "/log.txt").string(), false);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern(boring_log_str.str());
    std::vector<spdlog::sink_ptr> log_sinks {console_sink, file_sink};
    auto default_logger = std::make_shared<spdlog::logger>("default", begin(log_sinks), end(log_sinks));

    // spdlog::register_logger(default_logger);
    spdlog::set_default_logger(default_logger);
    spdlog::set_level(spdlog::actual_level);
    //spdlog::flush_on(spdlog::level::warn);

    spdlog::trace("Logger has refreshed");
}

void spdlog::clear() {
    const std::filesystem::path log_file_path = std::filesystem::path(RenWeb::Info::File::dir) / "log.txt";
    if (std::filesystem::exists(log_file_path)) {
        std::filesystem::resize_file(std::filesystem::path(RenWeb::Info::File::dir) / "log.txt", 0);
    }
}

std::filesystem::path spdlog::getLogFilePath() {
    return std::filesystem::path(RenWeb::Info::File::dir) / "log.txt";
}