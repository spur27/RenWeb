#ifndef VAN_CONFIG_H
#define VAN_CONFIG_H
#include "base_rw.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Vanitas {
    class Config: public BaseRW {
        private:
            Config();
            ~Config();
            inline static Config* instance = nullptr;
//        ^^std::string path;
            json __getSetting__(json, std::string);
            json __getSettings__(std::string);
            void __setSetting__(std::string, json);
            void verify() override;
            std::string readConfigFile();
        public:
            static json getSetting(json, std::string);
            static json getSettings(std::string);
            static void setSetting(std::string, json);
            static Config* getInstance();
            static void deleteInstance();
    };
};
#endif