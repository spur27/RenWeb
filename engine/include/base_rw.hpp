#ifndef VAN_BASE_RW_H
#define VAN_BASE_RW_H
#include <filesystem>
#include <spdlog/spdlog.h>
#include <pwd.h>
#include "app_info.hpp"

namespace Vanitas {
    class BaseRW {
        private:
        protected:
            BaseRW();
            virtual ~BaseRW();
            virtual void verify();
            inline static std::filesystem::path APPLICATION_DIR_PATH = std::filesystem::path("");
            std::filesystem::path path;
        public:
            static std::string getApplicationDirPath();
    };
};
#endif