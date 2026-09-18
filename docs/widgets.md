# Widget guide

[Documentation home](index.md) · [Exact signatures and types](api.md)

All snippets in this guide belong inside a frame unless marked as persistent state. Provide valid pointers and counts; initialize selection and value state yourself. A returned `bool` has widget-specific meaning.

## Text and input

| Widget | Meaning and result |
| --- | --- |
| `text`, `text_wrapped` | Display text; web escapes markup. Wrapped text uses available width. |
| `input` | Edits a caller-owned character buffer; true on a value change. Optional `enter_pressed` reports submission separately. |
| `text_area` | Multiline editor, five visible rows by default; true on a change. |
| `text_area_ex` | Multiline editor with flags. |
| `log_view` | Read-only text with eight rows by default and auto-scroll requested. |
| `code_view` | Plain source display with ten rows by default; no syntax highlighting or markup execution. |

`InputFlags` supports `Password`, `ReadOnly`, `CharsDecimal`, `CharsHexadecimal`, `CharsUppercase`, and `CharsNoBlank`; combine with `|`. Input filtering is a convenience, not domain validation. Parse and validate application data before using it. Password masking changes display, not storage. Web numeric controls ignore malformed or non-finite submissions. Web text truncation preserves UTF-8 character boundaries, and a submitted value that truncates to the already stored text does not report a change.

`TextAreaFlags` supports `ReadOnly`, `WordWrap`, and `AutoScrollBottom`. `LogViewFlags` supports `WordWrap` and `AutoScrollBottom`. In the web implementation `log_view` renders an escaped `<pre>` and ignores its flags; do not assume identical auto-scroll or wrapping behavior.

```cpp
bool submitted = false;
if (ft::input("Username", username, sizeof(username),
              ft::InputFlags::CharsNoBlank, &submitted)) {
    // Value changed; update validation feedback.
}
if (submitted) { /* handle submission */ }
```

## Buttons, values, and selection

`button()` returns true for activation. `checkbox()` and `slider_float()` update the supplied value and report a change. Supply an ordered slider range and a finite value within it.

`dropdown`, `listbox`, `radio_group`, `side_menu`, `side_menu_drawer`, and `tabs` update a zero-based selected index and report a selection change. Pass a valid array and matching count. Dropdown popup rows default to 8; listbox visible rows to 6; radio columns to 1. Web dropdowns use the browser's native select popup, ignoring the popup-row hint.

`collapsing_header(label, &open)` returns whether to render its body, not whether the state changed. A null `open` uses backend behavior. Web renders a heading and returns the supplied state (or true), without an interactive disclosure toggle.

```cpp
// Keep these outside the frame loop:
// int selected = 0; bool details_open = true;
const char* choices[] = {"Overview", "Settings"};
ft::tabs(choices, 2, &selected);
if (ft::collapsing_header("Details", &details_open)) {
    ft::text("Additional information");
}
```

## Tables

Keep a separate `TableState` for each table outside the loop:

```cpp
ft::TableRows rows = {{"Alice", "Ready"}, {"Bob", "Waiting"}};
ft::TableState state;
```

During rendering:

```cpp
ft::TableOptions options;
options.visible_rows = 8;
if (ft::table("People", {"Name", "Status"}, rows, state, options)) {
    // Selection or sorting changed.
}
```

`TableRows` owns its strings. `selected_row` is a **source row index**, even after sorting; `-1` means no selection. `first_row` is an offset into sorted display order. Inserting, removing, or reordering source records can change which record an index identifies, so reconcile selection in application code.

Sorting is stable and lexicographic: numeric strings are not sorted numerically. Repeated header activation toggles descending order. Missing cells compare as empty strings; extra cells add unnamed columns. `TableOptions` controls visible rows, headers, stripes, and user sorting. Disabling sortable headers does not inherently erase an existing `sort_column` in your state.

GUI tables provide fixed headers, clipped columns, wheel scrolling, and keyboard selection. Web/TUI use Previous/Next paging and first-cell selection. Pagination can change `first_row` without returning true; do not interpret the return value as a complete state-change notification.

## Context menus

Attach the default `LastItem` menu immediately after its target. Keep its ID stable and unique. Return value is the activated source item index, or `-1`.

```cpp
ft::button("Document");
ft::ContextMenuItem actions[] = {
    {"Copy", true},
    {"Unavailable", false},
    {"", true, false, true},
    {"Show details", true, show_details, false, true}
};
ft::ContextMenuOptions options;
int action = ft::context_menu("Document actions", actions, 4, options);
if (action == 3) show_details = !show_details;
```

Items specify label, enabled, checked, separator, and checkable in that order. The menu does not toggle your boolean automatically. Item arrays may be local to a render call.

Set `scope` to `Region` with `{x,y,width,height}` client coordinates, or `Window` for a fallback. Render specific menus before the window-wide menu; the first matching handler consumes the trigger. `right_click` and `keyboard` enable triggers; keyboard opening uses Shift+F10. Pass `true` as the final argument for programmatic opening.

GUI/web menus support keyboard navigation, dismissal, and bounded popup placement. `width` defaults to 220, `visible_rows` to 10. Animation defaults to 0.12 seconds and 0.025 overshoot; zero disables the pulse. Web respects reduced-motion preferences. TUI uses an action-list fallback rather than a spatial popup; region placement and animation do not transfer.

## Clipboard

Native clipboard calls require an open window. `set_clipboard_text()` returns write success; `get_clipboard_text()` returns a string, with empty text also representing unavailable data. These APIs handle text, not images or file lists.

`clipboard_can_read()` and `clipboard_can_write()` indicate an available integration, not guaranteed permission or successful transfer. GUI defaults to the OS clipboard. Web/TUI synchronous calls require explicit callbacks, except optional terminal copy through `enable_terminal_clipboard_osc52()`.

```cpp
ft::set_clipboard_handlers(ft::Mode::Tui, {
    []() -> std::string { return host_read(); },
    [](const char* value) -> bool { return host_write(value); }
});
```

Callbacks override facade clipboard access for the specified backend; they remain separate during handoff. Use valid `Mode` enum values. OSC 52 writes base64 terminal escape sequences and cannot confirm that the terminal accepted them; it does not enable reading.

Keep `ClipboardViewState` between frames and call `clipboard_view(label, state, rows)`. Local viewing reads a snapshot only when requested. In web mode the viewer uses browser clipboard APIs and keeps clipboard text in the browser; it does not populate the server-side state. Permission failures appear in the viewer. Browser clipboard use generally requires a secure context such as HTTPS or localhost.

Linux uses X11 CLIPBOARD, leaving PRIMARY alone. Keep pumping events to serve copied text. X11 incremental transfers are not implemented.

## Images, files, and child windows

Load assets once, draw them each frame, then free them. `ft::load_image()` creates a wrapper and attempts both native and TUI loading; a non-null wrapper alone does not prove successful image decoding. `free_image()` releases both handles and accepts null.

GUI images depend on the platform decoder (Linux uses Cairo PNG loading). TUI uses terminal image support or a placeholder. Web `image()` renders nothing; use explicit trusted HTML and an independently served image URL if needed. FT does not turn a filesystem path into an HTTP asset route.

`open_file_dialog()` returns the chosen path, or an empty string on cancel/failure. `FileFilter` has `name` and `spec` fields. TUI and web return an empty string; they do not present an equivalent picker.

`open_child_window(cfg, callback)` creates a native child window in GUI mode. Other modes execute the callback inline. Pass a valid callback and keep captured state alive for as long as native rendering needs it.
