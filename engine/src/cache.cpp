#include "../include/cache.hpp"
        
Vanitas::Cache::Cache() : Vanitas::BaseRW() {
    this->verify();
    spdlog::trace("Cache object constructed.");
};

Vanitas::Cache::~Cache() {
    spdlog::trace("Cache object deconstructed.");
};

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

void Vanitas::Cache::verify() {
    if (Vanitas::BaseRW::APPLICATION_DIR_PATH.empty()) {
        const char* ERROR_MSG = "APPLICATION_DIR_PATH is uninintialized before Config path initialization.";
        spdlog::critical(ERROR_MSG);
        throw std::runtime_error(ERROR_MSG);
    }
    this->path.assign(Vanitas::BaseRW::APPLICATION_DIR_PATH.string() + "/cache");
    if (!std::filesystem::exists(this->path)) {
        spdlog::warn("Cache directory path at:");
        spdlog::warn(this->path.string());
        spdlog::warn("Doesn't exist! Creating one...");
        std::filesystem::create_directory(this->path);
    }
};

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

/*static*/ Vanitas::Cache* Vanitas::Cache::getInstance() {
    if (Vanitas::Cache::instance == nullptr) {
        Vanitas::Cache::instance = new Vanitas::Cache();
    }
    return Vanitas::Cache::instance;
}

/*static*/ void Vanitas::Cache::deleteInstance() {
    if (Vanitas::Cache::instance != nullptr) {
        delete Vanitas::Cache::instance;
        Vanitas::Cache::instance = nullptr;
    }
}
