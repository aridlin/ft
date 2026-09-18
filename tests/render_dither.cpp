// Headless Cairo visual fixture; compile with -DFT_TEST_HEADER='"path/to/ft.hpp"'
// to compare an older header using the exact same scene.
#define FT_IMPLEMENTATION
#ifndef FT_TEST_HEADER
#define FT_TEST_HEADER "../ft.hpp"
#endif
#include FT_TEST_HEADER
#include <cassert>

int main(int argc, char** argv) {
#ifndef _WIN32
    const char* output = argc > 1 ? argv[1] : "build/dither.png";
    double scale = argc > 2 ? std::atof(argv[2]) : 1.0;
    int motif = argc > 3 ? std::atoi(argv[3]) : 4;
    assert(scale >= 1 && scale <= 4);
    using namespace ftui::internal;
    auto* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, int(840*scale), int(560*scale));
    cairo_surface_set_device_scale(surface,scale,scale);
    g_renderer.cr = cairo_create(surface);
    g_platform.width = 840; g_platform.height = 560;
    g_style = ftui::one_dark_style(); g_style.rounding = 12;
    g_effects_enabled = false; g_dither_size = motif;
    fill_rect({0,0,840,560}, g_style.background);
    draw_dither_backdrop_panel({20,20,800,520},1);
    draw_text_utf8("FT / Surface detail", {46,38,600,40}, g_style.text);
    draw_text_utf8("Dither should add texture without making the interface look pixelated.",
                   {46,80,740,28},g_style.text_dim);
    g_ctx = {}; g_ctx.content_region={46,132,748,310}; g_ctx.cursor_x=46; g_ctx.cursor_y=132;
    g_drawing = true;
    char name[64]="Design workspace"; bool enabled=true; float amount=.65f;
    ftui::input("Workspace",name,sizeof(name));
    ftui::checkbox("Keep the interface crisp",&enabled);
    ftui::slider_float("Opacity",&amount,0,1);
    ftui::row(2,[]{ftui::button("Apply",ftui::ColorRole::Accent);ftui::button("Cancel");});
    draw_text_utf8("Cached texture / native-size controls / no external assets",{46,488,740,28},g_style.text_dim);
    assert(cairo_status(g_renderer.cr)==CAIRO_STATUS_SUCCESS);
    assert(cairo_surface_write_to_png(surface,output)==CAIRO_STATUS_SUCCESS);
    release_dither_cache(); cairo_destroy(g_renderer.cr);g_renderer.cr=nullptr;cairo_surface_destroy(surface);
    g_drawing=false;
#else
    (void)argc; (void)argv;
#endif
}
