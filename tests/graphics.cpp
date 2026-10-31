#define FT_IMPLEMENTATION
#include "../ft.hpp"
#include <cassert>
#include <chrono>
#include <limits>

#ifndef _WIN32
static void canvas_checks(){
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,420,240);
    auto* cr=cairo_create(surface);ft::Canvas c(cr);
    c.fill({0,0,420,240},{.1f,.1f,.1f,1});
    c.fill({10,10,80,40},{.9f,.3f,.1f,1},10);
    c.clipped({10,70,40,40},[](ft::Canvas& c){c.fill({0,0,200,200},{0,1,0,1});});
    c.circle(120,90,18,{.3f,.5f,1,1});
    c.text("Zażółć gęślą jaźń — long text with clipping",{10,125,250,30},{1,1,1,1},{18,true});
    float values[]={0,.2f,-.5f,1,std::numeric_limits<float>::quiet_NaN()};
    ft::waveform(c,{270,120,100,60},values,5,{.3f,.7f,1,1});
    ft::status_pill(c,{275,20,120,28},"RECORDING",{.3f,.4f,1,1},{1,1,1,1});
    assert(c.text_width("Zażółć")>20);
    cairo_surface_flush(surface);auto* data=cairo_image_surface_get_data(surface);int stride=cairo_image_surface_get_stride(surface);
    assert(data[80*stride+20*4+1]==255);assert(data[80*stride+65*4+1]<50); // clip contained
    assert(cairo_status(cr)==CAIRO_STATUS_SUCCESS);
    assert(cairo_surface_write_to_png(surface,"build/graphics-canvas.png")==CAIRO_STATUS_SUCCESS);
    cairo_destroy(cr);cairo_surface_destroy(surface);
}
static void list_checks(){
    using namespace ftui::internal;
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,640,480);
    auto* cr=cairo_create(surface);g_renderer.cr=cr;g_drawing=true;g_style=ftui::one_dark_style();
    g_ctx={};g_ctx.content_region={0,0,600,480};g_input={};
    std::vector<ftui::ResultRow> rows={{"a","Application","Description","Pinned",nullptr,true},{"b","Disabled","","",nullptr,false,false},{"c","Calculator","1 + 2",""},{"d","File","Document",""}};
    ft::ResultListOptions options;options.visible_rows=2;options.preview_rows=1;
    ft::ResultListState state;state.selected=0;
    g_input.key_down=true;ftui::result_list("rows",rows,state,options);assert(state.selected==2&&state.first==1);
    g_ctx.cursor_y=0;g_input={};g_input.key_enter=true;
    assert(ftui::result_list("rows",rows,state,options)==2);
    // Clicking the lower preview cannot activate it.
    g_ctx.cursor_y=0;g_input={};g_input.mouse_x=20;g_input.mouse_y=145;g_input.mouse_pressed=true;
    assert(ftui::result_list("rows",rows,state,options)==-1);assert(state.selected==2);
    // Empty input, invalid selection, and disabled-only rows are harmless.
    std::vector<ftui::ResultRow> empty;ftui::result_list("rows",empty,state,options);assert(state.selected==-1);
    for(auto& row:rows)row.enabled=false;g_input={};state.selected=999;
    ftui::result_list("rows",rows,state,options);assert(state.selected==-1);
    g_drawing=false;g_renderer.cr=nullptr;cairo_destroy(cr);cairo_surface_destroy(surface);
}
static void integration(bool expect_gpu){
    ft::Config cfg;cfg.mode=ft::Mode::Gui;cfg.width=360;cfg.height=240;cfg.fps_limit=0;cfg.enable_effects=false;
    cfg.window_transparency=ft::WindowTransparency::BayerDither;cfg.window_opacity=.5f;
    assert(ft::create_window(cfg));auto info=ft::graphics_info();
    std::printf("renderer=%s shader=%d\n",info.renderer.c_str(),info.gpu_dither);
    if(expect_gpu)assert(info.gpu_dither);
    std::vector<double> times;
    for(int frame=0;frame<40;++frame){auto start=std::chrono::steady_clock::now();
        ft::request_redraw();assert(ft::pump());ft::begin();
        ft::canvas("custom",100,[](ft::Canvas& c,ft::Rect r,const ft::FrameInput&){c.fill(r,{.1f,.3f,.8f,1},12);c.text("FT GPU transparency",r,{1,1,1,1},{18,true});});
        ft::end();times.push_back(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count());
    }
    if(expect_gpu){
#if FTUI_HAS_GLX
        auto& g=ftui::internal::g_renderer.gpu;
        g.present(ftui::internal::g_platform.display,ftui::internal::g_platform.window,
            ftui::internal::g_renderer.back_surf,360,240,ft::WindowTransparency::BayerDither,.5f,4,0,{},false);
        g.glReadBuffer(GL_BACK);
        std::vector<unsigned char> pixel(360*240*4);g.glReadPixels(0,0,360,240,GL_RGBA,GL_UNSIGNED_BYTE,pixel.data());
        int solid=0,clear=0;for(size_t i=3;i<pixel.size();i+=4){solid+=pixel[i]>250;clear+=pixel[i]==0;}
        std::printf("frame alpha: solid=%d transparent=%d\n",solid,clear);assert(solid>30000&&clear>30000);
        assert(ftui::internal::g_dither_surface==nullptr); // GPU never builds the CPU tile.
        GLuint texture=g.texture;ft::DitherStyle zero;zero.reveal=0;
        g.present(ftui::internal::g_platform.display,ftui::internal::g_platform.window,
            ftui::internal::g_renderer.back_surf,360,240,ft::WindowTransparency::BayerDither,1,8,0,zero,false);
        g.glReadPixels(0,0,360,240,GL_RGBA,GL_UNSIGNED_BYTE,pixel.data());
        for(size_t i=3;i<pixel.size();i+=4)assert(pixel[i]==0);
        ft::DitherStyle dots;dots.pattern=ft::DitherPattern::Halftone;dots.top=1;dots.bottom=0;
        g.present(ftui::internal::g_platform.display,ftui::internal::g_platform.window,
            ftui::internal::g_renderer.back_surf,360,240,ft::WindowTransparency::BayerDither,1,8,0,dots,false);
        g.glReadPixels(0,0,360,240,GL_RGBA,GL_UNSIGNED_BYTE,pixel.data());
        size_t upper=0,lower=0;for(int y=0;y<240;++y)for(int x=0;x<360;++x){auto alpha=pixel[(y*360+x)*4+3];if(y<120)lower+=alpha;else upper+=alpha;}
        assert(upper>lower&&g.texture==texture);
#endif
    }
    ft::WindowOptions window;window.borderless=true;window.topmost=true;
    assert(ft::set_window_options(window));ft::set_window_visible(false);ft::set_window_visible(true,false);
    // Separate HUDs coexist with the GUI and each other, with actual empty input shapes.
    ft::Overlay a,b;ft::OverlayConfig config;config.width=420;config.height=100;
    assert(a.create(config));config.anchor=ft::OverlayAnchor::BottomRight;assert(b.create(config));
    auto draw=[](ft::Canvas& c,ft::Rect r){c.fill({0,0,r.w,r.h},{.05f,.07f,.12f,.85f},18);ft::status_pill(c,{20,20,100,24},"VOICE TTS",{.3f,.4f,1,1},{1,1,1,1});c.text("Recording — preview",{140,12,r.w-155,40},{1,1,1,1});};
    a.poll_input();b.poll_input();assert(a.render(draw));assert(b.render(draw));assert(a.visible()&&b.visible());
    // Verify native input behavior, not just the requested configuration.
    auto* display=ftui::internal::g_platform.display;Window overlay_window=(Window)(uintptr_t)a.native_window();
    XWMHints* hints=XGetWMHints(display,overlay_window);assert(hints&&(hints->flags&InputHint)&&!hints->input);XFree(hints);
    void* ext=dlopen("libXext.so.6",RTLD_LAZY|RTLD_LOCAL);
    using Shape=XRectangle*(*)(Display*,Window,int,int*,int*);auto get_shape=(Shape)dlsym(ext,"XShapeGetRectangles");
    assert(get_shape);int count=-1,ordering=-1;auto* shape=get_shape(display,overlay_window,2,&count,&ordering);
    assert(count==0);if(shape)XFree(shape); // empty ShapeInput actually passes clicks through
    // Retain the library: its Xlib hooks must survive display destruction.
    static void* shape_library=ext;(void)shape_library;
    if(expect_gpu){ft::DitherStyle dots;dots.pattern=ft::DitherPattern::Halftone;dots.bottom=.1f;
        assert(a.set_transparency(ft::WindowTransparency::BayerDither,.8f,dots));assert(a.render(draw));
        config.transparency=ft::WindowTransparency::BayerDither;ft::Overlay third;assert(third.create(config));assert(third.render(draw));}
    a.hide();assert(!a.visible());assert(b.visible());a.close();a.close();b.close();
    // Child windows exercise save/restore of the parent GLX resource ownership.
    ft::Config child=cfg;child.width=180;child.height=100;
    ft::open_child_window(child,[]{ft::text("child");ftui::internal::g_platform.running=false;});
    ft::request_redraw();assert(ft::pump());ft::begin();ft::text("parent restored");ft::end();
    std::sort(times.begin()+1,times.end());std::printf("frame first=%.2f ms median=%.2f ms p95=%.2f ms\n",times[0],times[20],times[38]);
    ft::shutdown();
}
#endif
int main(int argc,char** argv){
#ifndef _WIN32
    canvas_checks();list_checks();if(argc>1)integration(std::strcmp(argv[1],"--gpu")==0);
#else
    (void)argc;(void)argv;
#endif
    std::puts("Graphics checks passed.");
}
