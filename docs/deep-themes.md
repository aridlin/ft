# Deep themes

Deep themes change native GUI painting and embedded web presentation while keeping the existing FT widgets and interaction code. Terminal retains its regular renderer and normal Style color conversion; it has no deep-theme painter or registry integration of its own.

## Use Paper Ink

After creating a window:

```cpp
ft::set_deep_theme("paper-ink");
// Optional magenta accents:
ft::set_deep_theme("paper-ink-magenta");
// Return to a normal color preset:
ft::set_style(ft::one_dark_style());
```

`set_deep_theme(const char*)` returns false for an unknown/null name and leaves the active theme unchanged. `paper-ink`, `paper-ink-magenta`, and `standard` are reserved built-in names; `standard` selects One Dark. The enum overload also accepts `ft::DeepTheme::PaperInk` and `ft::DeepTheme::Standard`.

For customization, use `auto style = ft::paper_ink_style();`, change its fields, and pass it to `ft::set_style(style)`. Style owns its font-family, name, and CSS strings. Its native painter uses shared ownership. Ordinary presets carry no custom painter or CSS, so applying one resets all deep-theme features.

The native port supplies white paper, black ink, square connected controls, 44-pixel control height, 2-pixel rules, dashed input borders, inverse buttons, hatched progress and active boundaries, plus deterministic edge displacement and feathering. `style.ink_roughness` controls the displacement in logical pixels, clamped to 0–1. It never changes randomly between frames. Regular disabled states and hit boxes still use FT's existing widget code.

Direct backend users can call `ftui::set_style(ftui::paper_ink_style())` or `ftht::set_style(ftht::paper_ink_style())`. The facade propagates its Style to native GUI and embedded web rendering, including web handoff. A custom facade registry is selected with `set_deep_theme`, not `style_from_name`.

## Command bar

In GUI and embedded web mode, click outside a text field and press `:`. Type `deep-theme paper-ink`, `deep-theme paper-ink-magenta`, or `deep-theme standard`, then Enter. Tab completes names, including future themes registered with `register_deep_theme`. `theme paper-ink` is also accepted. Unknown names leave the active theme unchanged. Commands are session-local.

The command router is part of FT and keeps native/web styles and facade caches synchronized. Application command handlers do not have to forward theme commands. A web command does not replay stale form controls over the new theme. Terminal has no deep-theme commands.

The expanded `ft_paper_ink` demo has Workspace, Controls, Data/Logs and Theme Lab tabs, and registers `blueprint-demo` as an example custom painter. Its native window is 1360 by 1000 pixels.

## Register a future theme

Register on the UI thread, before selecting the theme. Registrations are snapshots; duplicate names and empty/reserved names return false without mutation. Keep persistent widget state in application code, outside drawing callbacks.

```cpp
auto theme = ft::one_dark_style();
theme.rounding = 0;
theme.font_family = "monospace";
auto painter = std::make_shared<ft::NativeThemePainter>();
painter->chrome = [](ftui::Canvas& canvas, const ft::ThemeChrome& item) {
    auto r = item.bounds;
    canvas.fill(r, item.fill, 0);
    canvas.stroke(r, item.border, item.focus > .5f ? 3.f : 1.f, 0);
    return true;
};
theme.native_painter = painter;
theme.web_css = R"CSS(
    button,input,textarea,select { border-radius:0; box-shadow:none; }
)CSS";
if (ft::register_deep_theme("my-blueprint", theme))
    ft::set_deep_theme("my-blueprint");
```

Hooks return true when they paint the surface, or false to use the built-in painter. `Canvas` exposes raw drawing methods, so drawing in a hook does not recursively invoke that hook. Preserve the current clip; use `Canvas::clipped` for a temporary clip. Chrome receives the current fill/border, hover/active/focus progress, input-boundary state, enabled state, and actual widget bounds. Hooks are painting callbacks: do not mutate focus, register widgets, or change the theme from inside one.

| Hook | Signature after `Canvas&` | Purpose |
| --- | --- | --- |
| `chrome` | `const ThemeChrome&` | Complete standard widget/popup shell |
| `fill` | `Rect, float radius, Color` | Filled rounded surfaces |
| `frame` | `Rect, float radius, float thickness, Color` | Outlined surfaces |
| `progress` | `Rect, float fraction, Color` | Progress track; FT retains its label |
| `button_text` | `Color fill, Color default_text` (no Canvas argument) | Return the foreground for custom button fills |

Color, font size, spacing, border width, and rounding remain normal Style fields. Set these as well as painter callbacks when a future theme needs different metrics. Custom web CSS is trusted application code and is appended after FT's base CSS. It receives the usual `.ft-*` widget markup, including `.ft-progress`, `.ft-progress-fill`, and semantic progressbar values. Fetch updates replace a single `style#ftht-theme` stylesheet, including when a control selects a theme during that request. Rules from the previous theme are removed.

## Font and source

Paper Ink's website `@font-face` is FantasqueSansM Nerd Font Propo Regular. The full original WOFF2 and a TTF conversion are in `themes/paper-ink/`; the original font license is included. Native platforms use the installed family. Install the bundled TTF if that family is missing; otherwise the platform substitutes an available font. No fonts are installed globally by this library. Native drawing does not need CSS, a browser, or a web view.

The embedded web theme reuses the original v2 CSS from `https://files.aridlin.pl/v2/paper-ink.css`, including its frame primitives and comments. The font is embedded in the header as a WOFF2 subset covering Latin (including Polish), Greek, Cyrillic, punctuation, mathematical symbols, and geometric marks. It loads without a CDN or an external asset request. Font licensing is preserved in the embedded font metadata and `themes/paper-ink/FONT-LICENSE.txt`.

The local OpenDesign installation found during this port contained the earlier Blueprint OS design, rather than standalone Paper Ink documentation. The exact deployed Paper Ink CSS and its matching local font were used for this port. `themes/paper-ink/reference.css` preserves the original stylesheet. Native edge rendering approximates the browser's turbulence filter with C++ strokes; it is not a pixel-for-pixel SVG filter implementation.

## Demo and checks

```sh
c++ examples/ft_paper_ink.cpp -std=c++17 $(pkg-config --cflags --libs cairo x11) -pthread -o build/ft_paper_ink
./build/ft_paper_ink --gui
./build/ft_paper_ink --web # localhost:18764

c++ tests/deep_theme.cpp -std=c++17 $(pkg-config --cflags --libs cairo x11) -pthread -o build/deep_theme
./build/deep_theme build/paper-ink.png 1
./build/deep_theme build/paper-ink-150.png 1.5
./build/deep_theme build/paper-ink-200.png 2
node tests/deep_theme_web.js # requires Playwright and its browser
```

Headless native checks exercise the registry, conversion lifetimes, callback dispatch, CSS restoration, mouse activation, disabled controls, and byte-identical rendering. Browser checks cover theme round trips, embedded font loading, dark browser settings, input preservation, modal controls, disabled buttons, progress and mobile width. Windows is cross-compiled; physical Windows rendering is not validated by those checks.
