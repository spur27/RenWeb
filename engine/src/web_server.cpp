#include "../include/web_server.hpp"

RenWeb::Session::Session(tcp::socket&& socket)
  // Take ownership of the stream
    : stream(std::move(socket))
{ 
    // spdlog::trace("SESSION STARTED");
}

RenWeb::Session::~Session() { 
    // spdlog::trace("SESSION DESTROYED");
}

std::string RenWeb::Session::errorify(http::status status, std::string why) {
return (R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        * {
            color: white;
        }
        html {
            background-color: #2b282e;
        }
        .row {
            display:flex;
            flex-direction: row;
            align-items: center;
            gap: 10px;
        }
        p {
            font-size: 12px;
        }
        button {
            background-color: #2b282e;
        }
        button:hover {
            background-color: #3d3942ff;
        }
    </style>
    <script defer>
        window.onload = async () => {
            await BIND_show();
        };
        document.querySelector(".reload").onclick = async () => {
            await BIND_reloadPage();
        };
    </script>
</head>
<body>
    <div class="row">
        <h1 class="error_code">)") + std::to_string(static_cast<int>(status)) + std::string(R"(</h1>
        <p>-</p>
        <h2 class="error_message">)") + std::string(http::obsolete_reason(status)) + std::string(R"(</h2>
    </div>
    <p>)") + why + std::string(R"(</p>
    <button class="reload">Reload</button>
</body>
</html>)");
}

void RenWeb::Session::handle_current_request() {
    if (this->req.method() != http::verb::get && this->req.method() != http::verb::head) {
        // return send(bad_request())
    }
    spdlog::info("[SERVER] Recieved \"" + std::string(this->req.method_string()) +  "\" request from \"http://" + this->stream.socket().remote_endpoint().address().to_string() + ":" + std::to_string(this->stream.socket().remote_endpoint().port()) + "\"" + " with target \"" + std::string(req.target()) + "\"");
    switch (this->req.method()) {
        case http::verb::get:
        case http::verb::head:
            this->handle_get();
            break;
        case http::verb::put:
        case http::verb::post:
            // this->handle_put();
            // break;
        case http::verb::delete_:
            // this->handle_delete();
            // break;
        default:
            const std::string err_msg = "\"" + std::string(this->req.method_string()) + "\"" + " Isn't a supported request.";
            this->send_error(http::status::bad_request, err_msg);
            break;
    }
}

void RenWeb::Session::handle_get() {
    spdlog::debug(std::string("[SERVER] asking for target '") + std::string(this->req.target()) + "'");
    if (this->req.target().empty() || this->req.target()[0] != '/' || this->req.target().find("..") != beast::string_view::npos) {
        const std::string err_msg = "\"" + std::string(this->req.target()) + "\" is not a valid GET request target.";
        spdlog::error("[SERVER] " + err_msg);
        return this->send_error(http::status::bad_request, err_msg);
    } else if (RenWeb::Info::App::page.empty()) {
      // This should never happen, but just in case...
        spdlog::warn("[SERVER] No page is set. Resetting to default.");
        RenWeb::Info::App::resetPageToDefault();
    }
    std::filesystem::path resource_path = std::filesystem::path(std::string(this->req.target()));
    if (std::string(this->req.target()) == "/" || std::string(this->req.target()) == "\\") {
        resource_path = std::filesystem::path("index.html");
    } else if (std::string(this->req.target())[0] == '/' || std::string(this->req.target())[0] == '\\') {
        resource_path = std::filesystem::path(std::string(this->req.target()).substr(1));
    } else {
        resource_path = std::filesystem::path(std::string(this->req.target()));
    }
    if (resource_path.string().rfind("assets", 0) == 0) {
        resource_path = std::filesystem::path(resource_path.string().substr(7));
        std::filesystem::path custom_resource_path = std::filesystem::path(RenWeb::Info::File::dir) / "custom" / resource_path;
        if (std::filesystem::exists(custom_resource_path)) {
            resource_path = custom_resource_path;
        } else {
            resource_path = std::filesystem::path(RenWeb::Info::File::dir) / "assets" / resource_path;
        }
    } else {
        resource_path = std::filesystem::path(RenWeb::Info::File::dir) / "content" / RenWeb::Info::App::page / resource_path;
    }
    if (!std::filesystem::exists(resource_path)) {
        spdlog::error("[SERVER] file \"" + resource_path.string() + "\" was not found.");
        const std::string err_msg = "\"" + resource_path.string() + "\" cannot be found!";
        return this->send_error(http::status::not_found, err_msg);
    }
    try {
        beast::error_code ec;
        http::file_body::value_type body;
        body.open(resource_path.string().c_str(), beast::file_mode::scan, ec);
        if (ec) throw beast::system_error(ec);
        auto const size = body.size();
        spdlog::debug("[SERVER] reading \"" + std::to_string(size) + "\" bytes from \"" + resource_path.string() + "\"");
        if (this->req.method() == http::verb::head) {
            http::response<http::empty_body> res_v{http::status::ok, req.version()};
            res_v.set(http::field::server, std::string_view(std::to_string(static_cast<int>(http::field::version))));
            res_v.set(http::field::content_type, this->MIME(resource_path.string()));
            res_v.content_length(size);
            res_v.keep_alive(req.keep_alive());
            return this->send(std::move(res_v));
        }
        http::response<http::file_body> res_v{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
        res_v.set(http::field::server, std::string_view(std::to_string(static_cast<int>(http::field::version))));
        res_v.set(http::field::content_type, this->MIME(resource_path.string()));
        res_v.content_length(size);
        res_v.keep_alive(req.keep_alive());
        return this->send(std::move(res_v));
    } catch (const beast::system_error& e) {
        spdlog::error("[SERVER] " + std::string(e.what()));
        this->send_error(http::status(e.code().value()), e.what());
    } catch (const std::exception& e) {
        spdlog::error("[SERVER] " + std::string(e.what()));
        this->send_error(http::status::internal_server_error, e.what());
    }
}
void RenWeb::Session::handle_put() {

}
void RenWeb::Session::handle_delete() {}
void RenWeb::Session::get_file() {}

void RenWeb::Session::send_error(http::status status, std::string why) {
    http::response<http::string_body> res_v = {status, this->req.version()};
    res_v.set(http::field::server, std::string_view(std::to_string(static_cast<int>(http::field::version))));
    res_v.set(http::field::content_type, "text/html");
    res_v.keep_alive(req.keep_alive());
    res_v.body() = this->errorify(status, why);
    res_v.prepare_payload();
    return this->send(std::move(res_v));
}

template<bool isRequest, class Body, class Fields>
void RenWeb::Session::send(http::message<isRequest, Body, Fields>&& msg) {
    // The lifetime of the message has to extend
    // for the duration of the async operation so
    // we use a shared_ptr to manage it.
    auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(std::move(msg));

    // Store a type-erased version of the shared
    // pointer in the class to keep it alive.
    this->res = sp;

    // Write the response
    http::async_write(
        this->stream,
        *sp,
        beast::bind_front_handler(
            &Session::on_write,
            shared_from_this(),
            sp->need_eof()));
}


void RenWeb::Session::do_read() {
    // Make the request empty
    this->req = {};
    //Set the timeout
    // this->stream.expires_after(std::chrono::seconds(30));
    // Read a request
    http::async_read(this->stream, this->buffer, this->req,
        beast::bind_front_handler(
                &Session::on_read,
                shared_from_this()));
}

void RenWeb::Session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec == http::error::end_of_stream) {
        return this->close();
    } else if (ec) {
        throw boost::system::system_error(ec);
    }
    this->handle_current_request();
}

void RenWeb::Session::on_write(bool close, beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        throw boost::system::system_error(ec);
    } else if (close) {
        // The response likely indicated the "Connection: close" semantic
        return this->close();
    }
    // Delete the response
    this->res = nullptr;
    // Read another request
    this->do_read();
}

void RenWeb::Session::close() {
    try {
        // Send TCP shutdown
        this->stream.socket().shutdown(tcp::socket::shutdown_send);
    } catch (const std::exception& e) {
        spdlog::error(e.what());
    }
}

void RenWeb::Session::run() {
    // Dispatch a task to read from the stream
    net::dispatch(this->stream.get_executor(),
        beast::bind_front_handler(&RenWeb::Session::do_read, shared_from_this()));
}

/*static*/ std::string RenWeb::Session::MIME(std::string target) {
    auto const ext = std::filesystem::path(target).extension().string();
    if(ext == ".htm")  return "text/html";
    else if(ext == ".html") return "text/html";
    else if(ext == ".php")  return "text/html";
    else if(ext == ".css")  return "text/css";
    else if(ext == ".txt")  return "text/plain";
    else if(ext == ".js")   return "application/javascript";
    else if(ext == ".json") return "application/json";
    else if(ext == ".xml")  return "application/xml";
    else if(ext == ".swf")  return "application/x-shockwave-flash";
    else if(ext == ".flv")  return "video/x-flv";
    else if(ext == ".png")  return "image/png";
    else if(ext == ".jpe")  return "image/jpeg";
    else if(ext == ".jpeg") return "image/jpeg";
    else if(ext == ".jpg")  return "image/jpeg";
    else if (ext == ".gif")  return "image/gif";
    else if (ext == ".bmp")  return "image/bmp";
    else if (ext == ".ico")  return "image/vnd.microsoft.icon";
    else if(ext == ".tiff") return "image/tiff";
    else if(ext == ".tif")  return "image/tiff";
    else if (ext == ".svg")  return "image/svg+xml";
    else if (ext == ".svgz") return "image/svg+xml";
    else return "application/text";
}

// ----------------------------------------- //
// ----------------------------------------- //
// ----------------------------------------- //
// ----------------------------------------- //

RenWeb::Listener::Listener(net::io_context& ioc_v, tcp::endpoint base_endpoint_v)
    : ioc(ioc_v)
    , acceptor(net::make_strand(ioc_v)) 
{
    spdlog::debug("[SERVER SETUP] Starting listener construction");
    for (unsigned short port_offset = 0; port_offset < MAX_NUM_PORTS_TO_TRY; port_offset++) {
        try {
            const unsigned short current_port = std::min<unsigned short>(65535, base_endpoint_v.port() + port_offset);
            tcp::endpoint endpoint_v = {base_endpoint_v.address(), current_port};
            this->url = "http://" + endpoint_v.address().to_string() + ":" + std::to_string(endpoint_v.port());
            spdlog::info("[SERVER SETUP] Opening on \"" + this->url + "\"");
            this->acceptor.open(endpoint_v.protocol());
            this->acceptor.set_option(net::socket_base::reuse_address(true));
            spdlog::debug("[SERVER SETUP] Binding...");
            this->acceptor.bind(endpoint_v);
            spdlog::debug("[SERVER SETUP] Listening...");
            this->acceptor.listen(net::socket_base::max_listen_connections);        
            return;
        } catch (const boost::system::system_error& e) {
            if (e.code() == net::error::address_in_use) {
                spdlog::warn("[SERVER SETUP] Port \""+std::to_string(base_endpoint_v.port()+port_offset) + "\" is busy. Trying \"" + std::to_string(std::min<unsigned short>(65535, base_endpoint_v.port()+port_offset+1)) + "\"");
                this->acceptor.cancel();
                this->acceptor.close();
            } else {
                throw e;
            }
        }
    }
    throw std::runtime_error("[SERVER SETUP] can't find an open port! Tried " + std::to_string(MAX_NUM_PORTS_TO_TRY) + " ports starting from \"" + std::to_string(base_endpoint_v.port() - MAX_NUM_PORTS_TO_TRY) + "\"");
}

std::string RenWeb::Listener::getURL() {
    if (!this->url.empty()) {
        return this->url;
    } else {
      // This should never happen.
        const std::string err_msg = "URL is somehow empty!";
        spdlog::critical(err_msg);
        throw std::runtime_error(err_msg);
    }
}

RenWeb::Listener::~Listener() {
    spdlog::debug("[SERVER TAKEDOWN] Starting Listener deconstruction");
    spdlog::debug("[SERVER TAKEDOWN] Stopping listener acceptor");
    this->acceptor.cancel();
    this->acceptor.close();
    spdlog::debug("[SERVER TAKEDOWN] Done with Listener deconstruction");
}

void RenWeb::Listener::run() {
    // Start accepting connections
    spdlog::debug("[SERVER SETUP] Running listener...");
    this->do_accept();
}

void RenWeb::Listener::do_accept() {
    // Give a new connection its own strand
    this->acceptor.async_accept(
        net::make_strand(this->ioc),
        beast::bind_front_handler(
            &RenWeb::Listener::on_accept,
                    shared_from_this()));
}

void RenWeb::Listener::on_accept(beast::error_code ec,tcp::socket socket) {
    if (ec) {
        throw boost::system::system_error(ec);
    }
    std::make_shared<RenWeb::Session>(std::move(socket))->run();
    this->do_accept();
}

// ----------------------------------------- //
// ----------------------------------------- //
// ----------------------------------------- //
// ----------------------------------------- //

std::string RenWeb::WebServer::getURL() {
    if (this->listener != nullptr) {
        return this->listener->getURL();
    } else {
      // This will only happen if the constructor doesn't actually create a listener.
        const std::string err_msg = "Can't get url--no listener is set! Something broke in the constructor.";
        spdlog::critical(err_msg);
        throw std::runtime_error(err_msg);
    }
}

RenWeb::WebServer::WebServer(unsigned short thread_cnt_v, unsigned short port_v, std::string ip_v)
    : ioc(thread_cnt_v)
{
    spdlog::debug("[SERVER SETUP] making IOC listener");
    this->listener = std::make_shared<RenWeb::Listener>(this->ioc, tcp::endpoint{net::ip::make_address(ip_v), port_v});
    this->listener->run();
    this->threads.reserve(thread_cnt_v);
    for (unsigned short i = thread_cnt_v; i > 0; --i) {
        spdlog::trace("[SERVER SETUP] Starting IOC thread " + std::to_string(thread_cnt_v-i+1));
        this->threads.emplace_back([this]{this->ioc.run();});
    }
}

RenWeb::WebServer::~WebServer() {
    spdlog::debug("[SERVER TAKEDOWN] Starting Web Server deconstruction");
    spdlog::debug("[SERVER TAKEDOWN] Stopping IOC");
    this->ioc.stop();
    spdlog::debug("[SERVER TAKEDOWN] Stopping threads");
    unsigned short current_thread_index = 1;
    for (auto& i : this->threads) {
        i.interrupt();
        spdlog::trace("[SERVER TAKEDOWN] Interrupted thread \"" + std::to_string(current_thread_index) + "\". Joining...");
        i.join();
        current_thread_index++;
    }
    this->threads.clear();
    spdlog::debug("[SERVER TAKEDOWN] Done with Web Server deconstruction");
}

