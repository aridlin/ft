# FT documentation

FT is a C++17 single-header library for immediate-mode native windows, terminal interfaces, and embedded HTTP interfaces. The `ft::` facade routes an application to `ftui::` (GUI), `ftti::` (TUI), or `ftht::` (web).

These docs describe the current checked-out `ft.hpp`, including its data widgets and clipboard integrations. Backend compatibility means a shared calling interface; rendering, event timing, and supported features differ.

## Learn

New to GUI programming? Start with [Your first interface](learn.md), a guided introduction with interactive examples in the built site.


1. [Build your first application](getting-started.md): dependencies, a complete program, multiple translation units.
2. [Understand the lifecycle](lifecycle.md): application state, widget identity, frames, commands, and web handoff.
3. [Arrange and style an interface](layout-and-style.md): layout scopes, sizing, themes, overlays, and feedback.
4. [Use widgets](widgets.md): input, selection, tables, context menus, images, and clipboard.

## Reference

- [Configuration](configuration.md): every facade configuration field and its default.
- [API reference](api.md): all public facade functions, types, defaults, and links to usage guides.
- [Backend behavior](backends.md): feature matrix, platform details, and escape hatches.
- [Web integration](web.md): requests, polling, login, shared state, and HTTP limitations.
- [Examples and troubleshooting](examples-and-troubleshooting.md): runnable demos, diagnostics, and regression commands.
- [Maintaining the docs](contributing.md): scope, source map, and validation.

Start with `ft::` for an application that should support runtime mode selection. Choose a backend namespace only when deliberately writing a backend-specific application. Namespaces named `detail` or `internal` are implementation details, even though this single header exposes them to the compiler.
