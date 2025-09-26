// Adapted from HTTP server, asynchronous ex ample found here: https://github.com/boostorg/beast

#ifndef RENWEB_WEB_SERVER_H
#define RENWEB_WEB_SERVER_H
#include "info.hpp"
// #include "webview.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/string_type.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/regex.hpp>
#include <map>
#include <memory>
#include <spdlog/spdlog.h>
#include <boost/process.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/url.hpp>
#include <boost/thread.hpp>
#include <memory>
#include <string>
#include <sstream>
#include <string_view>

#define MAX_NUM_PORTS_TO_TRY 64

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using json = nlohmann::json;
using tcp = boost::asio::ip::tcp;

namespace RenWeb {
    class Session : public std::enable_shared_from_this<Session> {
        private:
            beast::tcp_stream stream;
            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            std::shared_ptr<void> res;
            template<bool isRequest, class Body, class Fields>
                void send(http::message<isRequest, Body, Fields>&&); 
            void send_error(http::status, std::string); 
            void handle_current_request();
          // APIs
            void handle_get();
            void handle_put();
            void handle_delete();
          // Responses
            void get_file();
            http::response<http::string_body> get_bad_request();
            http::response<http::string_body> get_not_found();
            http::response<http::string_body> get_server_error();

            std::string errorify(http::status, std::string);
            void do_read();
            void on_read(beast::error_code ec, std::size_t bytes_transferred);
            void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred);
            void close();
        public:
            Session(tcp::socket&&);
            ~Session();
            void run();
            static std::string MIME(std::string);
    };
    class Listener : public std::enable_shared_from_this<Listener> {
        private:
            net::io_context& ioc;
            tcp::acceptor acceptor;
            void do_accept();
            void on_accept(beast::error_code, tcp::socket);
            std::string url;
        public:
            void run();
            std::string getURL();
            Listener(net::io_context&, tcp::endpoint);
            ~Listener();
    }; 
    class WebServer {
        private:
            net::io_context ioc;
            std::vector<boost::thread> threads;
            std::shared_ptr<RenWeb::Listener> listener = nullptr;
            // std::make_shared<RenWeb::Web::Listener>;
        public:
            std::string getURL();
            static bool isURI(std::string);
            WebServer(unsigned short, unsigned short, std::string="127.0.0.1");
            ~WebServer();
    };
}

#endif