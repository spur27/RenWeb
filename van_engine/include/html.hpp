#ifndef VAN_HTML_H
#define VAN_HTML_H
#include "base_rw.hpp"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>

namespace Vanitas {
    class HTML: public BaseRW {
        private:
            HTML();
            ~HTML();
            inline static HTML* instance = nullptr;
            void verify() override;
            std::string __getHTMLFileContents__(std::string);
        public:
            static HTML* getInstance();
            static void deleteInstance();
            static std::string getHTMLFileContents(std::string);
    };
};
#endif