# FT Combined

`ft.hpp` merges the FT family behind one immediate-mode API:

- `ftui.hpp`: native GUI backend
- `ftti.hpp`: terminal TUI backend
- `ftht.hpp`: embedded web UI backend

The public syntax follows FTUI:

```cpp
#define FT_IMPLEMENTATION
#include "ft.hpp"

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "Combined FT App";
    cfg.mode = ft::Mode::Auto;

    if (!ft::create_window(cfg, argc, argv)) return 1;

    char name[64] = "";
    bool enabled = true;

    while (ft::pump()) {
        ft::begin();
        ft::text("Hello from ft.hpp");
        ft::input("Name", name, sizeof(name));
        ft::checkbox("Enabled", &enabled);
        if (ft::button("Run")) ft::toast_success("Run clicked");
        ft::end();
    }

    ft::shutdown();
    return 0;
}
```

## Modes

`cfg.mode = ft::Mode::Auto` chooses a backend at launch:

- GUI when a normal desktop display is available
- TUI when running on a real terminal without a display
- web server when neither GUI nor TUI is available

Command-line flags override `cfg.mode`:

- `--ft-auto`
- `--ft-gui` or `--gui`
- `--ft-tui` or `--tui`
- `--ft-web` or `--web`
- `--ft-mode=auto|gui|tui|web`

Forced modes do not fallback. Auto mode tries its best fallback path if the first backend cannot open.

## Build

Linux:

```bash
c++ examples/combined_smoke.cpp -o combined_smoke -std=c++17 $(pkg-config --cflags --libs cairo x11) -pthread
```

Windows with MSVC:

```bat
cl /std:c++17 examples\combined_smoke.cpp d2d1.lib dwrite.lib gdi32.lib ole32.lib uuid.lib user32.lib windowscodecs.lib ws2_32.lib
```

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

## Backend Escape Hatches

Use `ft::` for portable FTUI-style code. The original namespaces still exist:

- `ftui::` for native-only features
- `ftti::` for terminal-only features
- `ftht::` for server/request-specific features

`ft::html()`, `ft::url()`, `ft::path()`, `ft::method()`, `ft::param()`, and `ft::set_status()` are web-only helpers that become harmless no-ops or empty values outside web mode.
