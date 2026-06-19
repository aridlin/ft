#define FT_IMPLEMENTATION
#include "../../ft.hpp"

#include <cstdio>
#include <cstring>

static void append_line(char* buffer, int cap, const char* line) {
    if (!buffer || cap <= 0 || !line) return;
    int len = (int)std::strlen(buffer);
    if (len > 0 && len < cap - 1) {
        buffer[len++] = '\n';
        buffer[len] = '\0';
    }
    if (len >= cap - 1) return;
    int room = cap - len - 1;
    int add = (int)std::strlen(line);
    if (add > room) add = room;
    if (add > 0) {
        std::memcpy(buffer + len, line, (size_t)add);
        buffer[len + add] = '\0';
    }
}

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "FTHT Showcase";
    cfg.web_port = 8080;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    char username[64] = "operator";
    char port[16] = "8080";
    char notes[1024] =
        "This page is generated from immediate-mode C++ calls.\n"
        "Change a field and the browser posts back to the tiny embedded server.";
    char logs[2048] =
        "[boot] server started\n"
        "[boot] waiting for browser requests";

    bool safe_mode = false;
    float blend = 0.45f;
    int env_sel = 1;
    int tab_sel = 0;

    static const char* tabs[] = { "Controls", "Logs" };
    static const char* envs[] = { "Local", "Staging", "Production" };

    while (ft::pump()) {
        ft::begin();
        ft::webserver_button();

        ft::text_wrapped(
            "FTHT keeps a tiny immediate-mode loop, but renders HTML and serves it over HTTP. "
            "It is intended for small control panels on PCs, ESP32 boards, and similar devices.");
        ft::separator();

        ft::tabs(tabs, 2, &tab_sel);
        ft::separator();

        if (tab_sel == 0) {
            ft::row({2.0f, 1.0f}, [&]() {
                if (ft::input("Username", username, sizeof(username))) {
                    append_line(logs, sizeof(logs), "[form] username changed");
                }
                ft::input("Port", port, sizeof(port), ft::InputFlags::CharsDecimal);
            });

            ft::row({1.0f, 1.0f, 1.0f}, [&]() {
                ft::dropdown("Environment", envs, 3, &env_sel, 3);
                ft::checkbox("Safe mode", &safe_mode);
                ft::slider_float("Blend", &blend, 0.0f, 1.0f);
            });

            ft::text_area_ex("Notes", notes, sizeof(notes), 7, ft::TextAreaFlags::WordWrap);

            ft::row(3, [&]() {
                if (ft::button("Ping", ft::ColorRole::Accent)) {
                    append_line(logs, sizeof(logs), "[action] ping clicked");
                    ft::toast_success("Ping sent");
                }
                if (ft::button("Warn", ft::ColorRole::Warning)) {
                    append_line(logs, sizeof(logs), "[action] warning clicked");
                    ft::toast_warning("Warning toast queued");
                }
                if (ft::button("Open modal")) {
                    ft::open_modal("Confirm action");
                    ft::toast("Modal opened");
                }
            });
        } else {
            ft::log_view("Output", logs, 12,
                           ft::LogViewFlags::WordWrap | ft::LogViewFlags::AutoScrollBottom);
            if (ft::button("Append sample")) {
                append_line(logs, sizeof(logs), "[log] sample line appended");
            }
        }

        if (ft::modal("Confirm action", [&]() {
                ft::text_wrapped("This modal is still just immediate-mode C++ rendered into HTML.");
                ft::row(2, [&]() {
                    if (ft::button("Cancel")) ft::close_modal();
                    if (ft::button("Confirm", ft::ColorRole::Success)) {
                        append_line(logs, sizeof(logs), "[modal] action confirmed");
                        ft::toast_success("Action confirmed");
                        ft::close_modal();
                    }
                });
            })) {
        }

        ft::end();
    }

    ft::shutdown();
    return 0;
}
