#ifndef VAN_CUSTOM_H
#define VAN_CUSTOM_H
#include "base_rw.hpp"
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace Vanitas {
    class Custom: public BaseRW {
        private:
            Custom();
            ~Custom();
            inline static Custom* instance = nullptr;
            std::filesystem::path path_backup;
            std::filesystem::path path_custom;
            void verify() override;
        public:
            static Custom* getInstance();
            static void deleteInstance();
    };
};
#endif