#define FT_IMPLEMENTATION
#include "../ft.hpp"
#include <cassert>
#include <chrono>

static void api_tests() {
    auto original=ft::one_dark_style();
    auto ink=ft::paper_ink_style();
    assert(ink.deep_theme==ft::DeepTheme::PaperInk && ink.rounding==0 && ink.item_height==44);
    assert(std::string(ft::style_name(ink))=="paper-ink");
    assert(ink.text.r==0 && ink.background.r==1 && ink.item_spacing==0);
    ft::set_deep_theme("paper-ink-magenta");
    assert(ft::get_style().accent.r==1 && ft::get_style().accent.b==1);
    assert(ftui::get_style().deep_theme==ftui::DeepTheme::PaperInk);
    assert(ftht::get_style().deep_theme==ftui::DeepTheme::PaperInk);
    assert(ft::get_style().font_family==ftui::get_style().font_family);
    assert(std::string(ft::current_style_name())=="paper-ink-magenta");
    assert(!ft::set_deep_theme("missing") && !ft::set_deep_theme(nullptr));
    assert(ft::get_style().deep_theme==ft::DeepTheme::PaperInk);
    // A future theme owns its callbacks and CSS after the caller goes away.
    auto custom=original;
    auto painter=std::make_shared<ft::NativeThemePainter>();
    painter->chrome=[](ftui::Canvas& c,const ft::ThemeChrome& x){
        c.fill(x.bounds,{0,.2f,.4f,1});c.stroke(x.bounds,{1,1,1,1},3,0);return true;
    };
    custom.native_painter=painter;custom.web_css="button{border-radius:0!important}";
    assert(ft::register_deep_theme("blueprint-test",custom));
    assert(!ft::register_deep_theme("blueprint-test",custom));
    assert(!ft::register_deep_theme("paper-ink",custom));
    painter.reset();custom.native_painter.reset();
    assert(ft::set_deep_theme("blueprint-test"));
    assert(ft::get_style().native_painter && ftui::get_style().native_painter);
    assert(ftht::get_style().web_css.find("border-radius")!=std::string::npos);
    auto& web=ftht::internal::ctx();web.out.clear();ftht::internal::append_css();
    assert(web.out.find("blueprint")==std::string::npos); // internal name is not UI copy
    assert(web.out.find("border-radius:0!important")!=std::string::npos);
    ft::set_style(original);
    assert(ft::get_style().deep_theme==ft::DeepTheme::Standard && !ftui::get_style().native_painter);
    assert(ftht::get_style().web_css.empty());
    web.out.clear();ftht::internal::append_css();
    assert(web.out.find("FTINKSOURCE")==std::string::npos && web.out.find("--pi-paper")==std::string::npos);
    ft::set_style(ink);web.out.clear();ftht::internal::append_css();
    assert(web.out.find("--pi-paper")!=std::string::npos && web.out.find("data:font/woff2;base64,")!=std::string::npos);
    web.out.clear();
    ftht::progress_bar(.62f,"<unsafe>",true,32);
    assert(web.out.find("&lt;unsafe&gt;")!=std::string::npos);
    assert(web.out.find("aria-valuenow=\"62.0\"")!=std::string::npos);
    // Both command paths dispatch through the facade without replacing app handlers.
    ft::detail::install_deep_theme_commands();
    std::snprintf(ftui::internal::g_cmd.buf,sizeof(ftui::internal::g_cmd.buf),"deep-theme paper-ink-magenta");
    assert(ftui::internal::apply_cmd_action());
    assert(ft::get_style().deep_theme_name=="paper-ink-magenta");
    assert(ftht::internal::execute_command(":deep-theme blueprint-test"));
    assert(ft::get_style().deep_theme_name=="blueprint-test");
    assert(std::string(ftui::internal::gui_command_completion("deep-theme blue"))=="deep-theme blueprint-test");
    assert(ftht::internal::command_completion("deep-theme blue")=="deep-theme blueprint-test");
    assert(ftht::internal::execute_command(":deep-theme does-not-exist"));
    assert(ft::get_style().deep_theme_name=="blueprint-test");
    assert(ftht::internal::execute_command(":theme paper-ink"));
    assert(ft::get_style().deep_theme_name=="paper-ink");
    assert(ftht::internal::execute_command(":deep-theme standard"));
    assert(ft::get_style().deep_theme==ft::DeepTheme::Standard);
    web.params={{"_ftht_command","deep-theme paper-ink-magenta"},{"stale-selector","0"}};
    ftht::internal::execute_posted_command_if_needed();
    assert(web.params.empty() && ft::get_style().deep_theme_name=="paper-ink-magenta");
    ft::set_style(ink);
    // Partial responses carry a new replaceable stylesheet, even restoring defaults.
    web.client_update=true;ftht::begin();
    assert(web.out.find("id=\"ftht-theme\"")!=std::string::npos);web.client_update=false;
}

#ifndef _WIN32
static void region(float x,float y,float w,float h) {
    using namespace ftui::internal;
    g_ctx={};g_ctx.content_region={x,y,w,h};g_ctx.cursor_x=x;g_ctx.cursor_y=y;
}
static std::vector<unsigned char> scene(double scale,const char* path,bool magenta=false) {
    using namespace ftui::internal;
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,int(980*scale),int(760*scale));
    cairo_surface_set_device_scale(surface,scale,scale);g_renderer.cr=cairo_create(surface);
    g_platform.width=980;g_platform.height=760;
    g_style=ftui::paper_ink_style(magenta);g_effects_enabled=false;g_drawing=true;
    g_input={};g_input.mouse_x=-100;g_input.mouse_y=-100;g_disabled_depth=0;
    fill_rect({0,0,980,760},g_style.background);
    ink_frame({20,20,940,720},2,g_style.border);
    fill_rect({21,21,938,56},g_style.border);
    draw_text_utf8("PAPER INK / FT",{36,27,700,40},{1,1,1,1});
    ink_line(20,78,960,78,2,g_style.border);
    draw_text_utf8("NATIVE C++ / CONNECTED FRAMES / 044",{36,88,900,32},g_style.text);
    region(36,142,550,560);
    char name[128]="Żółć / design workspace";
    char notes[256]="Connected black-and-white frames.\nDashed inputs await material.\nSolid rules confirm structure.";
    ftui::input("WORKSPACE",name,sizeof(name));
    ftui::text_area("NOTES",notes,sizeof(notes),4);
    bool enabled=true;ftui::checkbox("Keep local copies",&enabled);
    float amount=.62f;ftui::slider_float("AMOUNT",&amount,0,1);
    ftui::row(2,[]{ftui::button("APPLY",ftui::ColorRole::Accent);ftui::button("PREVIEW");});
    ftui::begin_disabled();ftui::button("WAITING / DISABLED");ftui::end_disabled();
    ftui::spacing(18);
    ftui::ProgressStyle progress;progress.label="PROCESSING";progress.height=32;
    ftui::progress_bar(.62f,progress);
    region(618,142,324,560);
    ftui::text("INTERACTION / STATES");
    ftui::button("READY");
    // Hover, input focus, disabled behavior use the real widget machinery.
    g_input.mouse_x=640;g_input.mouse_y=g_ctx.cursor_y+12;
    ftui::button("HOVER / INVERSE");g_input.mouse_x=-100;
    set_widget_focus(hash_str("FOCUSED INPUT"),false);
    ftui::input("FOCUSED INPUT",name,sizeof(name));
    // Focus uses the actual editor text and active-boundary renderer.
    ftui::spacing(12);
    const char* choices[]={"CONFIRMED","PENDING","ARCHIVED"};int selected=0;
    ftui::radio_group("STATE",choices,3,&selected);
    ftui::spacing(18);
    ftui::text_wrapped("Solid = confirmed\nDashed = awaiting input\nHatching = active boundary");
    draw_text_utf8("FT / PAPER INK",{36,688,550,32},g_style.text);
    draw_text_utf8(magenta?"INK + MAGENTA":"BLACK ON WHITE",{618,688,324,32},g_style.text);
    // Real mouse activation and disabled guards remain intact under a deep theme.
    region(36,720,250,44);g_input={};g_input.mouse_x=40;g_input.mouse_y=730;g_input.mouse_pressed=true;
    assert(!ftui::button("hit-test"));
    region(36,720,250,44);g_ctx.active_id=hash_str("hit-test");g_input.mouse_pressed=false;g_input.mouse_released=true;
    assert(ftui::button("hit-test"));
    region(36,720,250,44);g_ctx.active_id=hash_str("hit-test");ftui::begin_disabled();assert(!ftui::button("hit-test"));ftui::end_disabled();
    // Paint over the interaction fixture; screenshot contains only the designed scene.
    fill_rect({0,718,980,42},g_style.background);ink_line(20,718,960,718,2,g_style.border);
    assert(cairo_status(g_renderer.cr)==CAIRO_STATUS_SUCCESS);
    cairo_surface_flush(surface);
    int length=cairo_image_surface_get_stride(surface)*cairo_image_surface_get_height(surface);
    auto* data=cairo_image_surface_get_data(surface);
    std::vector<unsigned char> bytes(data,data+length);
    if(path)assert(cairo_surface_write_to_png(surface,path)==CAIRO_STATUS_SUCCESS);
    cairo_destroy(g_renderer.cr);g_renderer.cr=nullptr;cairo_surface_destroy(surface);g_drawing=false;
    return bytes;
}
#endif
int main(int argc,char** argv) {
    api_tests();
#ifndef _WIN32
    double scale=argc>2?std::atof(argv[2]):1;
    const char* path=argc>1?argv[1]:nullptr;
    const bool magenta=argc>3;
    auto first=scene(scale,path,magenta);
    auto second=scene(scale,nullptr,magenta);
    assert(first==second); // no time-dependent noise or changing seams
    using namespace ftui::internal;
    auto icon=builtin_icon_property(ftui::BuiltinIcon::Symbol);
    size_t offset=0;for(int size:{32,64,128,256}) {
        assert(icon[offset]==static_cast<unsigned>(size) && icon[offset+1]==static_cast<unsigned>(size));
        offset+=2;
        assert(icon[offset]==0); // outside the square is transparent
        bool orange=false,white=false,black=false;
        for(int i=0;i<size*size;++i){auto p=icon[offset+i];orange|=((p>>24)>200 && ((p>>16)&255)>240 && ((p>>8)&255)>100 && ((p>>8)&255)<160 && (p&255)<20);white|=p==0xfffffffful;black|=p==0xff000000ul;}
        assert(orange&&white&&black);offset+=size*size;
    }
    assert(offset==icon.size());
    assert(builtin_icon_property(ftui::BuiltinIcon::SymbolWithText)!=icon);
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,100,100);g_renderer.cr=cairo_create(surface);
    // Render the real command bar and inspect the suggestion glyph region:
    // typed ink remains black while unaccepted completion has no black pixels.
    for(bool magenta:{false,true}) {
        auto* command_surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,900,120);
        auto* previous_cr=g_renderer.cr;g_renderer.cr=cairo_create(command_surface);
        g_style=ftui::paper_ink_style(magenta);
        fill_rect({0,0,900,120},{1,1,1,1});
        g_cmd.active=true;std::snprintf(g_cmd.buf,sizeof(g_cmd.buf),"deep-theme pa");g_cmd.len=int(std::strlen(g_cmd.buf));
        draw_command_overlay(900,120);cairo_surface_flush(command_surface);
        const char* completion=gui_command_completion(g_cmd.buf);
        float typed_width=measure_text_width(":deep-theme pa");
        float suffix_width=measure_text_width(completion+std::strlen(g_cmd.buf));
        int start=int(g_style.window_padding+22+typed_width)+1;
        int end=int(g_style.window_padding+22+typed_width+suffix_width)-1;
        auto* bytes=cairo_image_surface_get_data(command_surface);int stride=cairo_image_surface_get_stride(command_surface);
        int gray=0,black=0;
        float lh=text_line_height();int top=int(120-g_style.window_padding-(lh+16)+8);
        for(int y=top;y<int(top+lh);++y)for(int x=start;x<end;++x) {
            auto* p=bytes+y*stride+x*4;
            if(p[0]<240 && p[1]<240 && p[2]<240) {
                gray+=p[0]>90 && p[1]>90 && p[2]>90;
                black+=p[0]<40 && p[1]<40 && p[2]<40;
            }
        }
        assert(gray>10 && black==0);
        g_cmd={};cairo_destroy(g_renderer.cr);cairo_surface_destroy(command_surface);g_renderer.cr=previous_cr;
    }
    int painted=0;
    auto custom=ft::one_dark_style();auto hooks=std::make_shared<ft::NativeThemePainter>();
    hooks->chrome=[&](ftui::Canvas& c,const ft::ThemeChrome& x){++painted;c.fill(x.bounds,{0,1,0,1});return true;};
    custom.native_painter=hooks;ft::set_style(custom);
    draw_widget_chrome({2,2,80,80},0,{1,1,1,1},{0,0,0,1},0,0,0);
    assert(painted==1);cairo_surface_flush(surface);
    auto* pixel=cairo_image_surface_get_data(surface)+20*cairo_image_surface_get_stride(surface)+20*4;
    assert(pixel[1]==255 && pixel[0]==0 && pixel[2]==0);
    ft::set_style(ft::one_dark_style());
    draw_widget_chrome({2,2,80,80},0,{1,1,1,1},{0,0,0,1},0,0,0);assert(painted==1);
    cairo_destroy(g_renderer.cr);g_renderer.cr=nullptr;cairo_surface_destroy(surface);
#endif
    std::puts("Deep themes: registry, round trips, custom painter, CSS reset, input guards, deterministic rendering passed.");
}
