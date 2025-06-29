#include "../include/custom.hpp"

Vanitas::Custom::Custom() : Vanitas::BaseRW() {
    this->verify();
    spdlog::trace("Custom object constructed.");
};

Vanitas::Custom::~Custom() {
    spdlog::trace("Custom object deconstructed.");
};

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

void Vanitas::Custom::verify() {
    if (Vanitas::BaseRW::APPLICATION_DIR_PATH.empty()) {
        const char* ERROR_MSG = "APPLICATION_DIR_PATH is uninintialized before Config path initialization.";
        spdlog::critical(ERROR_MSG);
        throw std::runtime_error(ERROR_MSG);
    }
    this->path.assign("");
    this->path_backup.assign(Vanitas::BaseRW::APPLICATION_DIR_PATH.string() + "/backup");
    this->path_custom.assign(Vanitas::BaseRW::APPLICATION_DIR_PATH.string() + "/custom");
    if (!std::filesystem::exists(this->path_backup)) {
        spdlog::warn("Backup directory path at:");
        spdlog::warn(this->path_backup.string());
        spdlog::warn("Doesn't exist! Creating one...");
        std::filesystem::create_directory(this->path_backup);
    }
    if (!std::filesystem::exists(this->path_custom)) {
        spdlog::warn("Custom directory path at:");
        spdlog::warn(this->path_custom.string());
        spdlog::warn("Doesn't exist! Creating one...");
        std::filesystem::create_directory(this->path_custom);
    }
};

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

/*static*/ Vanitas::Custom* Vanitas::Custom::getInstance() {
    if (Vanitas::Custom::instance == nullptr) {
        Vanitas::Custom::instance = new Vanitas::Custom();
    }
    return Vanitas::Custom::instance;
}

/*static*/ void Vanitas::Custom::deleteInstance() {
    if (Vanitas::Custom::instance != nullptr) {
        delete Vanitas::Custom::instance;
        Vanitas::Custom::instance = nullptr;
    }
}
