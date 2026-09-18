# Embedded web integration

[Documentation home](index.md) · [Configuration](configuration.md)

## Requests and rendering

FTHT accepts HTTP requests and renders the immediate-mode UI as HTML forms. Widget calls inspect posted parameters, update application state, and append output. `end()` completes the response. This is an embedded server with shared application state, not a per-user application framework.

Use `path()`, `method()`, `is_post()`, and `param(name)` during a web frame. A missing parameter can return null; copy values that need to outlive the request. Outside web rendering the facade returns empty strings/false and ignores `html()` or `set_status()`.

```cpp
if (ft::is_post()) {
    const char* value = ft::param("external_value");
    if (value && *value) { /* validate before use */ }
}
```

`set_status(code, text)` changes the response status. `html(markup)` inserts raw HTML; it does not escape strings. Use `text()` for untrusted text. `web_extra_head_html` is likewise trusted raw markup for the document head. Custom form fields and scripts must coexist with FT's generated form names and lifecycle; avoid reserved `_ftht_*` names.

Keep form widget order and labels stable between GET and POST. Do not place irreversible side effects in unconditional rendering code: polling, multiple clients, and repeated requests can render the same view repeatedly.

## Binding and refresh

The default is `0.0.0.0:8080`, meaning all IPv4 interfaces, with no login. For a local utility set `web_host = "127.0.0.1"`. A bind address such as `0.0.0.0` is not the address remote users should enter; use the server's reachable host address.

`web_auto_submit` submits supported control changes automatically. `web_client_poll_ms` enables periodic client refresh when positive; its default of zero disables polling. Choose polling according to the freshness needed and cost of rendering. `request_redraw()` does not refresh browsers.

The server limits request bytes (default 32768) and read time (default 1500 ms). These protect the request path from unbounded reads but do not make it a general high-concurrency HTTP server. Test payload sizes and response latency for your actual controls. Request handling and UI work share the application loop. The reader enforces the byte limit over headers plus body, handles Content-Length case-insensitively, and rejects incomplete bodies, invalid/conflicting lengths, and unsupported Transfer-Encoding (including chunked requests). Trailing bytes beyond the declared body are not treated as form data.

## Login configuration

`LoginMode::None` disables the login gate. `LoginMode::Form` enables username/password login. `LoginMode::DrawPenis` is a novelty gesture demo preserved by the library; it is not a password-authentication substitute.

```cpp
ft::Config cfg;
cfg.mode = ft::Mode::Web;
cfg.web_host = "127.0.0.1";
cfg.web_login_mode = ft::LoginMode::Form;
cfg.web_login_username = "operator";
// Supply password storage from your application's configuration.
cfg.web_login_password = password;
```

The library also accepts `web_login_password_hash`, produced with `make_login_password_hash(password, salt)`. Match `web_login_salt` to the salt used for the hash. This built-in 64-bit hash is not a modern password-storage KDF. The embedded server serves HTTP, not TLS; its login facilities should not be treated as a hardened public authentication system.

`web_login_db_path` supplies persistent credential storage. Registration requires a database path and `web_login_allow_registration = true`; registration defaults to disabled. Session duration defaults to 600 seconds. Protect the credential database as application data and do not place it in public source or a served directory.

For deployment that needs remote confidential access, put transport encryption and suitable authentication/access controls in a separately configured front end. FT's login does not isolate per-client C++ widget state: clients share the same values unless your application explicitly implements separation.

## Browser-only behavior

Clipboard viewing runs locally in browser JavaScript and requires the browser's clipboard permissions/secure context. Synchronous C++ clipboard calls do not read the visitor's clipboard. Web images require explicit markup and reachable asset URLs; there is no automatic mapping from `load_image()` to an HTTP resource.

The `:` command palette avoids ordinary typing targets, completes commands with Tab, and can perform browser-side tab navigation where possible. Other commands post back to the server. Client polling replaces content, so custom scripts should tolerate refresh and clean up their own listeners.

## Diagnostics

Enable `web_debug_output` for server diagnostics and `web_client_debug_output` for browser diagnostics. `web_print_url` prints the URL at startup by default. Use diagnostics in development with appropriate care for application data that may appear in logs.

If the browser launch helper fails during handoff, the server may still be running. Open its address manually. `ft::url()` is a render-target-aware helper; outside a web frame use your configured address or the direct backend's URL when intentionally working with FTHT.
