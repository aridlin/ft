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
    cfg.title = "FTHT Form Login Demo";
    cfg.web_port = 8081;
    cfg.web_login_mode = ft::LoginMode::Form;
    cfg.web_login_username = "admin";
    cfg.web_login_password = "secret";
    cfg.web_login_db_path = "ftht_form_login_demo.litesql";
    cfg.web_login_session_seconds = 10 * 60;
    cfg.web_login_allow_registration = true;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    bool relay = false;
    float threshold = 42.0f;
    char operator_note[256] = "Try admin / secret, or request registration and approve it in this terminal.";
    char log[2048] =
        "[boot] form login enabled\n"
        "[auth] registration requests require terminal approval\n"
        "[auth] idle sessions expire after 10 minutes";

    while (ft::pump()) {
        ft::begin();
        ft::webserver_button();

        ft::text_wrapped(
            "This panel is protected by the built-in form login. The first run writes a LiteSQL "
            "text file containing username, salt, and salted password hash.");
        ft::separator();

        ft::row({1.0f, 1.0f}, [&]() {
            if (ft::checkbox("Relay armed", &relay)) {
                append_event(log, sizeof(log), relay ? "relay armed" : "relay disarmed");
            }
            if (ft::slider_float("Alert threshold", &threshold, 0.0f, 100.0f)) {
                append_event(log, sizeof(log), "threshold changed");
            }
        });

        ft::text_area_ex("Operator note", operator_note, sizeof(operator_note), 4,
                           ft::TextAreaFlags::WordWrap);

        ft::row(2, [&]() {
            if (ft::button("Record check", ft::ColorRole::Success)) {
                append_event(log, sizeof(log), "manual check recorded");
            }
            ft::html("<button type=\"button\" onclick=\"location.href='/ftht/logout'\">Logout</button>");
        });

        ft::log_view("Audit log", log, 10,
                       ft::LogViewFlags::WordWrap | ft::LogViewFlags::AutoScrollBottom);

        ft::end();
    }

    ft::shutdown();
    return 0;
}
