# API reference

[Documentation home](index.md)

All signatures below are in `namespace ft`. Defaults are copied from the current header. These are declarations for reference; include `ft.hpp` rather than copying them into your program. Follow the guide links for ownership, return values, and backend limitations.

## Lifecycle and commands

[Behavior and examples](lifecycle.md)

```cpp
Mode active_mode();
const Config& config();
bool webserver_running();
bool start_webserver();
void stop_webserver();
bool create_window(const Config& cfg = {}, int argc = 0, char** argv = nullptr);
bool create_window(int argc, char** argv, const Config& cfg = {});
bool pump();
void begin();
void end();
void shutdown();
void request_redraw();
void set_quit_on_ctrl_q(bool enabled);
void set_command_handler(CommandHandler handler);
void set_fps_limit(int fps);
int get_fps_limit();
bool webserver_button(const char* start_label = "O");
```

## Style and window controls

[Behavior and examples](layout-and-style.md)

```cpp
void set_backdrop_effect(BackdropEffect effect);
void set_dither_size(int px);
void set_window_transparency(WindowTransparency mode);
void set_window_opacity(float opacity);
void set_window_icon(void* native_icon);
void set_window_icon_builtin(BuiltinIcon variant = BuiltinIcon::Symbol);
Style default_dark_style();
Style catppuccin_mocha_style();
Style nord_style();
Style gruvbox_dark_style();
Style one_dark_style();
Style ghostty_green_style();
Style style_from_name(const char* name);
const char* style_name(const Style& style);
const char* current_style_name();
const Style& get_style();
void set_style(const Style& style);
Color color_from_hex(const char* hex);
Color color_from_hex(std::string_view hex);
void push_color(ColorRole role, Color color);
void pop_color();
void set_next_color(ColorRole role, Color color);
```

## Text and controls

[Behavior and examples](widgets.md)

```cpp
void text(const char* label);
void text_wrapped(const char* value);
void separator();
void spacing(float px = 8.0f);
bool input(const char* label, char* buffer, int buffer_size, InputFlags flags = InputFlags::Default, bool* enter_pressed = nullptr);
bool text_area(const char* label, char* buffer, int buffer_size, int rows = 5);
bool text_area_ex(const char* label, char* buffer, int buffer_size, int rows = 5, TextAreaFlags flags = TextAreaFlags::Default);
void log_view(const char* label, const char* value, int rows = 8, LogViewFlags flags = LogViewFlags::AutoScrollBottom);
bool checkbox(const char* label, bool* value);
bool slider_float(const char* label, float* value, float min_v, float max_v);
bool button(const char* label);
bool button(const char* label, ColorRole role);
bool button(const char* label, Color color);
bool dropdown(const char* label, const char* const* items, int count, int* selected, int popup_rows = 8);
bool listbox(const char* label, const char* const* items, int count, int* selected, int visible_rows = 6);
bool radio_group(const char* label, const char* const* items, int count, int* selected, int columns = 1);
bool collapsing_header(const char* label, bool* open = nullptr);
bool side_menu(const char* label, const char* const* items, int count, int* selected);
bool side_menu_drawer(const char* label, const char* const* items, int count, int* selected, float width = 260.0f);
bool tabs(const char* const* labels, int count, int* selected);
```

## Layout and feedback

[Behavior and examples](layout-and-style.md)

```cpp
void row(int cols, std::function<void()> fn);
void row(std::initializer_list<float> weights, std::function<void()> fn);
void split(std::initializer_list<float> columns, std::function<void()> fn);
void side_layout(float side_width, std::function<void()> fn);
void content(std::function<void()> fn);
void scroll_area(const char* label, float height, std::function<void()> fn);
void set_next_width(float px);
void set_next_fill();
void set_next_percent(float pct);
void set_next_limits(float min_px, float max_px);
void set_next_align(Align align);
void open_modal(const char* label);
bool modal(const char* label, std::function<void()> fn);
void close_modal();
void begin_disabled();
void end_disabled();
void tooltip(const char* value);
void request_focus(const char* label);
float calc_text_width(const char* value);
float calc_text_height(const char* value, float wrap_width);
void toast(const Toast& value);
void toast(const char* message);
void toast_info(const char* message);
void toast_success(const char* message);
void toast_warning(const char* message);
void toast_error(const char* message);
void clear_toasts();
void progress_bar(float progress);
void progress_bar(float progress, const char* label_or_mask_path);
void progress_bar(float progress, const ProgressStyle& style);
```

## Resources and data widgets

[Behavior and examples](widgets.md)

```cpp
ImageHandle* load_image(const char* utf8_path);
void image(ImageHandle* img, float width, float height);
void free_image(ImageHandle* img);
std::string open_file_dialog(const char* title = "Open File", const FileFilter* filters = nullptr, int filter_count = 0);
void open_child_window(const Config& cfg, std::function<void()> fn);
DebugState& debug();
void set_clipboard_handlers(Mode mode, ClipboardHandlers handlers);
void enable_terminal_clipboard_osc52(bool enabled = true);
bool set_clipboard_text(const char* value);
std::string get_clipboard_text();
bool clipboard_can_read();
bool clipboard_can_write();
int context_menu(const char* label, const ContextMenuItem* items, int count, const ContextMenuOptions& options = {}, bool open = false);
bool table(const char* label, const std::vector<std::string>& headers, const TableRows& rows, TableState& state, const TableOptions& options = {});
void code_view(const char* label, const char* source, int rows = 10);
void clipboard_view(const char* label, ClipboardViewState& state, int rows = 10);
```

## HTTP helpers

[Behavior and examples](web.md)

```cpp
void html(const char* markup);
const char* url();
const char* path();
const char* method();
bool is_post();
const char* param(const char* name);
void set_status(int code, const char* text);
uint64_t make_login_password_hash(const char* password, const char* salt = "ftht-login");
```

## Types

`Config` is documented field by field in [Configuration](configuration.md). The following enums and records are public facade types or aliases to their FTUI/FTHT equivalents. `TableRows` is `std::vector<std::vector<std::string>>`. `CommandHandler` is `bool (*)(const char*)`.

### Mode

```cpp
enum class Mode {
    Auto,
    Gui,
    Tui,
    Web,
};
```

### Align

```cpp
enum class Align { Start, Center, End };
```

### Color

```cpp
struct Color { float r, g, b, a; };
```

### ColorRole

```cpp
enum class ColorRole {
    Background,
    Panel,
    Text,
    TextDim,
    Border,
    Button,
    ButtonHover,
    ButtonActive,
    InputBg,
    InputFocus,
    Accent,
    Warning,
    Success,
};
```

### BuiltinIcon

```cpp
enum class BuiltinIcon {
    Symbol,
    SymbolWithText,
};
```

### BackdropEffect

```cpp
enum class BackdropEffect {
    Blur,
    BayerDither,
};
```

### WindowTransparency

```cpp
enum class WindowTransparency {
    Opaque,
    Plain,
    BayerDither,
    Blur,
};
```

### InputFlags

```cpp
enum class InputFlags : unsigned {
    Default = 0,
    Password = 1 << 0,
    ReadOnly = 1 << 1,
    CharsDecimal = 1 << 2,
    CharsHexadecimal = 1 << 3,
    CharsUppercase = 1 << 4,
    CharsNoBlank = 1 << 5,
};
```

### TextAreaFlags

```cpp
enum class TextAreaFlags : unsigned {
    Default = 0,
    ReadOnly = 1 << 0,
    WordWrap = 1 << 1,
    AutoScrollBottom = 1 << 2,
};
```

### LogViewFlags

```cpp
enum class LogViewFlags : unsigned {
    Default = 0,
    WordWrap = 1 << 0,
    AutoScrollBottom = 1 << 1,
};
```

### ToastType

```cpp
enum class ToastType {
    Info,
    Success,
    Warning,
    Error,
};
```

### LoginMode

```cpp
enum class LoginMode {
    None,
    DrawPenis,
    Form,
};
```

### Style

```cpp
struct Style {
    Color background    = {0.055f, 0.055f, 0.059f, 1.0f};
    Color panel         = {0.090f, 0.094f, 0.102f, 1.0f};
    Color text          = {0.910f, 0.910f, 0.910f, 1.0f};
    Color text_dim      = {0.663f, 0.678f, 0.702f, 1.0f};
    Color border        = {0.169f, 0.176f, 0.192f, 1.0f};
    Color button        = {0.090f, 0.094f, 0.102f, 1.0f};
    Color button_hover  = {0.137f, 0.149f, 0.169f, 1.0f};
    Color button_active = {0.176f, 0.192f, 0.220f, 1.0f};
    Color input_bg      = {0.071f, 0.075f, 0.082f, 1.0f};
    Color input_focus   = {0.310f, 0.420f, 0.780f, 1.0f};
    Color accent        = {0.310f, 0.420f, 0.780f, 1.0f};
    Color warning       = {0.894f, 0.486f, 0.353f, 1.0f};
    Color success       = {0.420f, 0.741f, 0.522f, 1.0f};
    float window_padding = 20.0f;
    float item_spacing   = 10.0f;
    float item_height    = 36.0f;
    float rounding       = 8.0f;
    float border_width   = 1.0f;
    float font_size      = 16.0f;
};
```

### Toast

```cpp
struct Toast {
    const char* message = "";
    ToastType   type = ToastType::Info;
    int         duration_ms = 3500;
    bool        dismissible = true;
};
```

### ProgressStyle

```cpp
struct ProgressStyle {
    const char* label = nullptr;
    const char* mask_path = nullptr;
    const char* mask_svg = nullptr;
    const char* mask_shape = nullptr;
    ColorRole   fill_role = ColorRole::Accent;
    Color       fill_color = {-1.0f, -1.0f, -1.0f, -1.0f};
    float       height = 22.0f;
    bool        show_percent = true;
    bool        wave_front = false;
    bool        glint = false;
};
```

### ImageHandle

```cpp
struct ImageHandle {
    ftui::ImageHandle* gui = nullptr;
    ftti::ImageHandle* tui = nullptr;
};
```

### ContextMenuItem

```cpp
struct ContextMenuItem {
    const char* label = "";
    bool enabled = true;
    bool checked = false;
    bool separator = false;
    bool checkable = false; // Also identifies unchecked toggle actions.
};
```

### ContextMenuScope

```cpp
enum class ContextMenuScope { LastItem, Window, Region };
```

### ContextMenuRegion

```cpp
struct ContextMenuRegion { float x = 0, y = 0, width = 0, height = 0; };
```

### ContextMenuOptions

```cpp
struct ContextMenuOptions {
    bool enabled = true;
    bool right_click = true;
    bool keyboard = true; // Shift+F10
    float width = 220.0f;
    int visible_rows = 10;
    ContextMenuScope scope = ContextMenuScope::LastItem;
    ContextMenuRegion region; // Window/client coordinates for Region scope.
    float animation_seconds = 0.12f; // Fully visible at t=0; then a small overshoot.
    float overshoot = 0.025f; // Set to zero to disable the pulse.
};
```

### TableOptions

```cpp
struct TableOptions {
    int visible_rows = 8;
    bool headers = true;
    bool striped = true;
    bool sortable = true;
};
```

### TableState

```cpp
struct TableState {
    int selected_row = -1; // Source row index, even after sorting.
    int sort_column = -1;
    bool descending = false;
    int first_row = 0;
};
```

### ClipboardHandlers

```cpp
struct ClipboardHandlers {
    std::function<std::string()> read;
    std::function<bool(const char*)> write;
};
```

### ClipboardViewState

```cpp
struct ClipboardViewState { std::string text; bool loaded = false; };
```

### FileFilter

```cpp
struct FileFilter {
    const char* name; // e.g. "PNG Images"
    const char* spec; // e.g. "*.png;*.jpg"  (Windows wildcard; Linux space-sep)
};
```

### DebugState

```cpp
struct DebugState {
    bool show_layout_rects = false;
    bool show_hovered_id   = false;
    bool show_active_id    = false;
    bool show_fps          = false;
    bool log_widget_calls  = false;
};
```

Flag enums support `|` to combine flags and `&` to test them (the latter returns `bool`). `ImageHandle` owns backend handles through `load_image`/`free_image`; do not copy and free its members independently. `DebugState` is live through `ft::debug()` only for GUI rendering.
