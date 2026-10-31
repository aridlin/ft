// Build with the same command as every other FT example (C++17, Cairo/X11).
#define FT_IMPLEMENTATION
#include "../ft.hpp"
#include <chrono>

int main(int argc,char** argv){
    bool capture=argc>1&&std::strcmp(argv[1],"--capture")==0;
    ft::Config config;config.mode=ft::Mode::Gui;config.title="FT / Launcher graphics + desktop HUD";
    config.width=650;config.height=790;config.window_transparency=ft::WindowTransparency::BayerDither;
    config.window_opacity=.96f;config.enable_effects=false;
    if(!ft::create_window(config))return 1;
    ft::WindowOptions window;window.borderless=true;ft::set_window_options(window);
    ft::DitherStyle dots;dots.pattern=ft::DitherPattern::Halftone;ft::set_dither_style(dots);
    ft::Overlay overlay;ft::OverlayConfig hud;hud.margin=40;
    if(!overlay.create(hud))return 2;
    std::vector<ft::ResultRow> rows={
        {"editor","Text editor","Applications / A place to write","Pinned",nullptr,true},
        {"terminal","Terminal","Interactive command popup","Enter"},
        {"calculator","3 / 2","Decimal 1.5 · fraction 3/2 · 150%","Copy"},
        {"file","Design notes","Documents / project / notes.txt","File"},
        {"settings","Appearance","Colors, density, timing and layout","Settings"},
        {"preview1","Clipboard history","Preview rows have no hit target",""},
        {"preview2","Unicode lookup","Glyph preview, detail and badge",""},
        {"preview3","Custom mini game","Draw sprites, circles, lines or pixel frames",""}
    };
    ft::ResultListState state;state.selected=0;
    ft::ResultListOptions options;options.preview_rows=2;
    char query[256]="";float opacity=.96f;bool show_overlay=true;
    int frames=0;
    while(ft::pump()){
        ft::begin();
        ft::canvas("heading",32,[](ft::Canvas& c,ft::Rect r,const ft::FrameInput&){
            ft::status_pill(c,{r.x,r.y,110,24},"FT GRAPHICS",{.31f,.42f,.78f,1},{1,1,1,1});
            ft::TextStyle label;label.size=12;label.align=ft::Align::End;
            c.text("CUSTOM CANVAS / LIVE SHADER",{r.x+140,r.y,r.w-140,24},{.65f,.7f,.8f,1},label);
        });
        bool submitted=false;ft::input("Search",query,sizeof(query),ft::InputFlags::Default,&submitted);
        if(ft::result_list("Results",rows,state,options)>=0)ft::toast_success("Row activated");
        ft::slider_float("Dither coverage",&opacity,.2f,1.f);ft::set_window_opacity(opacity);
        if(ft::checkbox("Show click-through recording overlay",&show_overlay)&&!show_overlay)overlay.hide();
        ft::text(ft::graphics_info().renderer.c_str());
        ft::end();
        if(show_overlay){overlay.poll_input();overlay.render([&](ft::Canvas& c,ft::Rect r){
            c.fill({.5f,.5f,r.w-1,r.h-1},{.067f,.075f,.094f,.86f},18);
            c.stroke({.5f,.5f,r.w-1,r.h-1},{.34f,.39f,.49f,.75f},1,18);
            ft::status_pill(c,{20,18,100,24},"VOICE TTS",{.33f,.46f,1,1},{1,1,1,1});
            c.text("Recording",{134,18,160,24},{.64f,.99f,.8f,1},{14,true});
            ft::TextStyle hint;hint.size=13;hint.align=ft::Align::End;
            c.text("Enter speak / Esc cancel",{r.w-230,18,210,24},{.78f,.81f,.87f,.9f},hint);
            c.text(query[0]?query:"Start typing…",{20,54,r.w-140,52},{.97f,.98f,1,1},{22});
            float samples[18];for(int i=0;i<18;++i)samples[i]=.2f+.65f*std::fabs(std::sin(i*.7f));
            ft::waveform(c,{r.w-110,64,86,30},samples,18,{.64f,.99f,.8f,1},2);
        });}
        if(ft::frame_input().escape)break;
        if(capture&&++frames==3){
#ifndef _WIN32
            // Export the finished content texture for a portable layout fixture.
            cairo_surface_write_to_png(cairo_get_target(ftui::internal::g_renderer.cr),"build/graphics-showcase.png");
            auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,720,128);auto* cr=cairo_create(surface);
            ft::Canvas canvas(cr);canvas.fill({0,0,720,128},{.067f,.075f,.094f,.86f},18);
            ft::status_pill(canvas,{20,18,100,24},"VOICE TTS",{.33f,.46f,1,1},{1,1,1,1});
            canvas.text("Recording",{134,18,160,24},{.64f,.99f,.8f,1},{14,true});
            canvas.text("Start typing…",{20,54,680,52},{.97f,.98f,1,1},{22});
            cairo_surface_write_to_png(surface,"build/graphics-overlay.png");cairo_destroy(cr);cairo_surface_destroy(surface);
#endif
            break;
        }
    }
    overlay.close();ft::shutdown();
}
