#ifndef VAN_APP_H
#define VAN_APP_H
#include <boost/process.hpp>
#include "webview.hpp"
#include <spdlog/spdlog.h>
#include <map>

namespace Vanitas {
    class App {
        private:
            App(std::string);
            ~App();
            Vanitas::Webview* main_window;
            std::map<std::string, boost::process::child*> child_processes;
            inline static App* instance = nullptr;
        public:
            void __addChild__(std::string, boost::process::child*);
            void __removeChild__(std::string);
            // static App* __getInstance__();
            static void run(std::string);
            static pid_t getChildPID(std::string);
            static void addChild(std::string, boost::process::child*);
            static void removeChild(std::string);
    };
};
#endif