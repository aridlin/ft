#define FT_IMPLEMENTATION
#include "ft.hpp"

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "FT documentation controls";
    cfg.web_host = "127.0.0.1";
    if (!ft::create_window(cfg, argc, argv)) return 1;
    char username[64] = "operator";
    bool enabled = true;
    bool show_details = false;
    float volume = 0.5f;
    ft::TableRows people = {{"Alice", "Ready"}, {"Bob", "Waiting"}};
    ft::TableState table_state;
    ft::ClipboardViewState clipboard;
    while (ft::pump()) {
        ft::begin();
        ft::text("Controls and state");
        ft::row({2.0f, 1.0f}, [&] {
            ft::input("Username", username, sizeof(username), ft::InputFlags::CharsNoBlank);
            ft::checkbox("Enabled", &enabled);
        });
        ft::slider_float("Volume", &volume, 0.0f, 1.0f);
        ft::progress_bar(volume);
        if (ft::button("Save", ft::ColorRole::Accent)) ft::toast_success("Saved");
        ft::ContextMenuItem actions[] = {
            {"Show details", true, show_details, false, true}
        };
        if (ft::context_menu("Save actions", actions, 1) == 0) show_details = !show_details;
        if (show_details) ft::text("Application state survives between frames.");
        ft::table("People", {"Name", "Status"}, people, table_state);
        if (ft::button("Open dialog")) ft::open_modal("About");
        ft::modal("About", [&] {
            ft::text("This is a complete documentation example.");
            if (ft::button("Close")) ft::close_modal();
        });
        ft::clipboard_view("Clipboard", clipboard, 4);
        ft::end();
    }
    ft::shutdown();
}
