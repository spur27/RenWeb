#include "../include/app.hpp"
#include <boost/process/detail/child_decl.hpp>

 
Vanitas::App::App(std::string html_doc_name_v) {
    this->main_window = new Vanitas::Webview(html_doc_name_v);
    spdlog::trace("App object constructed.");
}

Vanitas::App::~App() {
    delete this->main_window;
    spdlog::trace("App object deconstructed.");
}

void Vanitas::App::__addChild__(std::string html_doc_name_v, boost::process::child* child_v) {
    if (Vanitas::App::getChildPID(html_doc_name_v) < 0) {
        this->child_processes[html_doc_name_v] = child_v;
    } 
}

void Vanitas::App::__removeChild__(std::string html_doc_name_v) {
    if (this->child_processes.find(html_doc_name_v) != this->child_processes.end()) {
        auto child_v = this->child_processes[html_doc_name_v];
        child_v->terminate();
        if (child_v->joinable()) {
            spdlog::debug("Waiting for child of PID '" + std::to_string(child_v->id()) + "' to terminate...");
            child_v->join();
        }
        spdlog::debug("Deleting child process of PID '" + std::to_string(child_v->id()) + "'");
        delete child_v;
        this->child_processes[html_doc_name_v] = nullptr;
    }
}

/*static*/ void Vanitas::App::run(std::string html_doc_name_v) {
    if (Vanitas::App::instance == nullptr) {
        Vanitas::App::instance = new Vanitas::App(html_doc_name_v);
    }
    spdlog::info("Running app~");
    Vanitas::App::instance->main_window->start();
    spdlog::info("Stopping app~");
    for (auto& [key, val] : Vanitas::App::instance->child_processes) {
        Vanitas::App::removeChild(key);
    }
    Vanitas::App::instance->child_processes.clear();
    delete Vanitas::App::instance;
}

/*static*/ pid_t Vanitas::App::getChildPID(std::string html_doc_name_v) {
    if (Vanitas::App::instance->child_processes.find(html_doc_name_v) != Vanitas::App::instance->child_processes.end()) {
        if (Vanitas::App::instance->child_processes[html_doc_name_v] == nullptr) {
            spdlog::debug("Child process found for HTML doc '" + html_doc_name_v + "' is dead. Erasing...");
            Vanitas::App::instance->child_processes.erase(Vanitas::App::instance->child_processes.find(html_doc_name_v));
        } else if (!Vanitas::App::instance->child_processes[html_doc_name_v]->running()) {
            spdlog::debug("Child process found for HTML doc '" + html_doc_name_v + "' is dead. Erasing...");
            delete Vanitas::App::instance->child_processes[html_doc_name_v];
            Vanitas::App::instance->child_processes.erase(Vanitas::App::instance->child_processes.find(html_doc_name_v));
        } else {
            spdlog::debug("Child process found for HTML doc '" + html_doc_name_v + "' at '" + std::to_string(Vanitas::App::instance->child_processes[html_doc_name_v]->id()) + "'");
            return Vanitas::App::instance->child_processes[html_doc_name_v]->id();
        }
    }
    spdlog::debug("No child process found for HTML doc '" + html_doc_name_v + "'");
    return -1;
}

/*static*/ void Vanitas::App::addChild(std::string html_doc_name_v, boost::process::child* child_v) {
    Vanitas::App::instance->__addChild__(html_doc_name_v, child_v);
}

/*static*/ void Vanitas::App::removeChild(std::string html_doc_name_v) {
    Vanitas::App::instance->__removeChild__(html_doc_name_v);
}

