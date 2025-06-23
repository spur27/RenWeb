#include "../include/config.hpp"
        
Vanitas::Config::Config() : Vanitas::BaseRW() {
    this->verify();
    spdlog::trace("Config object constructed.");
};

Vanitas::Config::~Config() {
    spdlog::trace("Config object deconstructed.");
};

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

void Vanitas::Config::verify() {
    if (Vanitas::BaseRW::APPLICATION_DIR_PATH.empty()) {
        const char* ERROR_MSG = "APPLICATION_DIR_PATH is uninintialized before Config path initialization.";
        spdlog::critical(ERROR_MSG);
        throw std::runtime_error(ERROR_MSG);
    }
    this->path.assign(Vanitas::BaseRW::APPLICATION_DIR_PATH.string() + "/config.json");
    if (!std::filesystem::exists(this->path)) {
        spdlog::warn("Config file path at:");
        spdlog::warn(this->path.string());
        spdlog::warn("Doesn't exist! Creating one...");
        std::ofstream file;
        file.open(this->path.c_str(), std::ofstream::out | std::ofstream::trunc);
        file << "{ }";
        file.close();
    }
    try {
        auto json_v = json::parse(this->readConfigFile());
    } catch (const std::exception& e) {
        spdlog::error("Tried to parse the config and this happened:");
        spdlog::error(e.what());
    }
};

json Vanitas::Config::__getSetting__(json json_v, std::string setting_name) {
    if (!json_v.contains(setting_name)) {
        const std::string ERROR_MSG = std::string("Setting '") + setting_name + std::string("' doesn't exist!");
        spdlog::error(ERROR_MSG);
        return "{}"_json;
    } 
    return json_v.at(setting_name);
}

std::string Vanitas::Config::readConfigFile() {
    std::ifstream file;
    std::ostringstream str;
    file.open(this->path.c_str());
    str << file.rdbuf();
    file.close();
    return str.str();
}


json Vanitas::Config::__getSettings__(std::string setting_name) {
    json json_v = json::parse(this->readConfigFile());
    return this->__getSetting__(json_v, setting_name);
}

void Vanitas::Config::__setSetting__(std::string setting_name, json json_v) {
    auto json_v_config = json::parse(this->readConfigFile());
    json_v_config[setting_name] = json_v;
    std::ofstream file;
    file.open(this->path.c_str(), std::ofstream::out | std::ofstream::trunc);
    file << json_v_config.dump(2);
    file.close();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

/*static*/ json Vanitas::Config::getSetting(json json_v, std::string setting_name) {
    return Vanitas::Config::getInstance()->__getSetting__(json_v, setting_name);
};

/*static*/ json Vanitas::Config::getSettings(std::string setting_name) {
    return Vanitas::Config::getInstance()->__getSettings__(setting_name);
};

/*static*/ void Vanitas::Config::setSetting(std::string setting_name, json json_v) {
    Vanitas::Config::getInstance()->__setSetting__(setting_name, json_v);
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

/*static*/ Vanitas::Config* Vanitas::Config::getInstance() {
    if (Vanitas::Config::instance == nullptr) {
        Vanitas::Config::instance = new Vanitas::Config();
    }
    return Vanitas::Config::instance;
}

/*static*/ void Vanitas::Config::deleteInstance() {
    if (Vanitas::Config::instance != nullptr) {
        delete Vanitas::Config::instance;
        Vanitas::Config::instance = nullptr;
    }
}