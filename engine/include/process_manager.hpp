#ifndef RENWEB_PROCESS_MANAGER_H
#define RENWEB_PROCESS_MANAGER_H
#include "info.hpp"
// #include "webview.hpp"
#include <memory>
#include <map>
#include <memory>
#include <spdlog/spdlog.h>
#include <boost/process.hpp>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#if defined(_WIN32)
#include <windows.h>
#include <tlhelp32.h>
#elif defined(__APPLE__)
#include <Cocoa/Cocoa.h>
#elif defined(__linux__)
#include <gtk/gtk.h>
#endif



namespace RenWeb {
    class ProcessManager {
        private:
            std::map<std::string, std::vector<boost::process::child>> sub_processes;
        public:
            ProcessManager();
            ~ProcessManager();
            void startProcess(std::string);
            int cleanProcesses();
            void printProcesses();
            void killProcesses();
            bool hasProcess(std::string);
            void bringToForeground(std::string);
    };
};
#endif