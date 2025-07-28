#include "../include/process_manager.hpp"
#include <exception>
#include <spdlog/spdlog.h>
#include <vector>

RenWeb::ProcessManager::ProcessManager() {
    // this->main_web_page.reset(new RenWeb::Webview(this->info->getAppName()));
    spdlog::trace("Process Manager object constructed.");
}

RenWeb::ProcessManager::~ProcessManager() {
    // Takedown logger (shouldn't have to do anything)
    // Setup cache, config, custom, html
    // delete this->main_window;
    this->killProcesses();
    spdlog::trace("Process Manager object deconstructed.");
}

void RenWeb::ProcessManager::startProcess(std::string web_page_name) {
    if (this->sub_processes.find(web_page_name) == this->sub_processes.cend()) {
        this->sub_processes.insert(std::make_pair(web_page_name, std::vector<boost::process::child>()));
    }
    this->sub_processes[web_page_name].push_back(boost::process::child(boost::dll::program_location().generic_string(), "-P", web_page_name, "-l", std::to_string(spdlog::get_level()), boost::process::std_out > stdout, boost::process::std_err > stderr, boost::process::std_in < stdin));
    //DEBUG
    this->printProcesses();
}

int RenWeb::ProcessManager::cleanProcesses() {
    int removed_process_count = 0;
    for (auto& [web_page_name, vector_v] : this->sub_processes) {
        this->sub_processes[web_page_name].erase(std::remove_if(vector_v.begin(), vector_v.end(), [&removed_process_count](boost::process::child& proc) -> bool {
            const bool process_status = !proc.running();
            if (process_status) {
                spdlog::info("Removing process...");
                removed_process_count++;
            }
            return process_status;
        }));
    }
    for (auto iter = this->sub_processes.begin(); iter != this->sub_processes.end();) {
        if (iter->second.empty()) {
            spdlog::info("Process vector for \"" + iter->first + "\" is empty. Deleting from map...");
            iter = this->sub_processes.erase(iter);
        } else {
            iter++;
        }
    }
    return removed_process_count;
}


void RenWeb::ProcessManager::printProcesses() {
    std::stringstream result("###Processes###\n");
    for (auto& [web_page_name, vector_v] : this->sub_processes) {
        if (vector_v.empty()) {
            result << "\t" << web_page_name + ": EMPTY\n";
            continue;
        }
        for (size_t i = 0; i < vector_v.size(); i++) {
            const std::string status = [](const bool& status_v) -> std::string {
                if (status_v) {
                    return "ACTIVE";
                } else {
                    return "INACTIVE";
                }
            }(vector_v[i].running());
            result << "\t\t[" << std::to_string(i) << "]: " << std::to_string(vector_v[i].id()) << " " << status << "\n";
        }
    }
    spdlog::info(result.str());
}

void RenWeb::ProcessManager::killProcesses() {
    for (auto& [webpage_name, vector_v] : this->sub_processes) {
        for (auto& proc : vector_v) {
            if (proc.running()) {
                spdlog::info("Killing " + std::to_string(proc.id()) + " of document \"" + webpage_name + "\"");
                proc.terminate();
                proc.join();
            }
        }
    }
    this->sub_processes.clear();
}

bool RenWeb::ProcessManager::hasProcess(std::string process_name) {
    if (this->sub_processes.find(process_name) == this->sub_processes.end()) return false;
    for (auto& [proc_name, proc_vec] : this->sub_processes) {
        for (auto& proc : proc_vec) {
            if (proc.running()) return true;
        }
    }
    return false;
}

void RenWeb::ProcessManager::bringToForeground(std::string process_name) {
    if (this->sub_processes.find(process_name) == this->sub_processes.end()) {
        spdlog::error("No process of name \"" + process_name + "\" exists! Can't bring to foreground.");
        return;
    }
    std::vector<boost::process::child>& proc_vec = this->sub_processes[process_name]; 
    boost::process::child* proc = nullptr;
    bool multiple_processes = false;
    for (auto& proc_v : proc_vec) {
        if (proc_v.running() && proc == nullptr) {
            proc = &proc_v;
        } else if (proc_v.running() && !multiple_processes) {
            multiple_processes = true;
        }
    }
    if (proc == nullptr) {
        spdlog::error("No process of name \"" + process_name + "\" exists! Can't bring to foreground.");
        return;
    } else if (multiple_processes) {
        spdlog::warn("Multiple processes of \"" + process_name + "\" are open. Changing first one found to foreground.");
    }
    pid_t child_pid = proc->id();
    try {
#if defined(_WIN32)
        // Windows: Enum all windows and bring one belonging to the PID to the front
        bool CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid == (DWORD)lParam && IsWindowVisible(hwnd)) {
                SetForegroundWindow(hwnd);
                return FALSE; // stop enumerating
            }
            return TRUE; // continue
        }
        EnumWindows(EnumWindowsCallback, child_pid);
#elif defined(__APPLE__)
        // macOS: Use AppleScript to bring process with PID to front
        NSString *script = [NSString stringWithFormat:
            @"tell application \"System Events\"\n"
            "    set frontmost of the first process whose unix id is %d to true\n"
            "end tell", child_pid];
        NSAppleScript* appleScript = [[NSAppleScript alloc] initWithSource:script];
        NSDictionary* errorInfo = nil;
        [appleScript executeAndReturnError:&errorInfo];
        [appleScript release];
#elif defined(__linux__)
    spdlog::critical("bringToForefround is UNIMPLEMENTED");
#endif
    } catch (const std::exception& e) {
        spdlog::error(e.what());
    }
}



// void RenWeb::App::run(std::string html_doc_name_v) {
//     spdlog::info("Running app~");
//     this->main_window->start();
//     spdlog::info("Stopping app~");
//     for (auto& [key, val] : this->child_processes) {
//         RenWeb::App::removeChild(key);
//     }
//     this->child_processes.clear();
//     delete this;
// }


