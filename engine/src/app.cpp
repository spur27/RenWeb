#include "../include/app.hpp"

RenWeb::App::App() {
    this->addArgs();
}

RenWeb::App::~App() { }

void RenWeb::App::addArgs() {
    this->desc.add_options()
        ("help,h", "Displays help info")
        ("version,v", "Displays version info")
        ("clear,c", "Clears the log file")
        ("log_level,l", boost::program_options::value<unsigned int>()->default_value(2, "2 (info)"), "Sets log level (n>=0)")
        ("thread_count,t", boost::program_options::value<unsigned short>()->default_value(4, "4"), "Number of threads (n>=0)")
        ("port,p", boost::program_options::value<unsigned short>()->default_value(8270, "8270"), "Web server port (n>=0)")
        ("ip,i", boost::program_options::value<std::string>(), "IP of web server")
        ("pages,P", boost::program_options::value<std::vector<std::string>>()->multitoken()->default_value(std::vector<std::string>{}, RENWEB_INFO_DEFAULT_PAGE), "List of pages to open");
        // add custom args here
}

void RenWeb::App::run(int argc, char** argv) {
    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, RenWeb::App::desc), vm);
        if (vm.count("help")) {
            std::cout << RenWeb::App::desc << std::endl;
            return;
        } else if (vm.count("version")) {
            std::cout << RenWeb::Info::App::name << " (" << RenWeb::Info::App::version << ")" << std::endl;
            exit(1);
        } else if (vm.count("clear")) {
            spdlog::clear();
            std::cout << "Log file at " << spdlog::getLogFilePath() << " cleared." << std::endl;
            exit(1);
        } // add more else ifs here

        this->pages = vm["pages"].as<std::vector<std::string>>();
        const unsigned int pages_vec_size = RenWeb::App::pages.size();
        const unsigned int log_level = vm["log_level"].as<unsigned int>();
        if (pages_vec_size <= 1) {
            if (this->pages.empty() || this->pages[0] == "_") {
                RenWeb::Info::App::resetPageToDefault();
            } else {
                RenWeb::Info::App::page = this->pages[0];
            }
        }
        spdlog::actual_level = spdlog::level::level_enum(log_level);
#if defined(_WIN32)
        if (spdlog::actual_level < spdlog::level::info) {
            AllocConsole();
        }
#endif
        spdlog::refresh();
        RenWeb::Info::App::loadFromConfigFile();
        // setup everything else now
        if (pages_vec_size > 1) {
            std::vector<boost::process::child> child_procs;
            for (auto page_name_v : RenWeb::App::pages) {
                if (page_name_v == "_") page_name_v = RENWEB_INFO_DEFAULT_PAGE;
                try {
                    child_procs.push_back(boost::process::child(RenWeb::Info::File::path, "-P", page_name_v, "-l", std::to_string(log_level), boost::process::std_out > stdout, boost::process::std_err > stderr, boost::process::std_in < stdin));
                } catch (const std::exception& e) {
                    spdlog::error(e.what());
                }
            }
            for (auto& proc : child_procs) {
                while(proc.running()) {
                    boost::this_thread::sleep(boost::posix_time::milliseconds(500));
                }
                proc.join();
            }
        } else {
            if (RenWeb::Info::App::page.empty()) {
                throw std::runtime_error("Page cannot be empty when running a single page!!");
            }
            const unsigned short thread_cnt = std::min<unsigned short>(64, std::max<unsigned short>(1, vm["thread_count"].as<unsigned short>()));
            const unsigned short port = std::min<unsigned short>(65535, std::max<unsigned short>(0, vm["port"].as<unsigned short>()));
            if (vm.count("ip")) {
                const std::string ip = vm["ip"].as<std::string>();
                spdlog::info("Starting single window with " + std::to_string(thread_cnt) + " threads at ip \"" + ip + "\" on port \"" + std::to_string(port) + "\"");
                std::unique_ptr<RenWeb::Window> window(new RenWeb::Window(thread_cnt, port, ip));
                window->start();
            } else {
                spdlog::info("Starting single window with " + std::to_string(thread_cnt) + " threads on port \"" + std::to_string(port) + "\"");
                std::unique_ptr<RenWeb::Window> window(new RenWeb::Window(thread_cnt, port));
                window->start();
            }
        }
    } catch (const std::exception& e) {
        spdlog::critical(std::string("[ARGS BLOCK] ") + e.what());
    }
}

    

