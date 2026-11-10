# FT Combined

`ft.hpp` is the combined FT family header. It keeps the FTUI immediate-mode
syntax as the primary API and routes the same app code to three backends:

| Backend | Namespace | Use case |
| --- | --- | --- |
| Native GUI | `ftui::` | Desktop windows, normal local apps, child windows, images, file dialogs |
| Terminal TUI | `ftti::` | SSH, Linux tty, terminal dashboards, no-display machines |
| Embedded web | `ftht::` | Browser panels, remote control pages, tiny local HTTP tools |
| Combined facade | `ft::` | One syntax that launches GUI, TUI, or web at runtime |

The intended application API is `ft::`. The original backend namespaces are
still included as escape hatches when a program deliberately wants one backend.

## Goals

- One header: copy `ft.hpp` into a C++17 project.
- One immediate-mode syntax: write normal FTUI-style code under `ft::`.
- Three launch modes: GUI, TUI, or web, selected automatically or by flag.
- A GUI-to-web handoff: start an embedded web server from a running desktop app.
- Backend compatibility: FTHT and FTTI features are mapped into the combined API
  where they make sense, while backend-only helpers remain available.

## Quick Start

Define `FT_IMPLEMENTATION` in exactly one translation unit:

```cpp
#define FT_IMPLEMENTATION
#include "ft.hpp"

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "Combined FT App";
    cfg.mode = ft::Mode::Auto;
    cfg.width = 900;
    cfg.height = 560;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    char name[64] = "operator";
    bool enabled = true;
    float level = 0.5f;

    while (ft::pump()) {
        ft::begin();

        ft::webserver_button();

        ft::text("Hello from ft.hpp");
        ft::input("Name", name, sizeof(name));
        ft::checkbox("Enabled", &enabled);
        ft::slider_float("Level", &level, 0.0f, 1.0f);

        if (ft::button("Run", ft::ColorRole::Accent)) {
            ft::toast_success("Run clicked");
        }

        ft::end();
    }

    ft::shutdown();
    return 0;
}
```

Compile on Linux:

```bash
mkdir -p build
c++ examples/ft_combined_smoke.cpp -o build/ft_combined_smoke -std=c++17 $(pkg-config --cflags --libs cairo x11) -pthread
```

Compile on Windows with MSVC:

```bat
cl /std:c++17 examples\ft_combined_smoke.cpp d2d1.lib dwrite.lib gdi32.lib ole32.lib uuid.lib user32.lib windowscodecs.lib ws2_32.lib
```

Build every bundled demo:

```bash
sh scripts/build_demos.sh
```

## Launch Modes

`ft::Config::mode` controls the preferred backend:

```cpp
cfg.mode = ft::Mode::Auto;
cfg.mode = ft::Mode::Gui;
cfg.mode = ft::Mode::Tui;
cfg.mode = ft::Mode::Web;
```

`ft::Mode::Auto` chooses at launch:

| Environment | Auto result |
| --- | --- |
| Desktop display available | GUI |
| No display, real interactive terminal available | TUI |
| No display and no usable terminal | Web |

Auto mode may fall back if the first backend cannot open. Forced modes do not
fall back, because an explicit `--tui` or `--web` should mean exactly that mode.

### Command-Line Flags

The combined launcher accepts these flags:

| Flag | Result |
| --- | --- |
| `--ft-auto` | Force auto detection |
| `--ft-gui` or `--gui` | Force native GUI |
| `--ft-tui` or `--tui` | Force terminal TUI |
| `--ft-web` or `--web` | Force embedded web server |
| `--ft-mode=auto` | Force auto detection |
| `--ft-mode=gui` | Force native GUI |
| `--ft-mode=tui` | Force terminal TUI |
| `--ft-mode=web` | Force embedded web server |

The app must pass `argc, argv` into `ft::create_window()` for flags to work:

```cpp
int main(int argc, char** argv) {
    ft::Config cfg;
    return ft::create_window(cfg, argc, argv) ? 0 : 1;
}
```

This overload is equivalent:

```cpp
ft::create_window(argc, argv, cfg);
```

If an app calls `ftui::create_window()` directly, it is intentionally a
native-only FTUI program and `--tui` will not affect it. Use the combined `ft::`
launcher when runtime backend selection is required. The bundled demos use
`ft::` so the mode flags apply to them.

### `--tui` Notes

`--tui` requires a real interactive terminal. It works from a terminal or PTY:

```bash
build/ft_combined_smoke --tui
```

It can fail or appear to do nothing when launched from a file explorer, desktop
launcher, or process manager that does not attach stdin/stdout to a terminal.
For file-explorer launches on a desktop, use auto mode or `--gui`. For browser
mode, use `--web`.

## App Lifecycle

Every combined FT app follows the same loop:

```cpp
if (!ft::create_window(cfg, argc, argv)) return 1;

while (ft::pump()) {
    ft::begin();
    // widgets here
    ft::end();
}

ft::shutdown();
```

Functions:

| Function | Purpose |
| --- | --- |
| `ft::create_window(cfg, argc, argv)` | Opens GUI/TUI or starts the web server |
| `ft::pump()` | Processes events and returns `false` when the app should quit |
| `ft::begin()` | Starts an immediate-mode frame |
| `ft::end()` | Finishes and presents the frame |
| `ft::shutdown()` | Releases backend state |
| `ft::active_mode()` | Returns the current `ft::Mode` |
| `ft::config()` | Returns the active resolved config |
| `ft::request_redraw()` | Asks the backend to redraw |

## Vim-Style Commands

GUI, TUI, and web builds support a small Vim-style command prompt. Press `:`,
type a command, press `Tab` to accept the gray autocomplete preview, press
`Enter` to run, or press `Esc` to cancel.

Common commands:

| Command | Meaning |
| --- | --- |
| `:q`, `:quit`, `:exit` | Quit where the backend can quit directly; web explains that the tab/server must be closed |
| `:help`, `:?` | Show command help |
| `:tab next`, `:tn` | Move to the next tab where the backend can resolve tabs |
| `:tab prev`, `:tp` | Move to the previous tab where the backend can resolve tabs |
| `:theme dark`, `:td` | Switch to a dark theme |
| `:theme light` | Switch to a light/default web theme where supported |
| `:theme green` | TUI green theme |
| `:theme amber` | TUI amber theme |
| `:rr` | Request refresh/redraw |

The TUI command line renders the typed prefix in the accent color and the
completion suffix in gray. The web command palette uses the same interaction in
the browser and stays out of the way while typing inside inputs, text areas, and
select controls.

## Configuration

`ft::Config` contains the shared window settings plus backend-specific options:

```cpp
ft::Config cfg;
cfg.mode = ft::Mode::Auto;
cfg.title = "FT App";
cfg.width = 960;
cfg.height = 640;
cfg.fps_limit = 60;
cfg.resizable = true;
cfg.center_window = true;
```

### Common Fields

| Field | Default | Meaning |
| --- | --- | --- |
| `mode` | `ft::Mode::Auto` | Backend selection |
| `title` | `"FT App"` | Window title or web page title |
| `width`, `height` | `960`, `640` | Native window size |
| `fps_limit` | `60` | Frame cap; `0` or lower disables limiting in GUI |
| `resizable` | `true` | Native resize behavior |
| `center_window` | `true` | Center the native window on create |
| `icon` | `nullptr` | Native icon handle where supported |
| `enable_effects` | `true` | Smooth scrolling and animations |
| `backdrop_effect` | `ft::BackdropEffect::Blur` | GUI/terminal visual effect preference |
| `dither_size` | `4` | Dither motif size |
| `window_transparency` | `ft::WindowTransparency::Opaque` | GUI transparency mode |
| `window_opacity` | `0.92f` | GUI opacity for transparent modes |

### TUI Fields

| Field | Default | Meaning |
| --- | --- | --- |
| `tui_alternate_screen` | `true` | Use the terminal alternate screen |
| `tui_enable_mouse` | `true` | Enable terminal mouse input |
| `tui_quit_on_ctrl_q` | `true` | Let Ctrl-Q close the app |
| `tui_animate` | `true` | Enable terminal animation/redraw behavior |

### Web Fields

| Field | Default | Meaning |
| --- | --- | --- |
| `web_host` | `"0.0.0.0"` | HTTP bind address |
| `web_port` | `8080` | HTTP port |
| `web_max_request_bytes` | `32 * 1024` | Request size limit |
| `web_read_timeout_ms` | `1500` | Socket read timeout |
| `web_client_poll_ms` | `0` | Browser polling interval, `0` disables polling |
| `web_auto_submit` | `true` | Submit changed controls automatically |
| `web_print_url` | `true` | Print the URL when the server starts |
| `web_debug_output` | `false` | Server debug logs |
| `web_client_debug_output` | `false` | Browser debug logs |
| `web_dark_mode` | `false` | Force dark web theme |
| `web_respect_browser_dark_mode` | `false` | Follow browser color preference |
| `web_extra_head_html` | `nullptr` | Extra HTML injected into `<head>` |
| `web_login_mode` | `ft::LoginMode::None` | Optional login gate |
| `web_login_username` | `"admin"` | Form login username |
| `web_login_password` | `nullptr` | Plain password, if used |
| `web_login_salt` | `"ftht-login"` | Hash salt |
| `web_login_db_path` | `nullptr` | Registration/session database path |
| `web_login_password_hash` | `0` | Precomputed login password hash |
| `web_login_session_seconds` | `600` | Login session duration |
| `web_login_allow_registration` | `false` | Allow browser-side user registration |

Password hash helper:

```cpp
cfg.web_login_password_hash =
    ft::make_login_password_hash("secret", cfg.web_login_salt);
```

## Styling

The combined `ft::Style` mirrors the FTUI style shape and is translated to TUI
and web colors when those backends are active.

```cpp
ft::Style style = ft::one_dark_style();
style.accent = ft::color_from_hex("#65d1ff");
style.rounding = 6.0f;
ft::set_style(style);
```

Built-in style helpers:

```cpp
ft::default_dark_style();
ft::catppuccin_mocha_style();
ft::nord_style();
ft::gruvbox_dark_style();
ft::one_dark_style();
ft::ghostty_green_style();
ft::style_from_name("one-dark");
ft::style_name(ft::get_style());
ft::current_style_name();
```

Color helpers:

```cpp
ft::Color c = ft::color_from_hex("#7aa2f7");
ft::push_color(ft::ColorRole::Accent, c);
ft::button("Temporary accent");
ft::pop_color();

ft::set_next_color(ft::ColorRole::Button, ft::color_from_hex("#1f2937"));
ft::button("One-off color");
```

Color roles:

```cpp
ft::ColorRole::Background
ft::ColorRole::Panel
ft::ColorRole::Text
ft::ColorRole::TextDim
ft::ColorRole::Border
ft::ColorRole::Button
ft::ColorRole::ButtonHover
ft::ColorRole::ButtonActive
ft::ColorRole::InputBg
ft::ColorRole::InputFocus
ft::ColorRole::Accent
ft::ColorRole::Warning
ft::ColorRole::Success
```

## Widget Syntax

Most widgets return `true` when their value changed or their action fired.

### Text

```cpp
ft::text("Single line");
ft::text_wrapped("Wrapped paragraph text.");
ft::separator();
ft::spacing();
ft::spacing(16.0f);
```

### Inputs

```cpp
char username[64] = "";
char password[64] = "";
bool submitted = false;

ft::input("Username", username, sizeof(username));
ft::input("Password", password, sizeof(password), ft::InputFlags::Password);
ft::input("Hex", username, sizeof(username),
          ft::InputFlags::CharsHexadecimal | ft::InputFlags::CharsUppercase,
          &submitted);
```

Input flags:

```cpp
ft::InputFlags::Default
ft::InputFlags::Password
ft::InputFlags::ReadOnly
ft::InputFlags::CharsDecimal
ft::InputFlags::CharsHexadecimal
ft::InputFlags::CharsUppercase
ft::InputFlags::CharsNoBlank
```

### Text Areas And Logs

```cpp
char notes[2048] = "";

ft::text_area("Notes", notes, sizeof(notes), 6);
ft::text_area_ex("Read-only notes", notes, sizeof(notes), 6,
                 ft::TextAreaFlags::ReadOnly | ft::TextAreaFlags::WordWrap);

ft::log_view("Events", "boot\nready\n", 8,
             ft::LogViewFlags::WordWrap | ft::LogViewFlags::AutoScrollBottom);
```

Text area flags:

```cpp
ft::TextAreaFlags::Default
ft::TextAreaFlags::ReadOnly
ft::TextAreaFlags::WordWrap
ft::TextAreaFlags::AutoScrollBottom
```

Log view flags:

```cpp
ft::LogViewFlags::Default
ft::LogViewFlags::WordWrap
ft::LogViewFlags::AutoScrollBottom
```

### Controls

```cpp
bool enabled = true;
float gain = 0.25f;
int selected = 0;
const char* modes[] = {"Auto", "Manual", "Locked"};

ft::checkbox("Enabled", &enabled);
ft::slider_float("Gain", &gain, 0.0f, 1.0f);

if (ft::button("Apply")) {}
if (ft::button("Delete", ft::ColorRole::Warning)) {}
if (ft::button("Custom", ft::color_from_hex("#8bd5ca"))) {}

ft::dropdown("Mode", modes, 3, &selected);
ft::listbox("Mode list", modes, 3, &selected, 5);
ft::radio_group("Mode radio", modes, 3, &selected, 1);
```

### Navigation

```cpp
int tab = 0;
const char* tabs[] = {"Status", "Controls", "Logs"};

if (ft::tabs(tabs, 3, &tab)) {
    ft::toast("Tab changed");
}

if (ft::collapsing_header("Advanced")) {
    ft::text("Advanced settings");
}

int page = 0;
const char* pages[] = {"Overview", "Telemetry", "Settings"};
ft::side_menu("Pages", pages, 3, &page);
ft::side_menu_drawer("Pages", pages, 3, &page, 260.0f);
```

`side_menu_drawer()` is native-first. In TUI and web it falls back to the normal
side menu behavior.

### Layout

```cpp
ft::row(2, [&] {
    ft::text("Left");
    ft::text("Right");
});

ft::row({2.0f, 1.0f}, [&] {
    ft::text("Wide");
    ft::text("Narrow");
});

ft::split({220.0f, 1.0f}, [&] {
    ft::text("Sidebar");
    ft::text("Main");
});

ft::side_layout(240.0f, [&] {
    ft::side_menu("Nav", pages, 3, &page);
    ft::content([&] {
        ft::text("Page body");
    });
});

ft::scroll_area("History", 260.0f, [&] {
    ft::log_view("Log", "line 1\nline 2\n", 10);
});
```

`split()`, `side_layout()`, and `content()` are strongest in GUI/TUI. In web
mode they degrade to simple responsive layout or direct content calls.

### Next-Widget Layout Hints

These apply to the next widget only:

```cpp
ft::set_next_width(220.0f);
ft::input("Fixed width", buffer, sizeof(buffer));

ft::set_next_fill();
ft::button("Fill");

ft::set_next_percent(0.5f);
ft::button("Half width");

ft::set_next_limits(120.0f, 360.0f);
ft::input("Limited", buffer, sizeof(buffer));

ft::set_next_align(ft::Align::End);
ft::button("Right aligned");
```

Align values:

```cpp
ft::Align::Start
ft::Align::Center
ft::Align::End
```

### Modals, Disabled Blocks, Focus, Tooltips

```cpp
if (ft::button("Open modal")) {
    ft::open_modal("Confirm");
}

ft::modal("Confirm", [&] {
    ft::text("Continue?");
    if (ft::button("Close")) ft::close_modal();
});

ft::begin_disabled();
ft::button("Disabled");
ft::end_disabled();

ft::button("Hover me");
ft::tooltip("Helpful detail");

ft::request_focus("Username");
```

### Toasts

```cpp
ft::toast("Plain info");
ft::toast_info("Info");
ft::toast_success("Saved");
ft::toast_warning("Check settings");
ft::toast_error("Failed");

ft::Toast toast;
toast.message = "Custom duration";
toast.type = ft::ToastType::Success;
toast.duration_ms = 6000;
toast.dismissible = true;
ft::toast(toast);

ft::clear_toasts();
```

Toast types:

```cpp
ft::ToastType::Info
ft::ToastType::Success
ft::ToastType::Warning
ft::ToastType::Error
```

### Progress

```cpp
ft::progress_bar(0.42f);
ft::progress_bar(0.42f, "Upload");

ft::ProgressStyle ps;
ps.label = "Deploy";
ps.fill_role = ft::ColorRole::Success;
ps.show_percent = true;
ps.wave_front = true;
ps.glint = true;
ft::progress_bar(0.75f, ps);
```

`ProgressStyle::mask_path`, `mask_svg`, and `mask_shape` are mainly native GUI
features. TUI and web use practical fallbacks.

### Images And File Dialogs

```cpp
ft::ImageHandle* img = ft::load_image("docs/screenshots/ftui/showcase-overview.png");
ft::image(img, 320.0f, 180.0f);
ft::free_image(img);

ft::FileFilter filters[] = {
    {"Images", "*.png;*.jpg;*.jpeg"},
    {"All Files", "*.*"},
};
std::string path = ft::open_file_dialog("Open image", filters, 2);
```

Images are native/TUI features. Web mode ignores `ft::image()`; use `ft::html()`
for explicit browser markup if needed.

### Child Windows And Debug

```cpp
ft::Config child;
child.title = "Tools";
child.width = 420;
child.height = 320;

ft::open_child_window(child, [&] {
    ft::text("Child window body");
});

ft::debug().show_fps = true;
ft::debug().show_layout_rects = true;
```

Child windows are native GUI features. Other modes run the callback inline.

## Web Mode Syntax

When the active render target is web, these helpers expose request details:

```cpp
ft::html("<strong>Raw browser markup</strong>");

const char* app_url = ft::url();
const char* request_path = ft::path();
const char* request_method = ft::method();
bool posted = ft::is_post();
const char* value = ft::param("name");

ft::set_status(404, "Not found");
```

Outside web mode, web helpers are harmless no-ops or empty values.

The web backend also includes the same `:` command palette. It can run browser
commands like `:tab next` locally when possible, and posts `_ftht_command` to
the server for commands such as `:help`, `:theme dark`, and `:rr`.

### Login Modes

```cpp
cfg.web_login_mode = ft::LoginMode::None;
cfg.web_login_mode = ft::LoginMode::Form;
cfg.web_login_mode = ft::LoginMode::DrawPenis;

cfg.web_login_username = "admin";
cfg.web_login_password_hash = ft::make_login_password_hash("secret");
```

`Form` is the normal username/password gate. `DrawPenis` preserves the FTHT
draw-login demo behavior for compatibility with the original family library.

## GUI To Web Handoff

An app launched through `ft::create_window()` can expose the same immediate-mode
UI over HTTP while keeping a tiny local control window.

```cpp
ft::webserver_button();
```

The default button is a compact top-right square with a circle label:

```cpp
ft::webserver_button("O");
```

When clicked in GUI mode:

1. FT starts an embedded FTHT server using the active `web_*` config fields.
2. FT opens `ft::url()` in the default browser.
3. The native window shrinks to a tiny stop-control rectangle.
4. Normal native widgets disappear while the server is active.
5. Clicking the stop control shuts down the server and restores the window size.

The same server control is also available programmatically:

```cpp
ft::start_webserver();
ft::webserver_running();
ft::stop_webserver();
```

If a developer does not include `ft::webserver_button()`, the built-in GUI
command line can do the same thing. Press `:` and run one of:

```text
:webserver
:web
:server
:ws
```

The command toggles the server on or off.

## Backend Escape Hatches

Prefer `ft::` for portable apps. Use backend namespaces when you specifically
want a single backend or a feature that cannot be meaningfully mapped:

```cpp
ftui::create_window();   // native GUI only
ftti::create_window();   // terminal TUI only
ftht::create_server();   // HTTP server only
```

Rules of thumb:

- Use `ft::` for demos and apps that should honor `--gui`, `--tui`, and `--web`.
- Use `ftui::` only when you deliberately want to lock a program to native GUI.
- Use `ftti::` for terminal-specific experiments.
- Use `ftht::` for request/server internals that should not pretend to be GUI.

## Bundled Demos

| Source | Built binary | API used | Notes |
| --- | --- | --- | --- |
| `examples/ft_combined_smoke.cpp` | `build/ft_combined_smoke` | `ft::` | Small combined smoke app |
| `examples/ftht/http_showcase.cpp` | `build/ftht/http_showcase` | `ft::` | FTHT demo migrated to combined launcher |
| `examples/ftht/device_panel.cpp` | `build/ftht/device_panel` | `ft::` | Device panel, web handoff button |
| `examples/ftht/login_form_demo.cpp` | `build/ftht/login_form_demo` | `ft::` | Form login demo through FT |
| `examples/ftht/login_draw_demo.cpp` | `build/ftht/login_draw_demo` | `ft::` | Draw-login demo through FT |
| `examples/ftht/compile_overwatch.cpp` | `build/ftht/compile_overwatch` | `ft::` | Compile/process dashboard |
| `examples/ftui/showcase.cpp` | `build/ftui/showcase` | `ft::` | FTUI showcase through combined launcher |
| `examples/ftui/control_panel.cpp` | `build/ftui/control_panel` | `ft::` | Controls through combined launcher |
| `examples/ftui/log_viewer.cpp` | `build/ftui/log_viewer` | `ft::` | Log viewer through combined launcher |
| `examples/ftui/benchmark.cpp` | `build/ftui/benchmark` | `ft::` | Benchmark through combined launcher |
| `examples/ftui/navigation_status.cpp` | `build/ftui/navigation_status` | `ft::` | Navigation/status demo through combined launcher |

Run examples:

```bash
build/ft_combined_smoke
build/ft_combined_smoke --tui
build/ft_combined_smoke --web

build/ftht/device_panel --gui
build/ftht/device_panel --web
build/ftht/device_panel --tui
```

The FTHT demos are written against `ft::`, not direct `ftht::`, so opening the
built executables normally on a desktop starts the native FTUI GUI. Use `--web`
or `--ft-web` when you want the same demo to launch directly as an embedded web
panel.

The copied FTUI demos are also written against `ft::` now. They still showcase
the FTUI syntax and widgets, but `--tui` and `--web` are available for the same
binaries.

## Screenshots

### Native GUI

| Showcase overview | Drawer and progress masks |
| --- | --- |
| ![FTUI showcase overview](docs/screenshots/ftui/showcase-overview.png) | ![FTUI drawer and progress masks](docs/screenshots/ftui/showcase-drawer-progress.png) |

| Controls and colors | Ghostty-style theme and toasts |
| --- | --- |
| ![FTUI controls and color showcase](docs/screenshots/ftui/showcase-controls.png) | ![FTUI Ghostty-style theme and toasts](docs/screenshots/ftui/showcase-ghostty-toasts.png) |

### Terminal TUI

| Dashboard | Inputs | Themes |
| --- | --- | --- |
| ![FTTI dashboard tab](docs/screenshots/ftti/showcase-dashboard.png) | ![FTTI inputs tab](docs/screenshots/ftti/showcase-inputs.png) | ![FTTI themes tab](docs/screenshots/ftti/showcase-themes.png) |

### Embedded Web

| Status | Controls |
| --- | --- |
| ![FTHT device panel status](docs/screenshots/ftht/device-panel-status.png) | ![FTHT device panel controls](docs/screenshots/ftht/device-panel-controls.png) |

| Slider | Event log |
| --- | --- |
| ![FTHT device panel slider](docs/screenshots/ftht/device-panel-slider.png) | ![FTHT device panel events](docs/screenshots/ftht/device-panel-events.png) |

## Troubleshooting

### `--tui` Does Not Work

Check these first:

1. The program must call `ft::create_window(cfg, argc, argv)` or
   `ft::create_window(argc, argv, cfg)`.
2. The program must be a combined `ft::` app. Direct `ftui::` programs ignore
   combined launch flags.
3. The process must have a real interactive terminal attached. File explorer
   launches usually do not.
4. Run from a terminal to test:

```bash
build/ft_combined_smoke --tui
```

If that renders, the flag works and the failed launch context probably had no
usable TTY.

### TUI Scrolling Does Not Move

TUI scrolling supports normal terminal wheel events, including two-finger
touchpad scrolling in terminal emulators that report wheel escape sequences. FT
enables SGR mouse mode for TUI apps. Wheel-down scrolls the page down and a
right-edge scrollbar appears when the rendered page is taller than the terminal.
The scrollbar is interactive: drag the thumb to scrub the page, or click the
track above/below it to page up or down.

Widget-local scroll still works for text areas, log views, list boxes, and
`scroll_area()`. Those widgets consume the wheel only while the pointer is over
the widget; otherwise the same wheel event scrolls the whole page. Arrow keys
continue to scroll focused text areas, log views, and scroll regions.

TUI tooltips use the same hover-stability heuristics as the native backend. They
wait two seconds before opening, reset when the pointer moves, and dismiss after
about one second once the pointer has left the owning widget even if that widget
still has keyboard focus.

### GUI Build Fails On Linux

Install the X11 and Cairo development packages, then compile with `pkg-config`:

```bash
c++ examples/ft_combined_smoke.cpp -o build/ft_combined_smoke -std=c++17 $(pkg-config --cflags --libs cairo x11) -pthread
```

### Web Port Already In Use

Change the configured port:

```cpp
cfg.web_port = 8082;
```

Then run:

```bash
build/ft_combined_smoke --web
```

### Browser Does Not Open From GUI Handoff

The server can still be running even if `xdg-open`, `open`, or `start` fails.
Use the printed URL or call `ft::url()` from a web frame. The default bind is
`0.0.0.0:8080`.

## Repository Notes

This repository keeps the combined source and examples in version control. The
original imported library snapshots and local build products are intentionally
ignored:

```text
ftui.hpp
ftti.hpp
ftht.hpp
README.ftui.md
README.ftti.md
README.ftht.md
build/
```

The public repo name is `ft`, and the header remains explicit that it is the
combined FT family build.
