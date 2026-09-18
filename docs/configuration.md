# Configuration

[Documentation home](index.md) · [Backend differences](backends.md)

Create `ft::Config cfg;`, change fields, and pass it to `create_window`. `config()` returns a const reference to the effective launch config. It is not a live, mutable settings interface. String fields store pointers; keep their backing storage alive.

## Window and mode

| Field | Type | Default | Meaning |
| --- | --- | --- | --- |
| `mode` | `Mode` | `Mode::Auto` | Preferred backend; command-line flags override it. |
| `title` | `const char*` | `"FT App"` | Window/page title; keep string storage alive. |
| `width` | `int` | `960` | Initial GUI width in pixels. |
| `height` | `int` | `640` | Initial GUI height in pixels. |
| `fps_limit` | `int` | `60` | Local frame limit. GUI: non-positive means uncapped; facade TUI creation maps non-positive to 30. Web is request-driven. |
| `resizable` | `bool` | `true` | Native window resize policy. |
| `center_window` | `bool` | `true` | Native startup placement. |
| `icon` | `void*` | `nullptr` | Native icon pointer; HICON on Windows, ignored on Linux. |
| `enable_effects` | `bool` | `true` | Local widget animation/effects switch. |
| `backdrop_effect` | `BackdropEffect` | `BackdropEffect::Blur` | Local backdrop selection; blur is Windows-specific. |
| `dither_size` | `int` | `4` | Dither motif size. |
| `window_transparency` | `WindowTransparency` | `WindowTransparency::Opaque` | Native transparency mode; compositor/platform dependent. |
| `window_opacity` | `float` | `0.92f` | Native opacity for applicable transparency modes. |

## Terminal

| Field | Type | Default | Meaning |
| --- | --- | --- | --- |
| `tui_alternate_screen` | `bool` | `true` | Use alternate terminal screen. |
| `tui_enable_mouse` | `bool` | `true` | Enable terminal mouse reporting. |
| `tui_quit_on_ctrl_q` | `bool` | `true` | Allow Ctrl+Q to exit TUI. |
| `tui_animate` | `bool` | `true` | Enable terminal animations. |

## Web

| Field | Type | Default | Meaning |
| --- | --- | --- | --- |
| `web_host` | `const char*` | `"0.0.0.0"` | HTTP bind address; default listens on all IPv4 interfaces. |
| `web_port` | `int` | `8080` | TCP listening port. |
| `web_max_request_bytes` | `int` | `32 * 1024` | Request size limit in bytes. |
| `web_read_timeout_ms` | `int` | `1500` | Socket request read timeout in milliseconds. |
| `web_client_poll_ms` | `int` | `0` | Browser polling interval; zero disables polling. |
| `web_auto_submit` | `bool` | `true` | Submit supported field changes automatically. |
| `web_print_url` | `bool` | `true` | Print server URL on startup. |
| `web_debug_output` | `bool` | `false` | Server diagnostic output. |
| `web_client_debug_output` | `bool` | `false` | Client diagnostic output. |
| `web_dark_mode` | `bool` | `false` | Request dark web appearance. |
| `web_respect_browser_dark_mode` | `bool` | `false` | Consult browser color-scheme preference. |
| `web_extra_head_html` | `const char*` | `nullptr` | Trusted raw HTML inserted into the page head. |
| `web_login_mode` | `LoginMode` | `LoginMode::None` | None, Form, or novelty DrawPenis gate. |
| `web_login_username` | `const char*` | `"admin"` | Configured login username. |
| `web_login_password` | `const char*` | `nullptr` | Configured plaintext password pointer; keep storage alive. |
| `web_login_salt` | `const char*` | `"ftht-login"` | Salt for configured password hash. |
| `web_login_db_path` | `const char*` | `nullptr` | Credential database path; needed for persistent registration. |
| `web_login_password_hash` | `uint64_t` | `0` | Built-in 64-bit password hash; not a modern password KDF. |
| `web_login_session_seconds` | `int` | `10 * 60` | Login session lifetime in seconds. |
| `web_login_allow_registration` | `bool` | `false` | Allow form registration; requires a database path. |

## Local web configuration

```cpp
ft::Config cfg;
cfg.mode = ft::Mode::Web;
cfg.web_host = "127.0.0.1";
cfg.web_port = 8082;
cfg.web_client_poll_ms = 1000;
```

Login and network settings also apply to a GUI/TUI web handoff. See [Web integration](web.md) before deploying a remotely reachable panel. Runtime style changes use `set_style`; local FPS and effects also have dedicated setters in the [API reference](api.md). Not every setter updates the stored launch configuration.
