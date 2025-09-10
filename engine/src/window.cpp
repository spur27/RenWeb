#include "../include/window.hpp"
#include "info.hpp"
 
RenWeb::Window::Window(unsigned short thread_cnt, unsigned short port)
  : webview::webview(false, nullptr)
  , process_manager()
  , web_server(thread_cnt, port)
{
    spdlog::debug("Window object constructed.");
}

RenWeb::Window::Window(unsigned short thread_cnt, unsigned short port, std::string ip)
  : webview::webview(false, nullptr)
  , process_manager()
  , web_server(thread_cnt, port, ip)
{
    spdlog::debug("Window object constructed.");
}

RenWeb::Window::~Window() {
    spdlog::debug("Window object deconstructed.");
}

RenWeb::Window* RenWeb::Window::processContents() {
    this->set_html("<html\"><head><style>html { backgroundColor: black; width: 100vw; height: 100vh; }</style></head></html>");
#if defined(_WIN32)
    // this->hide();
    this->reloadPage();
#elif defined(__APPLE__)
    spdlog::critical("hideUntilContentLoaded NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    this->reloadPage()
        ->hide();
#endif
    return this;
}

bool RenWeb::Window::isURI(std::string maybe_uri) {
    // "\\b((?:https?|ftp|file)://[-a-zA-Z0-9+&@#/%?=~_|!:, .;]*[-a-zA-Z0-9+&@#/%=~_|])"
    const boost::regex url_regex(
        "\\b((?:https?|ftp|file)://[-a-zA-Z0-9+&@#/%?=~_|!:, .;]*[-a-zA-Z0-9+&@#/%=~_|])",
        boost::regex::icase);
    return boost::regex_match(maybe_uri, url_regex);
}

RenWeb::Window* RenWeb::Window::setSize(unsigned int width, unsigned int height, webview_hint_t hint) {
    this->set_size(width, height, hint);
    spdlog::debug("Window dimensions set to " + std::to_string(width) + "x" + std::to_string(height) + " of hint '" + std::to_string(hint) + "'");
    return this;
}

RenWeb::Window* RenWeb::Window::updateSize(bool force) {
    if (!force && !RenWeb::Info::App::save_resize_dimensions) return this;
    try {
        auto window_dims = this->getSize();
        RenWeb::Info::App::width = window_dims.first;
        RenWeb::Info::App::height = window_dims.second;
        spdlog::trace("Window dimensions updated to " + std::to_string(RenWeb::Info::App::width) + "x" + std::to_string(RenWeb::Info::App::height) + "'");
    } catch (const std::exception& e) {
        spdlog::error("Couldn't update window dimensions (is it open\?\?)");
        spdlog::error(e.what());
    }
    return this;
}

RenWeb::Window* RenWeb::Window::bindFunction(std::string fn_name, sync_binding_t fn) {
    if (this->bind_cache.find(fn_name) != this->bind_cache.end()) {
        spdlog::warn("Binding already exists for function \"" + fn_name + "\". Overwriting...");
    }
    this->bind(fn_name, fn);
    this->bind_cache[fn_name] = fn;
    spdlog::trace("Bound '" + fn_name + "'");
    return this;
}

RenWeb::Window* RenWeb::Window::unbindFunction(std::string fn_name) {
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

RenWeb::Window* RenWeb::Window::unbindAll() {
    auto bind_cache_copy(this->bind_cache);
    for (auto& [key, val] : bind_cache_copy) {
        this->unbindFunction(key);
    }
    return this;
}

RenWeb::Window* RenWeb::Window::runWindow() {
    this->run();
    return this;
}

RenWeb::Window* RenWeb::Window::terminateWindow() {
    this->terminate();
    return this;
}

RenWeb::Window* RenWeb::Window::reloadPage() {
    this->navigatePage("_");
    return this;
}

RenWeb::Window* RenWeb::Window::navigatePage(std::string uri) {
    if (uri != "_") RenWeb::Info::App::page = uri;
    if (this->isURI(RenWeb::Info::App::page)) {
        spdlog::warn("Navigating to page " + RenWeb::Info::App::page);
        this->navigate(RenWeb::Info::App::page);
    } else {
        spdlog::warn("Navigating to " + this->web_server.getURL() + " to display page of name " + RenWeb::Info::App::page);
        this->navigate(this->web_server.getURL());
    }
    return this;
}

RenWeb::Window* RenWeb::Window::refreshSettings() {
  // SIZE
    this->set_size(RenWeb::Info::App::width, RenWeb::Info::App::height, RenWeb::Info::App::hint);
  // TITLE
    if (spdlog::actual_level < spdlog::level::info) {
        this->set_title(RenWeb::Info::App::name + " ~ " + RenWeb::Info::App::page + " (" + this->web_server.getURL() + ")");
    } else {
        this->set_title(RenWeb::Info::App::name);
    }
  // DECORATED
    this->setDecorated(RenWeb::Info::App::decorated);
  // RESIZABLE
    this->setResizable(RenWeb::Info::App::resizable);
  // KEEP ABOVE
    this->setKeepAbove(RenWeb::Info::App::keep_above);

    return this;
}

std::pair<unsigned int, unsigned int> RenWeb::Window::getSize() {
    int width, height;
#if defined(_WIN32)
    RECT rect;
    HWND hwnd = GetActiveWindow();
    GetClientRect(hwnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
#elif defined(__APPLE__)
    spdlog::critical("getSize NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    gtk_window_get_size(GTK_WINDOW(window_widget), &width, &height);
#endif
    return std::make_pair(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
}

RenWeb::Window* RenWeb::Window::setDecorated(bool enable) {
#if defined(_WIN32)
    HWND hwnd = GetActiveWindow();
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (enable) {
        style |= (WS_CAPTION | WS_THICKFRAME | WS_BORDER | WS_DLGFRAME);
    } else {
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_BORDER | WS_DLGFRAME);
    }
    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
#elif defined(__APPLE__)
    spdlog::critical("decorated NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    gtk_window_set_decorated(GTK_WINDOW(window_widget), enable);
#endif
    RenWeb::Info::App::decorated = enable;
    return this;
}


RenWeb::Window* RenWeb::Window::setResizable(bool enable) {
#if defined(_WIN32)
    HWND hwnd = GetActiveWindow();
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (enable) {
        style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
    } else {
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
#elif defined(__APPLE__)
    spdlog::critical("resizable NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    int width, height;
    gtk_window_get_size(GTK_WINDOW(window_widget), &width, &height);
    gtk_window_set_default_size(GTK_WINDOW(window_widget), width, height);
    gtk_window_set_resizable(GTK_WINDOW(window_widget), enable);
#endif
    RenWeb::Info::App::resizable = enable;
    return this;
}

RenWeb::Window* RenWeb::Window::setKeepAbove(bool enable) {
#if defined(_WIN32)
    HWND hwnd = GetActiveWindow();
    SetWindowPos(hwnd, (enable) ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#elif defined(__APPLE__)
    spdlog::critical("keep above NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    gtk_window_set_keep_above(GTK_WINDOW(window_widget), enable);
#endif
    RenWeb::Info::App::keep_above = enable;
    return this;
}

RenWeb::Window* RenWeb::Window::minimize() {
#if defined(_WIN32)
    HWND hwnd = GetActiveWindow();
    ShowWindow(hwnd, (IsIconic(hwnd)) ? SW_RESTORE : SW_MINIMIZE);
#elif defined(__APPLE__)
    spdlog::critical("minimize NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    gtk_window_iconify(GTK_WINDOW(window_widget));
#endif
    return this;
}
RenWeb::Window* RenWeb::Window::maximize() {
#if defined(_WIN32)
    HWND hwnd = GetActiveWindow();
    ShowWindow(hwnd, (IsZoomed(hwnd)) ? SW_RESTORE : SW_MAXIMIZE);
#elif defined(__APPLE__)
    spdlog::critical("maximize NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    if ((gdk_window_get_state(gtk_widget_get_window(GTK_WIDGET(window_widget))) & GDK_WINDOW_STATE_MAXIMIZED) != 0) {
        gtk_window_unmaximize(GTK_WINDOW(window_widget));
    } else {
        gtk_window_maximize(GTK_WINDOW(window_widget));
    }
#endif
    return this;
}
RenWeb::Window* RenWeb::Window::fullscreen() {
#if defined(_WIN32)
    spdlog::critical("fullscreen NOT IMPLEMENTED FOR windows");
#elif defined(__APPLE__)
    spdlog::critical("fullscreen NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    if ((gdk_window_get_state(gtk_widget_get_window(GTK_WIDGET(window_widget))) & GDK_WINDOW_STATE_FULLSCREEN) != 0) {
        gtk_window_unfullscreen(GTK_WINDOW(window_widget));
    } else {
        gtk_window_fullscreen(GTK_WINDOW(window_widget));
    }
#endif
    return this;
}

RenWeb::Window* RenWeb::Window::hide() {
#if defined(_WIN32)
    HWND hwnd = GetActiveWindow();
    ShowWindow(hwnd, SW_HIDE);
    spdlog::critical("IN HIDE");
#elif defined(__APPLE__)
    spdlog::critical("fullscreen NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    gtk_widget_hide(GTK_WIDGET(window_widget));
#endif
    return this;
}

RenWeb::Window* RenWeb::Window::show() {
#if defined(_WIN32)
    HWND hwnd = GetActiveWindow();
    ShowWindow(hwnd, SW_SHOW);
    spdlog::critical("IN SHOW");
#elif defined(__APPLE__)
    spdlog::critical("fullscreen NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    gtk_widget_show(GTK_WIDGET(window_widget));
#endif
    return this;
}

std::vector<std::string> RenWeb::Window::openChooseFilesDialog(bool multi, bool dirs, RenWeb::ChooseFileDialogSettings* filtration) {
    std::vector<std::string> filepaths_vec{};
#if defined(_WIN32)
    boost::ignore_unused(multi);
    boost::ignore_unused(dirs);
    boost::ignore_unused(filtration);
    spdlog::critical("open file dialog NOT IMPLEMENTED FOR windows");
#elif defined(__APPLE__)
    spdlog::critical("open file dialog NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    auto window_widget = this->window().value();
    // Create the file chooser dialog
    std::stringstream instructions;
    instructions << "Choose ";
    if (multi) {
        instructions << "some ";
        if (dirs) {
            instructions << " directories";
        } else {
            instructions << " files";
        }
    } else {
        instructions << "a ";
        if (dirs) {
            instructions << " directory";
        } else {
            instructions << " file";
        }
    }
    GtkWidget *dialog = gtk_file_chooser_dialog_new(instructions.str().c_str(), GTK_WINDOW(window_widget),
                                         (dirs) ? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER : GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_OK,
                                         NULL);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), multi);
    if (filtration != nullptr) {
        for (const auto& i : filtration->patterns) {
            auto filter = gtk_file_filter_new();
            gtk_file_filter_set_name(filter, i.first.c_str());
            for (const auto& j : i.second) {
                spdlog::info("Adding " + j);
                gtk_file_filter_add_pattern(filter, j.c_str());
            }
            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        }
        for (const auto& i : filtration->mimes) {
            auto filter = gtk_file_filter_new();
            gtk_file_filter_set_name(filter, i.first.c_str());
            for (const auto& j : i.second) {
                gtk_file_filter_add_pattern(filter, j.c_str());
            }
            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        }
    }
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        // Get the selected file path
        auto filepaths = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        auto curr_filepath = filepaths;
        while (curr_filepath != nullptr) {
            filepaths_vec.push_back(std::string(static_cast<gchar*>(curr_filepath->data)));
            // g_free(curr_filepath);
            curr_filepath = curr_filepath->next;
        }
        g_slist_free(filepaths);
        spdlog::info("Selected " + std::to_string(filepaths_vec.size()) + " files.");
    } else {
        spdlog::error("Could not continue with choose file.");
    }
    // Destroy the dialog
    gtk_widget_destroy(dialog);
#endif
    return filepaths_vec;
}

RenWeb::Window* RenWeb::Window::sendNotif(std::string body, std::string summary, std::string icon_path) {
#if defined(_WIN32)
    spdlog::critical("sendNotif NOT IMPLEMENTED FOR apple");
#elif defined(__APPLE__)
    spdlog::critical("sendNotif NOT IMPLEMENTED FOR apple");
#elif defined(__linux__)
    notify_init(RenWeb::Info::App::name.c_str());
    NotifyNotification* n = notify_notification_new(summary.c_str(), body.c_str(), icon_path.c_str());
    notify_notification_show(n, nullptr);
    g_object_unref(G_OBJECT(n));
    notify_uninit();
#endif
    return this;
}


void RenWeb::Window::start() {
    this->refreshSettings()
        ->bindAll()
        ->processContents()
        ->runWindow()
        ->terminateWindow()
        ->unbindAll();
    this->process_manager.killProcesses();
    this->process_manager.cleanProcesses();
}
