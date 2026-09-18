// Optional benchmark; compare headers with -DFT_TEST_HEADER='"..."'.
#define FT_IMPLEMENTATION
#ifndef FT_TEST_HEADER
#define FT_TEST_HEADER "../ft.hpp"
#endif
#include FT_TEST_HEADER
#include <chrono>
#include <cstdio>

int main() {
    ft::detail::active_mode_ref()=ft::Mode::Web;
    ft::TableRows rows;rows.reserve(20000);
    for(int i=0;i<20000;++i)rows.push_back({std::to_string((i*7919)%20000),"Ready"});
    ft::TableState state;state.sort_column=0;
    constexpr int frames=60;
    auto start=std::chrono::steady_clock::now();
    for(int i=0;i<frames;++i){ftht::internal::ctx().out.clear();ft::table("Benchmark",{"Name","Status"},rows,state);}
    double elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    std::printf("20,000-row web table: %.3f ms/frame (%d frames)\n",elapsed/frames,frames);
#ifndef _WIN32
    using namespace ftui::internal;
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,800,600);
    g_renderer.cr=cairo_create(surface);g_dither_size=4;
    auto cold=std::chrono::steady_clock::now();ensure_dither_pattern();
    double cold_ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-cold).count();
    start=std::chrono::steady_clock::now();
    for(int i=0;i<100000;++i)ensure_dither_pattern();
    elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    std::printf("Dither: %.3f ms initial generation; %.3f us/cached lookup\n",cold_ms,elapsed*1000/100000);
    release_dither_cache();cairo_destroy(g_renderer.cr);g_renderer.cr=nullptr;cairo_surface_destroy(surface);
#endif
}
