#define FT_IMPLEMENTATION
#include "../ft.hpp"

static bool g_parity_command_seen = false;

static bool parity_command_handler(const char* command) {
    g_parity_command_seen = command && std::strcmp(command, "parity") == 0;
    return g_parity_command_seen;
}

static int run_self_test() {
    {
        ftui::CommandHandler gui_handler = parity_command_handler;
        ftti::CommandHandler tui_handler = parity_command_handler;
        ftht::CommandHandler web_handler = parity_command_handler;
        ftui::set_command_handler(gui_handler);
        ftti::set_command_handler(tui_handler);
        ftht::set_command_handler(web_handler);

        ftti::detail::App& a = ftti::detail::app();
        a = ftti::detail::App{};
        a.style = ftti::default_dark_style();
        ftti::set_command_handler(parity_command_handler);
        g_parity_command_seen = false;
        ftti::detail::execute_command(":parity");
        if (!g_parity_command_seen || a.command_message != "parity") {
            std::fprintf(stderr, "self-test failed: ftti custom command handler did not run\n");
            return 1;
        }

        ftht::set_command_handler(parity_command_handler);
        g_parity_command_seen = false;
        if (!ftht::internal::execute_command(":parity") || !g_parity_command_seen) {
            std::fprintf(stderr, "self-test failed: ftht custom command handler did not run\n");
            return 1;
        }

        const char* web_items[] = {"One", "Two"};
        int web_selected = 0;
        ftht::internal::ctx().out.clear();
        ftht::set_next_color(ftht::ColorRole::Text, ftht::Color{1.0f, 0.0f, 0.0f, 1.0f});
        ftht::text("colored");
        ftht::push_color(ftht::ColorRole::Border, ftht::Color{0.0f, 1.0f, 0.0f, 1.0f});
        ftht::side_menu("Web menu", web_items, 2, &web_selected);
        ftht::side_menu_drawer("Web drawer", web_items, 2, &web_selected);
        ftht::split({1.0f, 2.0f}, []() { ftht::text("split"); });
        ftht::side_layout(180.0f, []() { ftht::text("layout"); });
        ftht::content([]() { ftht::text("content"); });
        ftht::pop_color();
        if (ftht::internal::ctx().out.find("color:#ff0000") == std::string::npos) {
            std::fprintf(stderr, "self-test failed: ftht set_next_color did not affect output\n");
            return 1;
        }
    }

    using namespace ftti::detail;

    {
        App& a = app();
        a = App{};
        a.style = ftti::default_dark_style();
        a.width = 40;
        a.height = 10;
        a.page_content_height = 40.0f;
        a.focused = 1002;
        a.focus_scroll_pending_id = 1002;

        bool focused = register_focus(1002, Rect{2, 20, 12, 3});
        if (!focused || a.page_scroll_target <= 0.0f || a.focus_scroll_pending_id != 1002) {
            std::fprintf(stderr, "self-test failed: page focus did not request scroll\n");
            return 1;
        }

        a.page_scroll_current = a.page_scroll_target;
        register_focus(1002, Rect{2, 6, 12, 3});
        if (a.focus_scroll_pending_id != 0) {
            std::fprintf(stderr, "self-test failed: page focus pending did not clear when visible\n");
            return 1;
        }
    }

    {
        App& a = app();
        a = App{};
        a.style = ftti::default_dark_style();
        a.width = 50;
        a.height = 16;
        a.page_content_height = 20.0f;
        ScrollState& ss = scroll_state(2001);
        ss.content = 30.0f;
        ss.current = 0.0f;
        ss.target = 0.0f;
        a.scroll_scopes.push_back(ScrollScope{2001, Rect{2, 4, 30, 5}});
        a.focused = 2002;
        a.focus_scroll_pending_id = 2002;

        bool focused = register_focus(2002, Rect{4, 12, 12, 2});
        if (!focused || ss.target <= 0.0f || a.page_scroll_target != 0.0f) {
            std::fprintf(stderr, "self-test failed: scoped focus did not request local scroll\n");
            return 1;
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--self-test") == 0) return run_self_test();
    }

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
