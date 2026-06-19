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

static void copy_text(char* buffer, int cap, const char* text) {
    if (!buffer || cap <= 0) return;
    if (!text) {
        buffer[0] = '\0';
        return;
    }
    snprintf(buffer, (size_t)cap, "%s", text);
}

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "FTUI Showcase";
    cfg.width = 1080;
    cfg.height = 920;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    char username[64] = "operator";
    char password[64] = "";
    char port[16] = "8080";
    char access_code[32] = "DEMO01";
    char editor[2048] =
        "This is an editable multiline text box.\n"
        "Turn on word wrap to keep long notes readable without extra work.\n"
        "It uses the same immediate-mode flow as the smaller widgets.";
    char logs[4096] =
        "[09:14] Boot sequence complete.\n"
        "[09:14] Waiting for input.\n"
        "[09:15] Renderer warmed up.\n"
        "[09:16] Ready.";

    int click_count = 0;
    bool safe_mode = false;
    bool advanced_enabled = true;
    float blend = 0.45f;
    float transfer_progress = 0.73f;
    float window_opacity = 0.92f;
    float dither_size = 4.0f;

    static const char* tabs[] = { "Workspace", "Logs", "Settings" };
    int tab_sel = 0;
    static const char* side_pages[] = { "Overview", "Toasts", "Progress" };
    int side_page = 0;

    static const char* env_items[] = { "Local", "Staging", "Production" };
    static const char* profile_items[] = { "Admin", "Observer", "Maintainer", "Support" };
    static const char* shell_items[] = { "Powershell", "Bash", "Cmd" };
    static const char* theme_names[] = {
        "Default Dark", "Catppuccin Mocha", "Nord", "Gruvbox Dark", "One Dark", "Ghostty Green"
    };
    static const char* theme_keys[] = {
        "default-dark", "catppuccin-mocha", "nord", "gruvbox-dark", "one-dark", "ghostty-green"
    };
    static const char* transparency_names[] = {
        "Opaque", "Plain transparency", "Soft dither", "Windows blur"
    };
    static const char* icon_names[] = {
        "Symbol", "Symbol + text"
    };
    static ft::Style (*theme_fns[])() = {
        ft::default_dark_style,
        ft::catppuccin_mocha_style,
        ft::nord_style,
        ft::gruvbox_dark_style,
        ft::one_dark_style,
        ft::ghostty_green_style,
    };

    int env_sel = 1;
    int profile_sel = 0;
    int shell_sel = 0;
    int theme_idx = 0;
    int icon_idx = 0;
    int transparency_idx = 0;
    for (int i = 0; i < 6; ++i) {
        if (strcmp(theme_keys[i], ft::current_style_name()) == 0) {
            theme_idx = i;
            break;
        }
    }

    static const char* embedded_battery_svg =
        "<svg width='160' height='64' viewBox='0 0 160 64' xmlns='http://www.w3.org/2000/svg'>"
        "<rect x='4' y='12' width='132' height='40' rx='6' ry='6' fill='white'/>"
        "<rect x='140' y='24' width='16' height='16' rx='3' ry='3' fill='white'/>"
        "</svg>";

    bool general_open = true;
    bool layout_open = true;
    bool color_open = true;
    bool modal_was_open = false;

    static const char* history_lines[] = {
        "Background sync completed with 12 updates.",
        "Parser cache rebuilt after settings change.",
        "New device attached on COM7.",
        "Live session exported to disk.",
        "Diagnostics snapshot created for support.",
        "Low-priority polling interval reduced.",
        "Message queue drained without errors.",
        "Operator role promoted to maintainer.",
        "Theme changed at runtime without restart.",
        "Scroll container preserved wheel ownership.",
        "Tooltip text now waits for a steady hover.",
        "Modal blocked background interaction correctly."
    };

    while (ft::pump()) {
        ft::begin();

        ft::text("FTUI showcase");
        ft::text_wrapped(
            "Tiny immediate-mode GUI for utility apps. This demo focuses on the higher-value widgets "
            "and layout helpers while keeping the basic flow the same: begin, draw widgets, end.");
        ft::separator();

        ft::side_menu_drawer("Feature menu", side_pages, 3, &side_page, 240.0f);
        if (side_page == 0) {
            ft::text("Side menu navigation");
            ft::text_wrapped("The drawer-style side menu is a denser navigation option for utility apps that have grown past a small tab strip.");
            ft::progress_bar(transfer_progress, "Inline status");
        } else if (side_page == 1) {
            ft::text("Toast notifications");
            ft::row(4, [&]() {
                if (ft::button("Info")) ft::toast_info("Background sync complete");
                if (ft::button("Saved")) ft::toast_success("Configuration saved");
                if (ft::button("Warn")) ft::toast_warning("High CPU usage");
                if (ft::button("Error")) ft::toast_error("Connection failed");
            });
        } else {
            ft::text("Masked progress");
            ft::ProgressStyle battery;
            battery.label = "Embedded SVG mask";
            battery.mask_svg = embedded_battery_svg;
            battery.height = 52.0f;
            battery.wave_front = true;
            battery.glint = true;
            ft::progress_bar(transfer_progress, battery);

            ft::ProgressStyle png_mask;
            png_mask.label = "PNG mask";
            png_mask.mask_path = "probar.png";
            png_mask.height = 52.0f;
            png_mask.wave_front = true;
            png_mask.glint = true;
            ft::progress_bar(transfer_progress, png_mask);

            ft::ProgressStyle pill;
            pill.label = "Built-in pill mask";
            pill.mask_shape = "pill";
            pill.height = 42.0f;
            pill.glint = true;
            ft::progress_bar(transfer_progress, pill);

            ft::slider_float("Progress", &transfer_progress, 0.0f, 1.0f);
        }

        ft::separator();

        ft::tabs(tabs, 3, &tab_sel);
        ft::separator();

        if (tab_sel == 0) {
            ft::text_wrapped("Core form widgets stay compact, but you can now layer in pickers, layout hints, tooltips, and disabled scopes without changing the basic pattern.");
            ft::separator();

            ft::row({2.0f, 1.0f}, [&]() {
                ft::input("Username", username, sizeof(username));
                ft::tooltip("Single-line input with the original simple call shape.");

                ft::input("Port", port, sizeof(port), ft::InputFlags::CharsDecimal);
                ft::tooltip("This field uses decimal-only filtering.");
            });

            ft::row({2.0f, 1.0f}, [&]() {
                ft::input("Password", password, sizeof(password), ft::InputFlags::Password);
                ft::tooltip("Password masking still uses the same input widget.");

                ft::input("Access code", access_code, sizeof(access_code),
                            ft::InputFlags::CharsUppercase | ft::InputFlags::CharsNoBlank);
                ft::tooltip("Uppercase plus no-blank filtering is additive.");
            });

            ft::row(2, [&]() {
                if (ft::button("Sign in")) {
                    click_count++;
                    append_line(logs, sizeof(logs), "[Workspace] Sign in requested.");
                }
                ft::tooltip("Buttons support hover, press, disabled, and keyboard activation.");

                if (ft::button("Focus username")) {
                    ft::request_focus("Username");
                }
            });

            ft::row({1.3f, 1.0f}, [&]() {
                ft::listbox("Profile", profile_items, 4, &profile_sel, 4);
                ft::tooltip("Listboxes are explicit always-visible pickers.");

                ft::radio_group("Shell", shell_items, 3, &shell_sel, 1);
                ft::tooltip("Radio groups use arrow-key navigation when focused.");
            });

            ft::separator();
            ft::checkbox("Enable safe mode", &safe_mode);
            ft::tooltip("Checkboxes now use a filled square instead of a checkmark.");
            ft::slider_float("Blend", &blend, 0.0f, 1.0f);
            ft::tooltip("Sliders also participate in the shared focus and disabled state path.");

            ft::begin_disabled();
            if (!advanced_enabled) {
                ft::button("Disabled action");
                ft::tooltip("Scoped disabled state dims visuals and suppresses interaction.");
            }
            ft::end_disabled();

            ft::row(2, [&]() {
                if (ft::button("Toggle advanced")) advanced_enabled = !advanced_enabled;
                if (ft::button("Open child window")) {
                    ft::Config child;
                    child.title = "FTUI Child";
                    child.width = 520;
                    child.height = 260;
                    child.center_window = true;
                    ft::open_child_window(child, [&]() {
                        ft::text("Child windows inherit the same API.");
                        ft::text_wrapped("This is still immediate mode. There is no extra controller layer for nested windows.");
                        ft::separator();
                        static bool child_flag = true;
                        static float child_mix = 0.35f;
                        ft::checkbox("Keep child updates live", &child_flag);
                        ft::slider_float("Mix", &child_mix, 0.0f, 1.0f);
                    });
                }
            });
        } else if (tab_sel == 1) {
            ft::row({3.0f, 2.0f}, [&]() {
                ft::text_area_ex("Editor##main", editor, sizeof(editor), 10, ft::TextAreaFlags::WordWrap);
                ft::tooltip("Editable multiline input with optional word wrap.");

                ft::log_view("Output##log", logs, 10, ft::LogViewFlags::WordWrap | ft::LogViewFlags::AutoScrollBottom);
                ft::tooltip("Read-only log view supports selection, copy, wrapping, and scrolling.");
            });

            ft::separator();
            if (ft::button("Append log line")) {
                append_line(logs, sizeof(logs), "[Logs] Manual line appended.");
            }
            ft::tooltip("Useful for transcripts, debug output, and runtime status.");

            ft::separator();
            ft::scroll_area("History", 220.0f, [&]() {
                for (int i = 0; i < (int)(sizeof(history_lines) / sizeof(history_lines[0])); ++i) {
                    char line[256];
                    snprintf(line, sizeof(line), "%02d  %s", i + 1, history_lines[i]);
                    ft::text(line);
                }
            });
            ft::tooltip("Nested scroll areas consume wheel input before the outer window.");
        } else {
            ft::text_wrapped("Collapsible sections, one-shot layout hints, modals, and theme switching are meant to stay opt-in. If you do not call them, the basic API does not get more complicated.");
            ft::separator();

            if (ft::collapsing_header("General##settings", &general_open)) {
                ft::checkbox("Enable advanced controls", &advanced_enabled);
                ft::text("Current environment");
                ft::text(env_items[env_sel]);
                ft::set_next_width(220.0f);
                ft::button("Fixed width example");
                ft::set_next_fill();
                ft::button("Fill width example");
            }

            if (ft::collapsing_header("Layout helpers##settings", &layout_open)) {
                ft::text("Percent width");
                ft::set_next_percent(0.55f);
                ft::button("55% width");
                ft::text("Clamped width");
                ft::set_next_percent(0.75f);
                ft::set_next_limits(180.0f, 280.0f);
                ft::set_next_align(ft::Align::End);
                ft::button("Aligned and clamped");
                ft::row({2.0f, 1.0f}, [&]() {
                    ft::button("Wide cell");
                    ft::button("Narrow cell");
                });
            }

            if (ft::collapsing_header("Color showcase##settings", &color_open)) {
                ft::text_wrapped(
                    "Theme presets now expose semantic accent, warning, and success colors. "
                    "You can also use one-shot or scoped hex overrides without changing the surrounding style.");

                ft::row(3, [&]() {
                    ft::button("Accent", ft::ColorRole::Accent);
                    ft::button("Warning", ft::ColorRole::Warning);
                    ft::button("Success", ft::ColorRole::Success);
                });

                ft::row(2, [&]() {
                    ft::set_next_color(ft::ColorRole::Button, ft::color_from_hex("#c93c3c"));
                    ft::set_next_color(ft::ColorRole::Text, ft::color_from_hex("#fff5f5"));
                    ft::button("One-shot hex");

                    ft::button("Direct tint", ft::color_from_hex("#7c3aed"));
                });

                ft::push_color(ft::ColorRole::Button, ft::color_from_hex("#2f855a"));
                ft::push_color(ft::ColorRole::Text, ft::color_from_hex("#f7fff9"));
                ft::row(2, [&]() {
                    ft::button("Scoped green A");
                    ft::button("Scoped green B");
                });
                ft::pop_color();
                ft::pop_color();
            }

            ft::separator();
            if (ft::button("Reset session")) {
                ft::open_modal("Reset confirmation");
            }
        }

        ft::separator();
        ft::row(3, [&]() {
            if (ft::button("Toggle rects")) ft::debug().show_layout_rects ^= true;
            if (ft::button("Toggle IDs")) {
                ft::debug().show_hovered_id ^= true;
                ft::debug().show_active_id ^= true;
            }
            if (ft::button("Toggle FPS")) ft::debug().show_fps ^= true;
        });

        ft::separator();
        char summary[256];
        snprintf(summary, sizeof(summary),
                 "Clicks: %d   safe mode: %s   blend: %.2f   shell: %s",
                 click_count, safe_mode ? "on" : "off", blend, shell_items[shell_sel]);
        ft::text(summary);

        for (int i = 0; i < 6; ++i) {
            if (strcmp(theme_keys[i], ft::current_style_name()) == 0) {
                theme_idx = i;
                break;
            }
        }
        if (ft::dropdown("Theme", theme_names, 6, &theme_idx, 6)) {
            ft::set_style(theme_fns[theme_idx]());
            append_line(logs, sizeof(logs), "[Settings] Theme changed.");
        }
        ft::tooltip("Dropdowns are opt-in single-call pickers.");

        if (ft::dropdown("Window transparency", transparency_names, 4, &transparency_idx, 4)) {
            ft::WindowTransparency mode = ft::WindowTransparency::Opaque;
            if (transparency_idx == 1) mode = ft::WindowTransparency::Plain;
            else if (transparency_idx == 2) mode = ft::WindowTransparency::BayerDither;
            else if (transparency_idx == 3) mode = ft::WindowTransparency::Blur;
            ft::set_window_transparency(mode);
            append_line(logs, sizeof(logs), "[Settings] Window transparency changed.");
        }
        ft::row(2, [&]() {
            if (ft::slider_float("Window opacity", &window_opacity, 0.25f, 1.0f)) {
                ft::set_window_opacity(window_opacity);
            }
            if (ft::slider_float("Dither motif size", &dither_size, 1.0f, 16.0f)) {
                ft::set_dither_size((int)(dither_size + 0.5f));
            }
        });

        if (ft::dropdown("Window icon", icon_names, 2, &icon_idx, 2)) {
            ft::set_window_icon_builtin(icon_idx == 0
                ? ft::BuiltinIcon::Symbol
                : ft::BuiltinIcon::SymbolWithText);
            append_line(logs, sizeof(logs), "[Settings] Window icon changed.");
        }

        bool modal_open = ft::modal("Reset confirmation", [&]() {
            ft::text_wrapped("This lightweight modal blocks background widgets until you close it. It is still immediate mode: just call ft::modal() and draw inside it.");
            ft::separator();
            ft::row({1.0f, 1.0f}, [&]() {
                if (ft::button("Cancel")) ft::close_modal();
                if (ft::button("Reset now")) {
                    username[0] = '\0';
                    password[0] = '\0';
                    copy_text(port, sizeof(port), "8080");
                    copy_text(access_code, sizeof(access_code), "DEMO01");
                    copy_text(editor, sizeof(editor), "Session reset.\nYou can keep typing here.");
                    copy_text(logs, sizeof(logs), "[Reset] Session cleared.");
                    click_count = 0;
                    safe_mode = false;
                    blend = 0.45f;
                    ft::close_modal();
                }
            });
        });
        if (modal_open && !modal_was_open) {
            append_line(logs, sizeof(logs), "[Settings] Reset modal opened.");
        }
        modal_was_open = modal_open;

        ft::end();
    }

    ft::shutdown();
    return 0;
}
