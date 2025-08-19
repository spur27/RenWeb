#ifndef RENWEB_WINDOW_BINDS_HELPER_HPP
#define RENWEB_WINDOW_BINDS_HELPER_HPP
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

namespace RenWeb {
    namespace BindingHelpers {
        std::string strToJsonStr(std::string);
        std::string jsonStrToStr(std::string);
        std::string jsonToStr(json, int=-1);
        std::vector<char> jsonUint8arrToVec(json);
        std::vector<unsigned int> strToUint8arrVec(std::string);
        std::string jsonUint8arrToString(json);
    }
}

#endif