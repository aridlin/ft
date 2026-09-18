# Lifecycle and application state

[Documentation home](index.md) · [API reference](api.md)

## Creation and mode selection

`create_window(cfg, argc, argv)` returns whether the chosen backend started. The overload `create_window(argc, argv, cfg)` is equivalent. Check the result before entering the UI loop.

| Flag | Mode |
| --- | --- |
| `--ft-auto`, `--ft-mode=auto` | Auto |
| `--gui`, `--ft-gui`, `--ft-mode=gui` | GUI |
| `--tui`, `--ft-tui`, `--ft-mode=tui` | TUI |
| `--web`, `--ft-web`, `--ft-mode=web` | Web |

Flags override `Config::mode`. The last recognized mode flag wins. Unknown arguments and unknown `--ft-mode=` values do not change the selected mode. Arguments are inspected, not removed from `argv`. Without `argc, argv`, only the configuration applies.

Auto prefers GUI when a display is detected (always initially preferred on Windows), then an interactive TUI, then web. If the first choice fails, it tries an available TUI and then web. Forced modes do not fall back. Use `active_mode()` after successful creation to inspect the actual backend; `config()` retains the effective launch configuration, whose `mode` can still be `Auto`.

## Frame contract

Call `pump()`, `begin()`, widget functions, and `end()` in that order. When `pump()` returns false, leave the loop and call `shutdown()`. Pair each successful frame's `begin()` with `end()`; avoid early returns that leave a frame unfinished.

GUI/TUI frames process input and draw local output. Web frames process a request and produce its response. Pure web `pump()` waits for requests, so frame count is not a clock and frame-driven background jobs will stall without traffic. Keep slow disk/network work outside rendering; communicate completed results back to the UI through application-owned state.

The implementations use shared backend state. Treat UI calls as belonging to one UI thread and one active facade application. This API does not promise arbitrary concurrent calls or independent facade instances. Synchronize worker-produced data before rendering it.

`request_redraw()` wakes local backends; it does not push a new page to browsers. `set_fps_limit()` affects local pacing, while web refresh uses client polling. See [web integration](web.md).

## State and identity

Widget state such as buffers, checked values, selected indices, and `TableState` belongs to the caller. Initialize it outside the frame loop. C-string buffers must be writable, NUL-terminated, and accompanied by their full capacity in bytes, including room for the terminator. Keep configuration string storage alive for the application's lifetime: `Config` stores pointers, not owning strings.

Give interactive widgets stable, distinct labels. A `##` suffix can distinguish controls with the same visible text:

```cpp
if (ft::button("Delete##left")) { /* left action */ }
if (ft::button("Delete##right")) { /* right action */ }
```

Do not rely on a hidden suffix to make changing visible text preserve identity across every backend. Web identifiers hash the complete label (and any backend-supplied salt). Keep labels stable between displaying a form and processing its submission, and ensure distinct interactive fields do not share the same label. `text()` is display content; do not assume it strips widget ID suffixes.

## Commands

Press `:` outside text editing to open the command prompt, Tab to complete, Enter to execute, or Escape to cancel. Common commands include `:help`, `:q`, `:tab next`, `:tab prev`, `:theme dark`, and `:rr`. Exact theme and navigation commands vary by backend. Web quit commands do not terminate the server.

`set_command_handler()` installs a function pointer of type `bool (*)(const char*)`; return true for a handled command. It targets the current render backend. Installing a custom GUI/TUI handler replaces the facade's handler used for `:web`, `:ws`, `:server`, and `:webserver`; preserve any desired application command behavior yourself. Registration on a GUI frame does not also register a web handler.

## Web handoff

Call `webserver_button()` inside the frame or `start_webserver()` after successful creation. A successful handoff starts FTHT using the launch configuration's `web_*` fields and attempts to open a browser.

The local backend remains active. GUI mode becomes an 86×76 stop-control window; TUI mode displays a server control panel. Incoming HTTP requests route the application's widget calls to web; local control frames suppress normal widgets. Application statements outside widget callbacks can still execute, so do not use rendering as a background-task scheduler.

`active_mode()` continues to identify the local backend during handoff. `url()`, `path()`, and other facade HTTP helpers return meaningful request values only on web render frames. `webserver_running()` reports both handoff and pure-web operation.

`stop_webserver()` stops an attached handoff server and restores the configured GUI dimensions. It does not stop a pure `Mode::Web` application; use its normal lifecycle and `shutdown()`. Handoff uses the same application state, not a separate browser copy.
