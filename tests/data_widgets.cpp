#define FT_IMPLEMENTATION
#include "../ft.hpp"
#include <cassert>

int main(int argc, char** argv) {
#ifndef _WIN32
    if (argc>1 && std::strcmp(argv[1],"--gui")==0) {
        ft::Config config; config.mode=ft::Mode::Gui; config.width=900; config.height=640;
        config.title="FT data widgets - render check"; config.enable_effects=false;
        if(!ft::create_window(config)) return 2;
        for(int frame=0;frame<3;++frame) {
            ft::request_redraw();
            if(!ft::pump()) return 3;
            ft::begin();
            ftui::internal::g_effects_enabled=true;
            ftui::internal::g_dt=0.06f;
            ft::TableState selected; selected.selected_row=0; selected.sort_column=0;
            ft::TableOptions options; options.visible_rows=3;
            ft::table("Projects",{"Name","Language","State"},{{"ft","C++","Ready"},{"Unicode","UTF-8","Zażółć"}},selected,options);
            ft::code_view("Source", "// Clipboard preview\nint main() {\n    return 0;\n}",5);
            ft::ContextMenuItem menu[]={{"Copy row"},{"Unavailable",false},{"",true,false,true},{"Striped rows",true,true,false,true}};
            ft::ContextMenuOptions anywhere; anywhere.scope=ft::ContextMenuScope::Window;
            if(frame==0) {
                ftui::internal::g_input.context_pressed=true;
                ftui::internal::g_input.mouse_x=550; ftui::internal::g_input.mouse_y=220;
            }
            ft::context_menu("Actions",menu,4,anywhere);
            ft::end();
            std::string file="build/data-widgets-gui-"+std::to_string(frame)+".png";
            cairo_surface_write_to_png(cairo_get_target(ftui::internal::g_renderer.cr),file.c_str());
        }
        ft::shutdown();
        std::puts("GUI render passed.");
        return 0;
    }
#endif
    ft::TableRows data={{"b","2"},{"a","1"},{"a"},{"c","3"}};
    ft::TableState state; state.sort_column=0; state.selected_row=0;
    auto order=ftui::table_order(data,2,state);
    assert((order==std::vector<int>{1,2,0,3}));
    assert(state.selected_row==0);
    state.descending=true;
    assert((ftui::table_order(data,2,state)==std::vector<int>{3,0,1,2}));
    state.sort_column=1; state.descending=false;
    assert(ftui::table_order(data,2,state)[0]==2);
    state.selected_row=400; state.sort_column=400;
    ftui::table_order(data,2,state);
    assert(state.selected_row==-1 && state.sort_column==-1);

    ft::detail::active_mode_ref()=ft::Mode::Tui;
    std::string clipboard;
    ft::set_clipboard_handlers(ft::Mode::Tui,{
        [&]{return clipboard;},[&](const char* v){clipboard=v; return true;}});
    assert(ft::set_clipboard_text("Zażółć\nint main() {}"));
    assert(ft::get_clipboard_text()=="Zażółć\nint main() {}");
    ft::detail::active_mode_ref()=ft::Mode::Web;
    assert(!ft::clipboard_can_read() && !ft::clipboard_can_write());
    assert(ft::get_clipboard_text().empty());
    assert(!ft::set_clipboard_text("must not touch host"));
    auto& web=ftht::internal::ctx(); web.out.clear();
    ft::TableOptions opts; opts.visible_rows=2;
    ft::table("Data", {"<Name>","Value"},{{"<script>","a&b"}},state,opts);
    assert(web.out.find("<table")!=std::string::npos);
    assert(web.out.find("&lt;script&gt;")!=std::string::npos);
    assert(web.out.find("<script>")==std::string::npos);
    ft::ContextMenuItem web_menu[]={{"Toggle",true,true,false,true},{"Disabled",false},{"Another"}};
    ft::ContextMenuOptions web_opts; web_opts.scope=ft::ContextMenuScope::Window;
    ft::context_menu("Everywhere",web_menu,3,web_opts);
    assert(web.out.find("[x]")==std::string::npos);
    assert(web.out.find("<svg")!=std::string::npos);
    assert(web.out.find("data-ft-context")!=std::string::npos);
    std::ofstream("build/data-widgets-web.html")<<web.out;
    ft::ClipboardViewState view;
    ft::clipboard_view("<Clipboard>",view);
    assert(web.out.find("navigator.clipboard.readText()")!=std::string::npos);
    assert(web.out.find("&lt;Clipboard&gt;")!=std::string::npos);

#ifndef _WIN32
    using namespace ftui::internal;
    auto* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,640,480);
    g_renderer.cr=cairo_create(surface);
    g_ctx={}; g_ctx.content_region={0,0,640,480}; g_drawing=true;
    g_input={}; g_ctx.last_item_rect={10,10,150,36}; g_ctx.last_item_hovered=true;
    g_input.context_pressed=true; g_input.mouse_x=630; g_input.mouse_y=470;
    ft::ContextMenuItem items[]={{"disabled",false},{"separator",true,false,true},{"Copy"},{"checked",true,true}};
    assert(ftui::context_menu("menu",items,4)==-1);
    assert(g_dropdown_overlay.active);
    assert(g_dropdown_overlay.popup_r.x+g_dropdown_overlay.popup_r.w<=640);
    assert(g_dropdown_overlay.popup_r.y+g_dropdown_overlay.popup_r.h<=480);
    draw_dropdown_overlay();
    g_dropdown_overlay_prev=g_dropdown_overlay;
    g_input={}; g_input.key_down=true;
    ftui::context_menu("menu",items,4);
    assert(g_dropdown_overlay.selected_index==2);
    g_dropdown_overlay_prev=g_dropdown_overlay;
    g_input={}; g_input.key_enter=true;
    assert(ftui::context_menu("menu",items,4)==2);
    assert(!g_dropdown_open_id && !g_input.key_enter);
    g_input={};
    ftui::context_menu("menu",items,4,{},true);
    g_dropdown_overlay_prev=g_dropdown_overlay;
    g_input.mouse_pressed=true; g_input.mouse_x=0; g_input.mouse_y=0;
    assert(ftui::context_menu("menu",items,4)==-1);
    assert(!g_dropdown_open_id && !g_input.mouse_pressed);
    g_dropdown_capture_input=false; g_input={};
    g_ctx.cursor_x=0; g_ctx.cursor_y=0;
    state={};
    ftui::table("Table",{"Name","Value"},data,state,opts);
    auto body=g_ctx.last_item_rect;
    g_ctx.cursor_x=0; g_ctx.cursor_y=0;
    g_input.mouse_x=20; g_input.mouse_y=body.y+body.h-10; g_input.mouse_pressed=true;
    assert(ftui::table("Table",{"Name","Value"},data,state,opts));
    assert(state.selected_row==1);
    g_input={}; g_input.key_down=true;
    g_ctx.cursor_x=0; g_ctx.cursor_y=0;
    assert(ftui::table("Table",{"Name","Value"},data,state,opts));
    assert(state.selected_row==2 && state.first_row==1);
    // Wheel events must reach nested widgets even with animations disabled.
    g_effects_enabled=false; g_input={};
    XEvent wheel={}; wheel.xbutton.type=ButtonPress; wheel.xbutton.window=g_platform.window;
    wheel.xbutton.button=Button5;
    handle_xevent(wheel);
    assert(g_input.wheel_y==-80);
    g_input.mouse_x=20; g_input.mouse_y=70;
    g_ctx.cursor_x=0; g_ctx.cursor_y=0;
    ftui::table("Table",{"Name","Value"},data,state,opts);
    assert(state.first_row==2 && g_input.wheel_y==0);
    // A window menu works without a preceding widget and escapes content bounds.
    g_input={}; g_dropdown_overlay={}; g_dropdown_overlay_prev={}; g_dropdown_open_id=0;
    g_platform.width=640; g_platform.height=480;
    g_ctx.content_region={20,40,240,180}; g_ctx.last_item_hovered=false;
    g_input.context_pressed=true; g_input.mouse_x=610; g_input.mouse_y=450;
    ft::ContextMenuOptions anywhere; anywhere.scope=ft::ContextMenuScope::Window;
    g_effects_enabled=true;
    ftui::context_menu("anywhere",items,4,anywhere);
    assert(g_dropdown_overlay.context_id!=0 && !g_input.context_pressed);
    assert(g_dropdown_overlay.popup_r.x>260);
    assert(g_dropdown_overlay.animation_scale==1); // No fade or grow-in on the first frame.
    assert(g_dropdown_overlay.labels[3]=="checked" && g_dropdown_overlay.checked[3]);
    assert(g_dropdown_overlay.row_offsets[2]-g_dropdown_overlay.row_offsets[1]==9);
    auto anchor=g_dropdown_overlay.popup_r;
    g_dropdown_overlay_prev=g_dropdown_overlay; g_input={}; g_dt=0.06f;
    ftui::context_menu("anywhere",items,4,anywhere);
    assert(std::abs(g_dropdown_overlay.animation_scale-1.025f)<0.0001f);
    assert(g_dropdown_overlay.popup_r.x==anchor.x && g_dropdown_overlay.popup_r.y==anchor.y);
    draw_dropdown_overlay(); // Animated rendering preserves Cairo's transform.
    cairo_matrix_t matrix; cairo_get_matrix(g_renderer.cr,&matrix); assert(matrix.xx==1 && matrix.yy==1);
    g_dropdown_overlay_prev=g_dropdown_overlay;
    ftui::context_menu("anywhere",items,4,anywhere);
    assert(g_dropdown_overlay.animation_scale==1);
    g_dropdown_overlay_prev=g_dropdown_overlay;
    g_input.mouse_x=anchor.x+30;
    g_input.mouse_y=anchor.y+4+g_dropdown_overlay.row_offsets[2]+10;
    ftui::context_menu("anywhere",items,4,anywhere);
    assert(g_dropdown_overlay.selected_index==2); // Immediate hover, with no easing delay.
    g_dropdown_overlay_prev=g_dropdown_overlay;
    g_input.mouse_pressed=true;
    assert(ftui::context_menu("anywhere",items,4,anywhere)==2);
    g_input={}; g_dropdown_overlay_prev={};
    ft::ContextMenuOptions region; region.scope=ft::ContextMenuScope::Region; region.region={100,100,80,40};
    g_input.context_pressed=true; g_input.mouse_x=99; g_input.mouse_y=110;
    ftui::context_menu("region",items,4,region); assert(!g_dropdown_open_id);
    g_input.mouse_x=110;
    ftui::context_menu("region",items,4,region); assert(g_dropdown_open_id);
    region.enabled=false;
    ftui::context_menu("region",items,4,region); assert(!g_dropdown_open_id && !g_dropdown_overlay.active);
    g_input={}; g_dropdown_overlay_prev={};
    anywhere.animation_seconds=0;
    ftui::context_menu("no-animation",items,4,anywhere,true);
    g_dropdown_overlay_prev=g_dropdown_overlay;
    ftui::context_menu("no-animation",items,4,anywhere);
    assert(g_dropdown_overlay.animation_scale==1);
    assert(cairo_status(g_renderer.cr)==CAIRO_STATUS_SUCCESS);
    cairo_surface_write_to_png(surface,"build/data-widgets-test.png");
    cairo_destroy(g_renderer.cr); g_renderer.cr=nullptr; cairo_surface_destroy(surface);
    g_drawing=false;
    assert(!ftui::set_clipboard_text("no display"));
    assert(ftui::get_clipboard_text().empty());
#endif
    std::puts("Data widget tests passed.");
}
