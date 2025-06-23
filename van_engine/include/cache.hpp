#ifndef VAN_CACHE_H
#define VAN_CACHE_H
#include "base_rw.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>


namespace Vanitas {
    class Cache : public BaseRW {
        private:
            Cache();
            ~Cache();
            inline static Cache* instance = nullptr;
            void verify() override;
        public:
            static Cache* getInstance();
            static void deleteInstance();
    };
};
#endif