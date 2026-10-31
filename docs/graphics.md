# Custom graphics and desktop overlays

FT exposes the graphical building blocks used by launcher interfaces such as Kalwer
and recording HUDs such as VCTTS. These are reusable drawing/window primitives;
search engines, shell sessions, games, audio, notes and other app logic stay in the
application.

## Build and run

```sh
c++ examples/ft_graphics.cpp -o build/ft_graphics -std=c++17 \
  $(pkg-config --cflags --libs cairo x11) -pthread
./build/ft_graphics
```

Linux uses GLX automatically when OpenGL development headers, `libGL.so.1`,
a compatible 32-bit alpha visual and a shader-capable context are available.
GL functions load at runtime, so existing build commands do not need `-lGL`.
On glibc older than 2.34, add `-ldl`. `FTUI_DISABLE_GPU_DITHER` excludes GL entirely;
`FTUI_GPU=0` selects the legacy backend at runtime. Query `ft::graphics_info()`;
`gpu_dither` identifies the shader path, while `renderer` distinguishes actual
hardware from a software implementation such as llvmpipe.

A compositor is required to see through an alpha window. Linux currently uses
X11/XWayland; there is no native Wayland layer-shell backend, exclusive-fullscreen
overlay guarantee, or compositor-independent stacking guarantee.

## Custom drawing

Inside `begin()`/`end()`, `canvas(id, height, callback)` allocates a normal layout
item and clips drawing to it. The callback receives a `Canvas`, its bounds and a
`FrameInput` snapshot. Coordinates are window-local logical pixels, including the
item's origin. Drawing methods include rounded fills/strokes, circles, lines,
text, text measurement, clipped scopes and premultiplied BGRA pixel frames.
`paint(callback)` uses absolute window coordinates and respects the current
widget/content clip. For a fully custom window, set the style's window padding
to zero and draw from that callback.

```cpp
ft::canvas("preview", 100, [](ft::Canvas& c, ft::Rect r, const ft::FrameInput& in) {
    c.fill(r, {.06f,.07f,.10f,.9f}, 18);
    c.text("Custom preview", {r.x+20,r.y+10,r.w-40,32}, {1,1,1,1}, {22,true});
    if (in.pressed) { /* application-owned hit testing */ }
});
```

`TextStyle` controls size, weight, family, alignment and ellipsis. Windows uses
DirectWrite. Linux uses the existing Cairo font backend; complex shaping can use
Pango through `Canvas::native_context()` (`cairo_t*`). The Windows native context
is an `ID2D1RenderTarget*`. The caller must restore native drawing state.
`pixels()` takes premultiplied BGRA8 data with an explicit stride, permitting
software game frames and externally rendered sprites without changing FT.

`FrameInput` exposes mouse press/release/hold, wheel, text, arrows, Enter/Escape,
Page Up/Down, Home/End, Ctrl/Shift/Alt, focus and elapsed frame seconds. For animated
panels or sprites, update application-owned state with `delta_seconds` and call
`request_redraw()` while animation is active. There is no forced opening animation
or dither warm-up delay.

## Launcher result rows

`ResultRow` holds a stable application ID, title, subtitle, optional image,
badge, pinned styling and enabled state. `ResultListState` holds source selection
and scroll position; applications own ranking, favourites and retention.
`result_list()` returns the activated source index, or `-1`.

Keyboard navigation skips disabled rows. `select_on_hover` enables launcher-style
hover selection. Mouse wheel, arrow keys, Page Up/Down,
Home/End and Enter work over a scrollable list. `visible_rows` controls interactive
rows; `preview_rows` adds dimmed, non-clickable rows below them. Disable
`ResultListOptions::keyboard` when a text editor needs the same navigation keys.
Images loaded through `ft::load_image()` can be supplied as `ResultRow::icon`.
TUI and web use a basic listbox fallback, without the GUI layout/preview effect.

## Shader transparency

On the Linux shader backend, `WindowTransparency::BayerDither` masks the **finished
interface**, preserving its colors, using a fragment shader. This is real alpha
coverage, not decorative colored noise on an opaque rectangle. Cairo still draws
the interface, and a BGRA texture upload occurs on each requested frame. Only the
coverage/composition step is on the GPU; this is not a GPU rewrite of every widget.
The texture's storage is reused until resize, with no CPU-generated dither tile
and no GPU readback in the production path. The UI event loop still sleeps when
idle. FT owns frame pacing and disables an extra GLX swap wait when supported.

```cpp
ft::DitherStyle dots;
dots.pattern = ft::DitherPattern::Halftone; // or Bayer
// Coverage multipliers from top to bottom, and optional caller-driven reveal.
dots.top = 1; dots.bottom = .15f; dots.reveal = 1;
ft::set_dither_style(dots);
ft::set_dither_size(8);
ft::set_window_opacity(.8f);
```

Defaults use full reveal immediately. Style/coverage changes update uniforms;
no texture regeneration or animation delay is involved. First context creation
and shader compilation still have driver-dependent startup cost. Frame latency
also depends on the desktop compositor, refresh rate and UI drawing workload.
The legacy Cairo fallback and Windows main-window backend retain their previous
decorative dither behavior. `BackdropEffect::BayerDither` remains the separate
cached decorative popup treatment; it is not the new full-interface shader.

## Independent desktop HUDs

`ft::Overlay` owns an independent native window. It works without a primary FT
window and multiple overlays can coexist without a modal child-window loop.
Defaults are borderless, topmost, absent from the taskbar, click-through and
non-activating. The active app keeps keyboard focus. The overlay does not capture
or synthesize global keyboard input; the application supplies its preview text.

```cpp
ft::Overlay hud;
ft::OverlayConfig config;
config.anchor = ft::OverlayAnchor::TopCenter;
if (!hud.create(config)) { /* handle unavailable alpha/shape support */ }
hud.poll_input();
hud.render([](ft::Canvas& c, ft::Rect r) {
    c.fill({0,0,r.w,r.h}, {.067f,.075f,.094f,.86f}, 18);
    ft::status_pill(c, {20,18,100,24}, "VOICE TTS", {.33f,.46f,1,1}, {1,1,1,1});
    c.text("Recording", {134,18,180,24}, {.64f,.99f,.8f,1}, {14,true});
    c.text("Start typing…", {20,54,r.w-40,52}, {1,1,1,1}, {22});
});
// On the next application state change: hud.render(...), hud.hide(), or hud.close().
```

Anchors cover seven work-area positions, with DPI scaling, margins and width
clamping. Windows follows the foreground app's monitor; Linux follows the pointer's
monitor via Xinerama and intersects the desktop work area when available.
`set_position()` overrides anchoring. `poll_input()` nonblockingly processes only
the overlay's own events; render it again after expose or application changes.
Set `window.click_through=false` to build an interactive canvas HUD. Use the normal
FT window for a full text editor; the overlay event snapshot is intended for
simple pointer/button/navigation handling, not IME text entry.

`status_pill()` and `waveform()` provide common recording/caption visuals.
`hide()`, `close()` and `visible()` control lifecycle; destruction closes the
window. Every overlay has its own transparency settings and graphics status.
`set_transparency()` changes coverage without rebuilding the window. Linux shader
overlays support both mask patterns, gradient and reveal. Windows overlays use
`UpdateLayeredWindow` with true per-pixel alpha and Direct2D/DirectWrite drawing;
GPU dither there is **not implemented**, and requesting it returns `false`.

Primary windows also expose `set_window_options()`, `set_window_position()`,
`set_window_visible()` and `native_window()`. All graphics/window APIs run on the
owning UI thread. Always check creation/window-option results: click-through
requires XShape support on Linux. Stacking and activation hints remain subject to
the window manager. TUI/web canvas/window helpers return unavailable; overlays
are explicitly local desktop objects and must not be created for remote web users.

## Validation

`tests/graphics.cpp` covers primitive drawing/clipping, Unicode layout fixtures,
result activation/disabled rows/previews, real shader alpha, no CPU tile creation,
independent overlays, repeated cleanup and parent/child renderer restoration.
Run from the repository root:

```sh
c++ tests/graphics.cpp -o build/test-graphics -std=c++17 \
  $(pkg-config --cflags --libs cairo x11) -pthread
./build/test-graphics                  # headless drawing and row behavior
xvfb-run -a ./build/test-graphics --gpu # GLX integration (usually software GL)
./build/test-graphics --gpu            # local desktop hardware renderer
FTUI_GPU=0 xvfb-run -a ./build/test-graphics --cpu
```

Only the test reads GPU pixels, before swap, to verify coverage. A front-buffer
read after swap is not portable across the tested GLX drivers.
