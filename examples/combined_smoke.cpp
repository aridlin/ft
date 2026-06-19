#define FT_IMPLEMENTATION
#include "../ft.hpp"

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "FT Combined Smoke";
    cfg.web_port = 8090;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    char name[64] = "operator";
    bool enabled = true;
    float level = 0.5f;
    const char* tabs[] = {"Controls", "Log"};
    int tab = 0;

    while (ft::pump()) {
        ft::begin();
        ft::tabs(tabs, 2, &tab);

        if (tab == 0) {
            ft::text("Combined FT backend");
            ft::input("Name", name, sizeof(name));
            ft::checkbox("Enabled", &enabled);
            ft::slider_float("Level", &level, 0.0f, 1.0f);
            ft::progress_bar(level, "Level");
            if (ft::button("Notify", ft::ColorRole::Accent)) {
                ft::toast_success("Notification from ft.hpp");
            }
        } else {
            ft::log_view("Log", "Smoke example started\nAll widgets compiled\n", 6);
        }

        ft::end();
    }

    ft::shutdown();
    return 0;
}
