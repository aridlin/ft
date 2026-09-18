# Getting started

[Documentation home](index.md) · Next: [Lifecycle](lifecycle.md)

## Requirements and build

Copy `ft.hpp` into your project. Use C++17 or newer. Define `FT_IMPLEMENTATION` before including it in **exactly one translation unit**. This compiles all three backends; choosing `--web` at runtime does not remove native build dependencies.

On Linux, install a C++ compiler, `pkg-config`, and the Cairo and X11 development packages. Run from the repository root:

```sh
mkdir -p build
c++ docs/examples/hello.cpp -I. -std=c++17 -o build/docs-hello $(pkg-config --cflags --libs cairo x11) -pthread
build/docs-hello --gui
```

The Linux native renderer uses X11 and Cairo, including XWayland when available. Setting a Wayland environment variable alone does not provide a native Wayland renderer.

In an MSVC developer command prompt:

```bat
mkdir build
cl /EHsc /std:c++17 /I. docs\examples\hello.cpp /Fe:build\docs-hello.exe
```

Windows uses platform SDK libraries and MSVC auto-link pragmas in the header. Other Windows compiler/linker combinations may require explicit platform libraries; inspect the header's `#pragma comment(lib, ...)` entries. The non-Windows native implementation is X11/Cairo, not a Cocoa backend; macOS is not a documented native build target here.

## Complete application

The following is also available as [hello.cpp](examples/hello.cpp).

```cpp
#define FT_IMPLEMENTATION
#include "ft.hpp"

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "Hello FT";
    cfg.web_host = "127.0.0.1";
    if (!ft::create_window(cfg, argc, argv)) return 1;

    char name[64] = "operator";
    bool enabled = true;
    while (ft::pump()) {
        ft::begin();
        ft::text("Hello FT");
        ft::input("Name", name, sizeof(name));
        ft::checkbox("Enabled", &enabled);
        if (ft::button("Save", ft::ColorRole::Accent)) {
            ft::toast_success("Saved");
        }
        ft::end();
    }
    ft::shutdown();
}
```

The application owns `name` and `enabled`. They survive between frames. Put persistence or other side effects inside interaction handlers, such as the `if (button(...))` block, rather than executing them every frame.

## Select a backend

```sh
build/docs-hello --gui
build/docs-hello --tui
build/docs-hello --web
```

For web mode, visit `http://127.0.0.1:8080/`. Stop the server process from its terminal when finished. For TUI mode, use an interactive terminal with both standard input and output attached; redirecting output can make forced TUI creation fail.

With no mode flag, `Mode::Auto` tries the detected backend and can fall back. [Lifecycle](lifecycle.md) describes the exact rules.

## Multiple translation units

In `ft_impl.cpp`:

```cpp
#define FT_IMPLEMENTATION
#include "ft.hpp"
```

In `main.cpp` and other application files:

```cpp
#include "ft.hpp"
```

Compile and link both files together with the same platform dependencies. Do not define `FT_IMPLEMENTATION` in a shared application header or as a global compiler flag for all translation units. Multiple definitions cause linker errors; omitting the implementation causes unresolved backend symbols.

## Compile-time switches

| Macro | Meaning |
| --- | --- |
| `FT_IMPLEMENTATION` | Enables all three embedded implementations. |
| `FTUI_IMPLEMENTATION`, `FTTI_IMPLEMENTATION`, `FTHT_IMPLEMENTATION` | Enable individual implementations for direct backend use; do not assume a partial build can use all facade calls. |
| `FTUI_KEEP_CONSOLE` | Keeps the Windows console for GUI diagnostics. |
| `FTUI_LINUX_FONT` | String literal overriding the Linux font family. |
| `FTUI_DISABLE_EFFECTS` | Removes the Windows effects layer at compile time. |
| `FTUI_DEFAULT_STYLE`, `FTTI_DEFAULT_STYLE` | Backend default-style factory overrides; define before inclusion. |

Use `ft::set_style()` for portable runtime theming. See [configuration](configuration.md) for runtime effects and pacing.
