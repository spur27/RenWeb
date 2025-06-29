#include "../include/webview_info.hpp"

Vanitas::WebviewInfo::WebviewInfo(std::string html_doc_name_v) {
    this->html_doc_name = html_doc_name_v;
}

void Vanitas::WebviewInfo::refresh() {
    auto json_v = Vanitas::Config::getSettings(this->html_doc_name);
    if (!json_v.is_object()) {
        spdlog::error("Cannot refresh settings! Settings under HTML doc name '" + this->html_doc_name + "' don't exist.");
        return;
    }
    this->window_title =                  json_v.value("window_title", this->window_title);
    this->window_hint =                   json_v.value("window_hint", this->window_hint);
    this->window_width =                  json_v.value("window_width", this->window_width);
    this->window_height =                 json_v.value("window_height", this->window_height);
    this->window_save_resize_dimensions = json_v.value("window_save_resize_dimensions", this->window_save_resize_dimensions);
    this->window_resizable =              json_v.value("window_resizable", this->window_resizable);
    spdlog::trace("Window Info object constructed.");
}

Vanitas::WebviewInfo::~WebviewInfo() {
    spdlog::trace("Window Info object deconstructed.");
}

void Vanitas::WebviewInfo::save() {
    json json_v = { 
        { "window_title", this->window_title },
        { "window_hint", this->window_hint },
        { "window_width", this->window_width },
        { "window_height", this->window_height },
        { "window_save_resize_dimensions", this->window_save_resize_dimensions },
        { "window_resizable", this->window_resizable }
    };
    Vanitas::Config::setSetting(this->html_doc_name, json_v);
}
