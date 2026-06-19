#define FT_IMPLEMENTATION
#include "../../ft.hpp"

#include <cstdio>
#include <cstring>
#include <ctime>

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

static void append_event(char* log, int cap, const char* message) {
    char line[160];
    std::time_t now = std::time(nullptr);
#if defined(_WIN32)
    std::tm tm_buf{};
    localtime_s(&tm_buf, &now);
    std::tm* tm = &tm_buf;
#else
    std::tm* tm = std::localtime(&now);
#endif
    std::snprintf(line, sizeof(line), "[%02d:%02d:%02d] %s",
                  tm ? tm->tm_hour : 0, tm ? tm->tm_min : 0, tm ? tm->tm_sec : 0,
                  message ? message : "");
    append_line(log, cap, line);
}

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "FTHT Draw Login Demo";
    cfg.web_port = 8082;
    cfg.web_login_mode = ft::LoginMode::DrawPenis;
    cfg.web_login_session_seconds = 10 * 60;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    bool sprinkler = false;
    bool lights = true;
    float silliness = 69.0f;
    char log[2048] =
        "[boot] drawing challenge enabled\n"
        "[auth] draw two nearby ellipses and one long ellipse or rectangle";

    while (ft::pump()) {
        ft::begin();
        ft::webserver_button();

        ft::text_wrapped(
            "This panel is protected by the drawing challenge. It unlocks when the sketch contains "
            "two nearby ellipses and one long ellipse or rectangle.");
        ft::separator();

        ft::row(3, [&]() {
            if (ft::checkbox("Sprinkler", &sprinkler)) {
                append_event(log, sizeof(log), sprinkler ? "sprinkler enabled" : "sprinkler disabled");
            }
            if (ft::checkbox("Lights", &lights)) {
                append_event(log, sizeof(log), lights ? "lights enabled" : "lights disabled");
            }
            if (ft::slider_float("Silliness", &silliness, 0.0f, 100.0f)) {
                append_event(log, sizeof(log), "silliness adjusted");
            }
        });

        ft::row(2, [&]() {
            if (ft::button("Log victory", ft::ColorRole::Accent)) {
                append_event(log, sizeof(log), "challenge accepted by the operator");
            }
            ft::html("<button type=\"button\" onclick=\"location.href='/ftht/logout'\">Logout</button>");
        });

        ft::log_view("Event log", log, 10,
                       ft::LogViewFlags::WordWrap | ft::LogViewFlags::AutoScrollBottom);

        ft::end();
    }

    ft::shutdown();
    return 0;
}
