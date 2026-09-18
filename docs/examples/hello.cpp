#define FT_IMPLEMENTATION
#include "ft.hpp"

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "Hello FT";
    cfg.web_host = "127.0.0.1";
    if (!ft::create_window(cfg, argc, argv)) return 1;

    char name[64] = "operator";
    bool enabled = true;
    while (ft::pump()) {
        ft::begin();
        ft::text("Hello FT");
        ft::input("Name", name, sizeof(name));
        ft::checkbox("Enabled", &enabled);
        if (ft::button("Save", ft::ColorRole::Accent)) {
            ft::toast_success("Saved");
        }
        ft::end();
    }
    ft::shutdown();
}
