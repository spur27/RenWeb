#include "../include/info.hpp"
#include "logger_addons.hpp"
#include <exception>
#include <spdlog/spdlog.h>

/* ********************** */
/* ******** FILE ******** */
/* ********************** */
    /*static*/ void RenWeb::Info::File::refresh() {
        RenWeb::Info::File::name = boost::dll::program_location().filename().string();
        RenWeb::Info::File::dir = boost::dll::program_location().parent_path().string();
        RenWeb::Info::File::path = boost::dll::program_location().string();
    }
/* ********************** */
/* ******** APP ********* */
/* ********************** */
    /*static*/ void RenWeb::Info::App::refresh(json&& json_v) {
        if (RenWeb::Info::App::page.empty()) {
            spdlog::error("Can't refresh setting from a config when no page is set!");
            return;
        }
        try {
            RenWeb::Info::App::name = json_v.value("name", RenWeb::Info::App::name);
            RenWeb::Info::App::version = json_v.value("version", RenWeb::Info::App::version);
            RenWeb::Info::App::description = json_v.value("description", RenWeb::Info::App::description);
            RenWeb::Info::App::license = json_v.value("license", RenWeb::Info::App::license);
            RenWeb::Info::App::author = json_v.value("author", RenWeb::Info::App::author);
            RenWeb::Info::App::hint = json_v.value("hint", RenWeb::Info::App::hint);
            RenWeb::Info::App::width = json_v.value("width", RenWeb::Info::App::width);
            RenWeb::Info::App::height = json_v.value("height", RenWeb::Info::App::height);
            RenWeb::Info::App::save_resize_dimensions = json_v.value("save_resize_dimensions", RenWeb::Info::App::save_resize_dimensions);
            RenWeb::Info::App::resizable = json_v.value("resizable", RenWeb::Info::App::resizable);
            RenWeb::Info::App::decorated = json_v.value("decorated", RenWeb::Info::App::decorated);
            RenWeb::Info::App::keep_above = json_v.value("keep_above", RenWeb::Info::App::keep_above);
        } catch (const std::exception& e) {
            spdlog::error(e.what());
        }
    }

    /*static*/ void RenWeb::Info::App::loadFromConfigFile() {
        if (RenWeb::Info::App::page.empty()) {
            spdlog::error("Can't refresh setting from a config when no page is set!");
            return;
        }
        const std::filesystem::path config_path(RenWeb::Info::File::dir + "/config.json");
        if (!std::filesystem::exists(config_path)) {
            spdlog::error("No \"config.json\" found at '" + config_path.string() +"'");
            return;
        }
        try {
            std::ifstream config_file_contents(config_path, std::ios_base::binary);
            json config_json;
            config_file_contents >> config_json;
            config_file_contents.close();
            json settings = config_json[RenWeb::Info::App::page];
            if (!settings.is_object()) {
                throw std::runtime_error("Can't find settings object for \"" + RenWeb::Info::App::page + "\"");
            }
            RenWeb::Info::App::refresh(std::move(settings));
        } catch (const std::exception& e) {
            spdlog::error(e.what());
        }
    }

    /*static*/ void RenWeb::Info::App::resetPageToDefault() {
        RenWeb::Info::App::page = RENWEB_INFO_DEFAULT_PAGE;
        spdlog::debug(std::string("Reset page to default ") + RENWEB_INFO_DEFAULT_PAGE);
        // RenWeb::Config::setSetting(this->html_doc_name, json_v);
    }


    /*static*/ void RenWeb::Info::App::resetToDefaults() {
        RenWeb::Info::App::name = RENWEB_INFO_DEFAULT_NAME;
        RenWeb::Info::App::version = RENWEB_INFO_DEFAULT_VERSION;
        RenWeb::Info::App::description = RENWEB_INFO_DEFAULT_DESCRIPTION;
        RenWeb::Info::App::license = RENWEB_INFO_DEFAULT_LICENSE;
        RenWeb::Info::App::author = RENWEB_INFO_DEFAULT_AUTHOR;
        RenWeb::Info::App::hint = RENWEB_INFO_DEFAULT_HINT;
        RenWeb::Info::App::width = RENWEB_INFO_DEFAULT_WIDTH;
        RenWeb::Info::App::height = RENWEB_INFO_DEFAULT_HEIGHT;
        RenWeb::Info::App::save_resize_dimensions = RENWEB_INFO_DEFAULT_SAVE_RESIZE_DIMENSIONS;
        RenWeb::Info::App::resizable = RENWEB_INFO_DEFAULT_RESIZABLE;
        RenWeb::Info::App::decorated = RENWEB_INFO_DEFAULT_DECORATED;
        RenWeb::Info::App::keep_above = RENWEB_INFO_DEFAULT_KEEP_ABOVE;
        spdlog::debug("Settings reset to defaults");
        // RenWeb::Config::setSetting(this->html_doc_name, json_v);
    }


    /*static*/ void RenWeb::Info::App::save() {
        if (RenWeb::Info::App::page.empty()) {
            spdlog::error("Can't save settings when no page is set!");
            return;
        }
        const std::filesystem::path config_path(RenWeb::Info::File::dir + "/config.json");
        json json_v;
        if (std::filesystem::exists(config_path)) {
            std::ifstream config_file_contents(config_path);
            config_file_contents >> json_v;
            config_file_contents.close();
        } else {
            json_v = json::object();
        }
        if (!json_v[RenWeb::Info::App::page].is_object()) {
            spdlog::warn("Weird/missing settings found for '" + RenWeb::Info::App::page +"'. Setting them...");
            json_v[RenWeb::Info::App::page] = json::object();
        }
        json curr_settings = json::object();
        curr_settings[RenWeb::Info::App::page] = RenWeb::Info::App::get();
        json_v.update(curr_settings, true);
        try {
            std::ofstream config_file(config_path, std::ios_base::trunc);
            config_file << json_v.dump(2);
            config_file.close();
        } catch (const std::exception& e) {
            spdlog::error(e.what());
        }
        // RenWeb::Config::setSetting(this->html_doc_name, json_v);
    }
    /*static*/ json RenWeb::Info::App::get() {
        json json_v = { 
            { "name", RenWeb::Info::App::name },
            { "page", RenWeb::Info::App::page },
            { "version", RenWeb::Info::App::version },
            { "description", RenWeb::Info::App::description },
            { "license", RenWeb::Info::App::license },
            { "author", RenWeb::Info::App::author },
            { "hint", RenWeb::Info::App::hint },
            { "width", RenWeb::Info::App::width },
            { "height", RenWeb::Info::App::height },
            { "save_resize_dimensions", RenWeb::Info::App::save_resize_dimensions },
            { "resizable", RenWeb::Info::App::resizable },
            { "decorated", RenWeb::Info::App::decorated },
            { "keep_above", RenWeb::Info::App::keep_above }
        };
        return json_v;
        spdlog::debug("###APP INFO###\n" + json_v.dump(2));
    }

    /*static*/ void RenWeb::Info::App::set(json&& val) {
        if (RenWeb::Info::App::page.empty()) {
            spdlog::error("Can't set settings when no page is set!");
            return;
        }
        json curr_json = RenWeb::Info::App::get();
        curr_json.update(val, true);
        RenWeb::Info::App::refresh(std::move(curr_json));
    }
