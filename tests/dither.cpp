#define FT_IMPLEMENTATION
#include "../ft.hpp"
#include <cassert>
#include <limits>

int main() {
    using namespace ftui::internal;
    std::vector<unsigned char> pixels, repeat;
    int width=0,height=0,w2=0,h2=0;
    g_style=ftui::one_dark_style();
    for(int motif:{1,4,7,16,32}) for(float scale:{1.f,1.5f,2.f,4.f}) {
        build_soft_dither_tile(motif,pixels,width,height,scale);
        assert(width>=256*scale && width==height && width%8==0);
        assert(width<=1152 && pixels.size()==size_t(width)*height*4);
        size_t coverage=0;
        for(size_t i=0;i<pixels.size();i+=4) {
            for(int c=0;c<3;++c)assert(pixels[i+c]<=pixels[i+3]);
            coverage+=pixels[i+3]!=0;
        }
        assert(coverage>size_t(width)*height/5 && coverage<size_t(width)*height/2);
        build_soft_dither_tile(motif,repeat,w2,h2,scale);
        assert(pixels==repeat); // Stable frames; no animated noise/flicker.
    }
    build_soft_dither_tile(4,pixels,width,height);
    bool repeats_at_64=true;
    for(int y=0;y<height && repeats_at_64;++y) for(int x=0;x<width-64;++x) {
        size_t i=(size_t(y)*width+x)*4;
        if(std::memcmp(pixels.data()+i,pixels.data()+i+64*4,4)!=0){repeats_at_64=false;break;}
    }
    assert(!repeats_at_64);
    build_soft_dither_tile(-9,pixels,width,height,std::numeric_limits<float>::quiet_NaN());
    build_soft_dither_tile(1,repeat,w2,h2,1);
    assert(pixels==repeat);
#ifndef _WIN32
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,640,480);
    g_renderer.cr=cairo_create(surface);
    g_dither_size=4;
    assert(ensure_dither_pattern());
    auto* cached=cairo_pattern_reference(g_dither_pattern);
    assert(ensure_dither_pattern() && cached==g_dither_pattern);
    cairo_surface_set_device_scale(surface,2,2);
    assert(ensure_dither_pattern() && cached!=g_dither_pattern);
    assert(g_dither_cache_scale==2 && cairo_image_surface_get_width(g_dither_surface)==512);
    cairo_pattern_destroy(cached);
    cached=cairo_pattern_reference(g_dither_pattern);
    g_style.accent.r=.91f;
    assert(ensure_dither_pattern() && cached!=g_dither_pattern);
    cairo_pattern_destroy(cached);
    g_style.rounding=20;
    draw_dither_backdrop_panel({20,20,160,100},1);
    cairo_surface_flush(surface);
    auto* data=cairo_image_surface_get_data(surface);
    int stride=cairo_image_surface_get_stride(surface);
    // The grain and panel agree on the rounded shape (top-left corner is clear).
    assert(data[40*stride+40*4+3]==0);
    assert(data[100*stride+100*4+3]>0);
    release_dither_cache();cairo_destroy(g_renderer.cr);g_renderer.cr=nullptr;cairo_surface_destroy(surface);
#endif
    std::puts("Dither resolution, stability, premultiplication, cache, and clipping tests passed.");
}
