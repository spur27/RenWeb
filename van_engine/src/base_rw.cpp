#include "base_rw.hpp" 

Vanitas::BaseRW::BaseRW() {
    this->verify();
    spdlog::trace("BaseRW object constructed.");
}

Vanitas::BaseRW::~BaseRW() {
    spdlog::trace("BaseRW object deconstructed.");
}

void Vanitas::BaseRW::verify() {
    if (Vanitas::BaseRW::APPLICATION_DIR_PATH.empty()) { // if uninitialized
        char* home_dir_v = []() -> char* {
            if (getuid() == 0) {
                spdlog::warn("Program is being ran from root");
                return getpwuid(atoi(getenv("SUDO_UID")))->pw_dir;
            }
            return getenv("HOME");
        }();
        // TODO: Check getuid() and quit if program is run as root
        if (home_dir_v == nullptr) {
            const char* ERROR_MSG = "$HOME env variable couldn't be found!";
            spdlog::critical(ERROR_MSG);
            throw std::runtime_error(ERROR_MSG);
        } else {
            std::string dir_path_v = home_dir_v;
            if (!std::filesystem::exists(dir_path_v)) {
                spdlog::warn("Home directory path at:");
                spdlog::warn(dir_path_v);
                spdlog::warn("Doesn't exist! Creating one...");
                std::filesystem::create_directory(dir_path_v);
            }
            dir_path_v = dir_path_v.append("/").append(DIR_NAME);
            if (!std::filesystem::exists(dir_path_v)) {
                spdlog::warn("Application directory path at:");
                spdlog::warn(dir_path_v);
                spdlog::warn("Doesn't exist! Creating one...");
                std::filesystem::create_directory(dir_path_v);
            }
            spdlog::debug("Setting application directory to:");
            spdlog::debug(dir_path_v);
            Vanitas::BaseRW::APPLICATION_DIR_PATH.assign(dir_path_v);
        }
    } // else {
    //     spdlog::debug("Ran BaseRW verify function without doing anything.");
    // }
}

/*static*/ std::string Vanitas::BaseRW::getApplicationDirPath() {
    return Vanitas::BaseRW::APPLICATION_DIR_PATH;
}