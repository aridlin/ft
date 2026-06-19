#define FT_IMPLEMENTATION
#include "../../ft.hpp"

#include <cstdio>
#include <cstring>

static void append_line(char* buffer, int cap, const char* line) {
    if (!buffer || cap <= 0 || !line) return;
    int len = (int)strlen(buffer);
    if (len > 0 && len < cap - 1) {
        buffer[len++] = '\n';
        buffer[len] = '\0';
    }
    if (len >= cap - 1) return;
    int room = cap - len - 1;
    int add = (int)strlen(line);
    if (add > room) add = room;
    if (add > 0) {
        memcpy(buffer + len, line, (size_t)add);
        buffer[len + add] = '\0';
    }
}

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "FTUI Control Panel";
    cfg.width = 1040;
    cfg.height = 860;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    char username[64] = "operator";
    char api_token[64] = "";
    char endpoint[128] = "https://deploy.internal.example";
    char notes[2048] =
        "Deploy staging after the nightly build.\n"
        "Double-check the migration window before applying changes.";
    char activity[4096] =
        "[10:12] Connected to controller.\n"
        "[10:13] Pulled latest deployment plan.\n"
        "[10:14] Waiting for operator input.";

    bool remember_session = true;
    bool dry_run = true;
    bool alerts_enabled = true;
    bool connection_open = true;
    bool automation_open = true;
    bool confirm_open_last = false;

    static const char* envs[] = { "Development", "Staging", "Production" };
    static const char* roles[] = { "Observer", "Operator", "Maintainer" };
    int env_sel = 1;
    int role_sel = 1;

    while (ft::pump()) {
        ft::begin();

        ft::text("Deployment control panel");
        ft::text_wrapped("A settings-heavy utility app can stay direct: a few collapsible sections, a confirmation modal, and read-only activity output without introducing controllers or retained screens.");
        ft::separator();

        ft::row({1.15f, 1.0f}, [&]() {
            ft::dropdown("Environment", envs, 3, &env_sel, 3);
            ft::radio_group("Operator role", roles, 3, &role_sel, 1);
        });

        if (ft::collapsing_header("Connection", &connection_open)) {
            ft::row({1.5f, 1.0f}, [&]() {
                ft::input("Username", username, sizeof(username));
                ft::input("API token", api_token, sizeof(api_token), ft::InputFlags::Password);
            });
            ft::input("Endpoint", endpoint, sizeof(endpoint));
            ft::checkbox("Remember session", &remember_session);
        }

        if (ft::collapsing_header("Automation", &automation_open)) {
            ft::checkbox("Dry run", &dry_run);
            ft::checkbox("Alert on completion", &alerts_enabled);
            ft::text_area_ex("Change summary", notes, sizeof(notes), 8, ft::TextAreaFlags::WordWrap);
        }

        ft::separator();
        ft::row(3, [&]() {
            if (ft::button("Validate")) {
                append_line(activity, sizeof(activity), "[Run] Validation completed successfully.");
            }
            if (ft::button("Apply")) {
                ft::open_modal("Confirm deployment");
            }
            if (ft::button("Clear summary")) {
                notes[0] = '\0';
                append_line(activity, sizeof(activity), "[Run] Summary cleared.");
            }
        });

        ft::separator();
        ft::log_view("Activity", activity, 10,
                       ft::LogViewFlags::WordWrap | ft::LogViewFlags::AutoScrollBottom);

        bool confirm_open = ft::modal("Confirm deployment", [&]() {
            ft::text_wrapped("Applying this plan will use the current environment, credentials, and notes exactly as shown in the form.");
            ft::separator();
            ft::row({1.0f, 1.0f}, [&]() {
                if (ft::button("Cancel")) ft::close_modal();
                if (ft::button("Deploy now")) {
                    char line[256];
                    snprintf(line, sizeof(line), "[Deploy] Sent %s run to %s as %s.",
                             dry_run ? "dry" : "live", envs[env_sel], username[0] ? username : "operator");
                    append_line(activity, sizeof(activity), line);
                    ft::close_modal();
                }
            });
        });

        if (confirm_open && !confirm_open_last) {
            append_line(activity, sizeof(activity), "[Run] Deployment confirmation opened.");
        }
        confirm_open_last = confirm_open;

        ft::end();
    }

    ft::shutdown();
    return 0;
}
