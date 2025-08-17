#include "../include/app.hpp"

int main(int argc, char** argv) {
    std::unique_ptr<RenWeb::App> app(new RenWeb::App());
    app->run(argc, argv);
    return 0;
}