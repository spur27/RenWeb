#include "../include/html.hpp"
        
Vanitas::HTML::HTML() : Vanitas::BaseRW() {
    this->verify();
    spdlog::trace("HTML object constructed.");
};

Vanitas::HTML::~HTML() {
    spdlog::trace("HTML object deconstructed.");
};

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

void Vanitas::HTML::verify() {
    if (Vanitas::BaseRW::APPLICATION_DIR_PATH.empty()) {
        const char* ERROR_MSG = "APPLICATION_DIR_PATH is uninintialized before HTML paths initialization.";
        spdlog::critical(ERROR_MSG);
        throw std::runtime_error(ERROR_MSG);
    }
    this->path.assign(Vanitas::BaseRW::APPLICATION_DIR_PATH.string() + "/html");
    if (!std::filesystem::exists(this->path)) {
        spdlog::warn("HTML directory path at:");
        spdlog::warn(this->path.string());
        spdlog::warn("Doesn't exist! Creating one...");
        std::filesystem::create_directory(this->path);
    }
};

std::string Vanitas::HTML::__getHTMLFileContents__(std::string html_doc_name) {
    std::filesystem::path html_doc_path = std::filesystem::path(this->path.string() + "/" + html_doc_name + ".html");
    if (!std::filesystem::exists(html_doc_path)) {
        spdlog::error("Could not find HTML file of doc name " + html_doc_name + " at:");
        spdlog::error(html_doc_path.string());
        return std::string("<h1>404 not found!</h1>") + "<p>Could not find file at</p>" + "<a href=\"file://" + html_doc_path.string() + "\">" + html_doc_path.string() + "</a>";
    }
    std::ifstream file;
    std::ostringstream str;
    file.open(html_doc_path.c_str());
    str << file.rdbuf();
    file.close();
    return str.str();
}

// ----------------------------------------
// ----------------------------------------
// ----------------------------------------

/*static*/ Vanitas::HTML* Vanitas::HTML::getInstance() {
    if (Vanitas::HTML::instance == nullptr) {
        Vanitas::HTML::instance = new Vanitas::HTML();
    }
    return Vanitas::HTML::instance;
}

/*static*/ void Vanitas::HTML::deleteInstance() {
    if (Vanitas::HTML::instance != nullptr) {
        delete Vanitas::HTML::instance;
        Vanitas::HTML::instance = nullptr;
    }
}

/*static*/ std::string Vanitas::HTML::getHTMLFileContents(std::string html_doc_name) {
    return Vanitas::HTML::getInstance()->__getHTMLFileContents__(html_doc_name);
}



