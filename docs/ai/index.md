# FT: LLM reference

> FT is a C++17 single-header immediate-mode UI library. Use the `ft::` facade to target native GUI, terminal TUI, or embedded HTTP. This reference is plain text and needs no JavaScript, browser, or visual interpretation.

## Retrieval

- [Complete reference in one request](https://aridlin.pl/ft/ai/llms-full.txt): contracts, API signatures, configuration, backend differences, guides, and complete C++ examples.
- [Compact entry point](https://aridlin.pl/ft/ai/llms.txt): this document.
- [Structured API inventory](https://aridlin.pl/ft/ai/api.json): public facade overloads, type declarations, and configuration defaults.
- [Snapshot manifest](https://aridlin.pl/ft/ai/manifest.json): header SHA-256, Git HEAD, working-tree status, and file checksums. HEAD alone does not identify uncommitted source changes.
- [Human documentation](https://aridlin.pl/ft/docs/): interactive lessons and visual examples.
- [Repository](https://github.com/aridlin/ft): source repository; verify the header in the consumer's checkout before assuming this documentation matches it.

## Targeted reference

- [API](https://aridlin.pl/ft/ai/api.md): exact public facade signatures and supporting types.
- [Configuration](https://aridlin.pl/ft/ai/configuration.md): all fields and defaults.
- [Lifecycle](https://aridlin.pl/ft/ai/lifecycle.md): mode selection, state, identity, frame loop, commands, and handoff.
- [Widgets](https://aridlin.pl/ft/ai/widgets.md): input, selection, tables, menus, clipboard, images, and ownership.
- [Layout and style](https://aridlin.pl/ft/ai/layout-and-style.md): scopes, sizing, themes, overlays, and progress.
- [Backends](https://aridlin.pl/ft/ai/backends.md): compatibility matrix and direct backend APIs.
- [Web](https://aridlin.pl/ft/ai/web.md): request processing, polling, login, and shared-state limits.
- [Build instructions](https://aridlin.pl/ft/ai/getting-started.md): compiler requirements and implementation macros.
- [Troubleshooting](https://aridlin.pl/ft/ai/examples-and-troubleshooting.md): demos, diagnostics, and regression commands.
- [Complete minimal C++ app](https://aridlin.pl/ft/ai/examples/hello.cpp).
- [Complete controls C++ app](https://aridlin.pl/ft/ai/examples/controls.cpp).

## Code-generation contract

1. Include `ft.hpp`. Define `FT_IMPLEMENTATION` before inclusion in exactly one translation unit. Other translation units include it without the macro. Use C++17 or newer.
2. Prefer `ft::`. Direct `ftui::`, `ftti::`, and `ftht::` APIs have separate initialization and types. Do not initialize a direct backend and assume facade routing was initialized. Do not depend on `detail` or `internal` symbols.
3. Check `ft::create_window(cfg, argc, argv)`. On success, repeat `pump()` → `begin()` → widgets → `end()`. On loop exit, call `shutdown()`. Pass arguments if runtime mode flags are expected.
4. Keep application state outside the frame loop. Mutating widgets receive pointers to that state. Buttons return activation; most editing widgets return value changes. `collapsing_header()` and `modal()` instead report whether their content is shown.
5. Use valid, NUL-terminated writable buffers with their capacity in bytes including the terminator. Keep pointer-backed configuration strings alive. Labels must be distinct and stable; `##` adds a hidden suffix for interactive widget labels. Do not assume changing visible text preserves identity.
6. Balance color and disabled scopes, and finish each begun frame. Layout callbacks execute during rendering. Captured state for native child windows must remain alive for subsequent rendering.
7. Treat UI operations as single-UI-thread operations. Backend state is shared. Synchronize worker results before displaying them; do not do slow network/disk operations in every frame.
8. Check the exact signature inventory before generating a call. There is no documented facade `slider_int`, `input_text`, `same_line`, `begin_window`, or `end_window`. Use `slider_float`, `input`, `row`, and the actual lifecycle instead. `set_window_size` exists in FTUI, not the public facade.

## Backend constraints that affect generated code

- `FT_IMPLEMENTATION` compiles all embedded implementations. Runtime `--web` does not remove Linux X11/Cairo link requirements. Linux build: `c++ app.cpp -I. -std=c++17 -o app $(pkg-config --cflags --libs cairo x11) -pthread`.
- Auto mode prefers a detected GUI, then usable terminal, then web, with fallback on startup failure. A forced mode does not fall back. The final recognized mode argument wins.
- Explicit layout dimensions pass through unchanged. GUI/web use pixel-oriented sizing; terminal sizing uses cells/rows. Prefer proportional rows for portable layouts.
- Pure-web frames are request-driven. Do not increment timers, run background jobs, or assume a constant animation rate from frame count. `request_redraw()` does not push browser updates; configure client polling when needed.
- Handoff preserves the local `active_mode()` while routing request frames to web. `active_mode()` is not a test for the current render target. Normal local widgets are suppressed while the handoff control view is active.
- `stop_webserver()` stops a handoff server, not a pure-web application. Pure web uses its lifecycle and `shutdown()`.
- Web ignores image rendering through `ft::image()`, focus requests, and most rich progress effects. Web sidebar layout scopes execute callbacks without the native sidebar geometry. Web collapsing headers are not interactive disclosure controls.
- File dialogs return empty outside GUI. Non-GUI child-window callbacks execute inline. `ft::debug()` exposes live GUI state only for GUI rendering.
- Tables sort lexicographically and stably. `selected_row` is a source index, not a display index. `first_row` is a sorted display offset. Paging can change state without a true return value. Reconcile selection after modifying the source collection.
- `ft::load_image()` can return a wrapper even when native decoding fails. Load once, render repeatedly, and release with `free_image()`.
- Synchronous web clipboard calls do not read the browser clipboard. `clipboard_view()` uses browser-local clipboard access; its text is not sent to the server. TUI integration uses explicit callbacks or opt-in OSC 52 copying. X11 clipboard ownership needs a running event loop.
- The default HTTP bind is `0.0.0.0:8080` with no login. Use loopback for a local-only example. The embedded HTTP server has no built-in TLS. The built-in password hash is a 64-bit non-cryptographic hash, not a modern password KDF. Browser clients share application state unless the application adds isolation.
- `html()` and `web_extra_head_html` insert trusted raw markup. Use `text()` for untrusted strings. Input filtering is not application-level validation.

## Recommended retrieval and validation workflow

For a small app, fetch this page plus the API and the applicable widget/backend guide. For substantial implementation, fetch `llms-full.txt` once. Verify the consumer's `ft.hpp` against the snapshot manifest; this documentation may include uncommitted changes not yet present on GitHub.

Generate a complete program or clearly label a fragment's required state and lifecycle context. Compile against the actual consumer header. A successful compile is not proof of GUI behavior, browser permissions, OS clipboard functionality, or terminal support. State which backend was exercised and which checks were only static.

Use the complete downloadable examples for baseline compilation. The human site's animated examples are JavaScript teaching models, not an FT runtime or a WebAssembly build.
