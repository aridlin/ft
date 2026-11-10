#define FT_IMPLEMENTATION
#include "../ft.hpp"
#include <deque>

static ft::Style blueprint() {
    auto s=ft::one_dark_style();
    s.background=ft::color_from_hex("#012b44");s.panel=s.input_bg=ft::color_from_hex("#073851");
    s.button=ft::color_from_hex("#10445f");s.button_hover=ft::color_from_hex("#236681");
    s.button_active=ft::color_from_hex("#397f99");s.border=ft::color_from_hex("#88bed1");
    s.text=ft::color_from_hex("#ffffff");s.text_dim=ft::color_from_hex("#bddbe5");
    s.accent=s.input_focus=ft::color_from_hex("#64d8ff");
    s.font_family="monospace";s.rounding=0;s.item_spacing=8;s.border_width=1;
    auto painter=std::make_shared<ft::NativeThemePainter>();
    painter->chrome=[](ftui::Canvas& c,const ft::ThemeChrome& x){
        c.fill(x.bounds,x.fill,0);
        c.stroke(x.bounds,x.border,x.focus>.5f?3.f:1.f,0);
        if(x.focus>.5f)c.line(x.bounds.x,x.bounds.y,x.bounds.x+10,x.bounds.y+10,x.border,2);
        return true;
    };
    s.native_painter=painter;
    s.web_css=R"CSS(
:root{color-scheme:dark;--desk:#012b44;--paper:#073851;--paper2:#073851;--ink:#fff;--muted:#bddbe5;--border:#88bed1;--button:#10445f;--hover:#236681;--active:#397f99;--input:#073851;--focus:#64d8ff;--accent:#64d8ff}
body{background:#012b44;color:#fff}main{box-shadow:none;border:1px solid #88bed1;border-radius:0}
button,input,textarea,select,.ft-scroll,details{border-radius:0;box-shadow:none;filter:none;font-weight:400}
button{color:#fff}button.accent{color:#012b44}.ft-title{font-weight:400;text-shadow:none}
)CSS";
    return s;
}

int main(int argc,char** argv) {
    ft::Config config;config.title="FT / Paper Ink Studio";
    config.width=1360;config.height=1000;
    config.web_host="127.0.0.1";config.web_port=18764;
    if(!ft::create_window(config,argc,argv))return 1;
    ft::register_deep_theme("blueprint-demo",blueprint());
    ft::set_deep_theme("paper-ink");
    const char* themes[]={"Paper Ink","Magenta Ink","One Dark","Blueprint / custom"};
    const char* ids[]={"paper-ink","paper-ink-magenta","standard","blueprint-demo"};
    const char* tabs[]={"WORKSPACE","CONTROLS","DATA / LOGS","THEME LAB"};
    char title[160]="Żółć / design workspace",notes[2048]="Paper Ink study\n\nA larger playground with native C++ controls, data tables,\neditable content, progress, modal dialogs and deep themes.\n\nSwitch appearance from the selector or the : command bar.";
    char search[100]="",password[100]="ink-and-paper";
    bool local=true,notifications=false,advanced=false,showPercent=true;
    float amount=.62f,opacity=.85f,roughness=.32f;
    int tab=0,theme=0,selected=0,delivery=0,clicks=0;
    const char* choices[]={"DESIGN SYSTEM","NATIVE CONTROLS","WEB ADAPTER","TYPOGRAPHY","REGRESSION CHECKS","RELEASE PACKAGE"};
    const char* states[]={"DRAFT","REVIEW","READY"};
    ft::TableRows records;for(int i=0;i<48;++i)records.push_back({std::to_string(i+1),choices[i%6],states[i%3],std::to_string(12+(i*17)%90)+" KB"});
    ft::TableState table;ft::TableOptions tableOptions;tableOptions.visible_rows=9;
    std::string log="Studio started.\n";
    auto event=[&](const std::string& text){log+=text+"\n";if(log.size()>6000)log.erase(0,2000);};
    while(ft::pump()) {
        ft::begin();
        ft::row({2,1},[&]{
            ft::text("PAPER INK / STUDIO");
            const auto& style=ft::get_style();
            for(int i=0;i<4;++i)if(style.deep_theme_name==ids[i] || (i==2&&style.deep_theme_name.empty()))theme=i;
            if(ft::dropdown("Appearance",themes,4,&theme)) {ft::set_deep_theme(ids[theme]);event(std::string("Theme: ")+ids[theme]);}
        });
        ft::text("Click outside a text field, press : and type deep-theme paper-ink, paper-ink-magenta, standard or blueprint-demo.");
        ft::tabs(tabs,4,&tab);
        ft::spacing(10);
        if(tab==0) {
            ft::row({2,1},[&]{
                ft::scroll_area("Workspace editor",650,[&]{
                    ft::input("WORKSPACE",title,sizeof(title));
                    ft::text_area("NOTES",notes,sizeof(notes),7);
                    ft::checkbox("Keep local copies",&local);
                    ft::checkbox("Show completion notifications",&notifications);
                    ft::slider_float("AMOUNT",&amount,0,1);
                    ft::row(3,[&]{
                        if(ft::button("APPLY",ft::ColorRole::Accent)){++clicks;event("Workspace applied");ft::toast_success("Saved in this demo session");}
                        if(ft::button("PREVIEW"))ft::open_modal("Preview");
                        ft::begin_disabled();ft::button("WAITING");ft::end_disabled();
                    });
                    ft::spacing(16);ft::ProgressStyle p;p.label="PROCESSING";p.height=34;p.show_percent=showPercent;ft::progress_bar(amount,p);
                });
                ft::scroll_area("Queue and status",650,[&]{
                    ft::text("PROJECT QUEUE");ft::listbox("Projects",choices,6,&selected,6);
                    ft::radio_group("DELIVERY",states,3,&delivery);
                    if(ft::button("QUEUE SELECTED")){event(std::string("Queued: ")+choices[selected]);ft::toast_info("Item added to the demo queue");}
                    ft::spacing(10);ft::text("SESSION ACTIVITY");ft::log_view("Recent events",log.c_str(),5);
                });
            });
        } else if(tab==1) {
            ft::row(2,[&]{
                ft::scroll_area("Input controls",650,[&]{
                    ft::input("Search",search,sizeof(search));
                    ft::input("Password",password,sizeof(password),ft::InputFlags::Password);
                    ft::checkbox("Enabled checkbox",&local);ft::checkbox("Notifications",&notifications);
                    ft::slider_float("Opacity",&opacity,0,1);
                    ft::radio_group("Pipeline status",states,3,&delivery,3);
                    ft::begin_disabled();ft::input("Read-only state",title,sizeof(title));ft::checkbox("Disabled checkbox",&local);ft::button("Disabled action");ft::end_disabled();
                    if(ft::collapsing_header("Advanced controls",&advanced)) {
                        ft::checkbox("Show progress percentage",&showPercent);
                        ft::slider_float("Ink roughness",&roughness,0,1);
                        if(ft::button("APPLY ROUGHNESS")){auto s=ft::get_style();s.ink_roughness=roughness;ft::set_style(s);}
                    }
                });
                ft::scroll_area("Action controls",650,[&]{
                    ft::text("ACTIONS / FEEDBACK");
                    ft::row(2,[&]{if(ft::button("INFO"))ft::toast_info("Informational message");if(ft::button("SUCCESS",ft::ColorRole::Success))ft::toast_success("Operation complete");});
                    ft::row(2,[&]{if(ft::button("WARNING",ft::ColorRole::Warning))ft::toast_warning("Review this demo item");if(ft::button("ERROR"))ft::toast_error("Example actionable error");});
                    if(ft::button("OPEN MODAL"))ft::open_modal("Preview");
                    ft::spacing(12);
                    for(float value:{0.f,.25f,.5f,.75f,1.f}){ft::ProgressStyle p;p.height=26;p.show_percent=showPercent;ft::progress_bar(value,p);ft::spacing(8);}
                    ft::text_wrapped("Keyboard: Tab changes focus, Enter activates a focused button, and Escape closes the command bar.\n\nTry hover, focus, disabled and selected states in each theme.");
                });
            });
        } else if(tab==2) {
            ft::table("Project records",{"ID","PROJECT","STATUS","SIZE"},records,table,tableOptions);
            ft::ContextMenuItem actions[]={{"Queue row",table.selected_row>=0},{"Inspect row",table.selected_row>=0}};
            int action=ft::context_menu("Record actions",actions,2);
            if(action>=0){event("Row action on record "+std::to_string(table.selected_row+1));if(action==1)ft::open_modal("Preview");}
            ft::row(2,[&]{ft::log_view("Event log",log.c_str(),7);ft::code_view("C++ theme selection","ft::set_deep_theme(\"paper-ink\");\nft::set_deep_theme(\"blueprint-demo\");\nft::set_style(ft::one_dark_style());",7);});
        } else {
            ft::text("DEEP THEME LAB");
            ft::text_wrapped("Paper Ink replaces native control painting and reuses the original CSS on web. Blueprint is a separately registered custom theme, using the same public painter API. Terminal keeps its regular renderer.");
            ft::code_view("Commands",":deep-theme paper-ink\n:deep-theme paper-ink-magenta\n:deep-theme blueprint-demo\n:deep-theme standard\n\nTab completes a command, including registered future themes.",8);
            ft::row(4,[&]{for(int i=0;i<4;++i)if(ft::button(themes[i]))ft::set_deep_theme(ids[i]);});
            ft::text_wrapped("Try switching while notes, selections, sliders and table state are edited. Theme changes preserve the application's state.");
        }
        ft::modal("Preview",[&]{ft::text("WORKSPACE PREVIEW");ft::text(title);ft::text_wrapped(notes);if(ft::button("CLOSE"))ft::close_modal();});
        ft::end();
    }
    ft::shutdown();
}
