#include "../include/window_binds_helper.hpp"

namespace RWBH = RenWeb::BindingHelpers;

std::string RWBH::strToJsonStr(std::string str) {
    return std::string('"' + str + '"');
}

// Removes the surrounding quotes from a JSON dumped string
std::string RWBH::jsonStrToStr(std::string str) {
    return str.substr(1, str.length()-2);
} 

// Gets JSON contents as a usable std::string
std::string RWBH::jsonToStr(json json_v, int indent) {
    std::string json_str = json_v.dump(indent);
    if (json_v.is_string()) {
        json_str = RWBH::jsonStrToStr(json_str);
    }
    return json_str;
}

std::vector<char> RWBH::jsonUint8arrToVec(json json_v) {
    try {
        if (!json_v.is_object() && !json_v.is_array()) {
            throw std::runtime_error(std::string("Variable passed isn't a uint8array and can't be converted to one! A ") + json_v.type_name() + " was recieved.");
        }
        size_t n = json_v.size();
        std::vector<char> buffer(n);
        for (const auto& [key, value] : json_v.items()) {
            size_t index = static_cast<size_t>(std::stoul(key));
            if (index >= n) continue; // Safety check in case of malformed data
            int val = json_v.value(key, -1);
            if (val == -1) {
                throw std::runtime_error("Invalid value in binary string: " + RWBH::jsonToStr(value));
            }
            buffer[index] = static_cast<char>(val);
        }
        return buffer;
    } catch (const std::exception& e) {
        spdlog::error(e.what());
        return std::vector<char>();
    }
}

std::vector<unsigned int> RWBH::strToUint8arrVec(std::string str) {
    return std::vector<unsigned int>{str.begin(), str.end()};
}

std::string RWBH::jsonUint8arrToString(json json_v) {
    std::vector<char> vec = RWBH::jsonUint8arrToVec(json_v);
    if (vec.empty()) return "";
    else return std::string{vec.begin(), vec.end()};
}

std::string RWBH::formatPath(std::string path) {
    if (RenWeb::WebServer::isURI(path)) {
        return path;
    }
#if defined(_WIN32)
    // this->hide();
    for (int i = 0; i < path.length(); i++) {
        if (path[i] == '/') path[i] = '\\';
    }
    return path;
#else 
    for (int i = 0; i < path.length(); i++) {
        if (path[i] == '\\') path[i] = '/';
    }
    return path;
#endif
}