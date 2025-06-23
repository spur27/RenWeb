#include "../include/webview.hpp"
#include "../include/webview_binds.hpp"


// problem is compiler args
 
Vanitas::Webview::Webview(std::string html_doc_name_v)
  : webview::webview(DEBUG_V, nullptr) {
    this->info = new Vanitas::WebviewInfo(html_doc_name_v);
    this->info->refresh();
    spdlog::debug("Webview object constructed.");
}

Vanitas::Webview::~Webview() {
    this->info->save();
    delete this->info;
    this->info = nullptr;
    spdlog::debug("Webview object deconstructed.");
}

Vanitas::Webview* Vanitas::Webview::refreshSize() {
    this->set_size(this->info->window_width, this->info->window_height, this->getHintType(this->info->window_hint));
    spdlog::debug("Window dimensions set to " + std::to_string(this->info->window_width) + "x" + std::to_string(this->info->window_height) + " of hint '" + this->info->window_hint + "'");
    return this;
}

Vanitas::Webview* Vanitas::Webview::setSize(int width, int height, webview_hint_t hint) {
    this->set_size(width, height, hint);
    spdlog::debug("Window dimensions set to " + std::to_string(width) + "x" + std::to_string(height) + " of hint '" + std::to_string(hint) + "'");
    return this;
}

Vanitas::Webview* Vanitas::Webview::refreshTitle() {
    this->set_title(this->info->window_title);
    spdlog::debug("Window title set to '" + this->info->window_title + "'");
    return this;
}

Vanitas::Webview* Vanitas::Webview::setTitle(std::string title) {
    this->set_title(title.c_str());
    spdlog::debug("Window title set to '" + title + "'");
    return this;
}

Vanitas::Webview* Vanitas::Webview::setHTML() {
    return this->setHTML(Vanitas::HTML::getHTMLFileContents(this->info->html_doc_name));
}

Vanitas::Webview* Vanitas::Webview::setHTMLToFile(std::string html_doc_name_v) {
    return this->setHTML(Vanitas::HTML::getHTMLFileContents(html_doc_name_v));
}

Vanitas::Webview* Vanitas::Webview::setHTML(std::string html) {
    this->set_html(html.c_str());
    bool is_string_small = html.length() < 75;
    if (!is_string_small) {
        html = html.substr(0, 75);
    }
    html.erase(std::remove(html.begin(), html.end(), '\n'), html.cend());
    html.erase(std::remove(html.begin(), html.end(), '\t'), html.cend());
    spdlog::debug("Window HTML set to:");
    spdlog::debug(html + ((is_string_small) ? "" : "..."));
    return this;
}

Vanitas::Webview* Vanitas::Webview::bindFunction(std::string fn_name, sync_binding_t fn) {
    this->bind(fn_name, fn);
    this->bind_cache[fn_name] = &fn;
    spdlog::trace("Bound '" + fn_name + "'");
    return this;
}

Vanitas::Webview* Vanitas::Webview::unbindFunction(std::string fn_name) {
    if (this->bind_cache.find(fn_name) == this->bind_cache.end()) {
        spdlog::warn("Function of name '" + fn_name + "' isn't in the bind cache. Attempting to unbind anyways...");   
    } else {
        this->bind_cache.erase(this->bind_cache.find(fn_name));
    }
    try {
        this->unbind(fn_name);
        spdlog::trace("Unbound '" + fn_name + "'");
    } catch (const std::runtime_error& e) {
        spdlog::error(e.what());
    }
    return this;
}

Vanitas::Webview* Vanitas::Webview::bindAll() {

    Vanitas::Bindings::Log::bind_logTrace(this);
    Vanitas::Bindings::Log::bind_logDebug(this);
    Vanitas::Bindings::Log::bind_logInfo(this);
    Vanitas::Bindings::Log::bind_logWarn(this);
    Vanitas::Bindings::Log::bind_logError(this);
    Vanitas::Bindings::Log::bind_logCritical(this);

    Vanitas::Bindings::Webview::bind_readSettings(this);
    Vanitas::Bindings::Webview::bind_setHTML(this);
    Vanitas::Bindings::Webview::bind_setHTMLToDoc(this);
    Vanitas::Bindings::Webview::bind_setTitle(this);
    Vanitas::Bindings::Webview::bind_terminate(this);
    Vanitas::Bindings::Webview::bind_openWindow(this);

    Vanitas::Bindings::Util::bind_getPID(this);
    Vanitas::Bindings::Util::bind_getApplicationDirPath(this);
    Vanitas::Bindings::Util::bind_getHTMLDocName(this);
    
    return this;
}

Vanitas::Webview* Vanitas::Webview::unbindAll() {
    auto bind_cache_copy(this->bind_cache);
    for (auto& [key, val] : bind_cache_copy) {
        this->unbindFunction(key);
    }
    return this;
}

Vanitas::Webview* Vanitas::Webview::runWindow() {
    this->run();
    return this;
}

Vanitas::Webview* Vanitas::Webview::terminateWindow() {
    this->terminate();
    return this;
}

webview_hint_t Vanitas::Webview::getHintType(std::string hint_str) {
    if (hint_str == "NONE" || hint_str == "0") {
        return WEBVIEW_HINT_NONE;
    } else if (hint_str == "MIN" || hint_str == "1") {
        return WEBVIEW_HINT_MIN;
    } else if (hint_str == "MAX" || hint_str == "2") {
        return WEBVIEW_HINT_MAX;
    } else if (hint_str == "FIXED" || hint_str == "3") {
        return WEBVIEW_HINT_FIXED;
    } else {
        spdlog::error("Unknown hint type entered in 'getHintType': " + hint_str);
        return WEBVIEW_HINT_NONE;
    }
}

void Vanitas::Webview::start() {
    this->refreshSize()
        ->refreshTitle()
        ->setHTML()
        ->bindAll()
        ->runWindow()
        ->unbindAll();
}
