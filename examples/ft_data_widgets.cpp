#define FT_IMPLEMENTATION
#include "../ft.hpp"

int main(int argc, char** argv) {
    ft::Config config;
    config.title="FT - Tables, context menus and clipboard";
    config.width=980; config.height=800;
    if (!ft::create_window(config,argc,argv)) return 1;
    ft::TableRows data={{"ft", "C++", "GUI / TUI / Web"}, {"Example", "Text", "Right-click this table"},
                        {"Unicode", "UTF-8", "Zażółć gęślą jaźń"}, {"Source", "Code", "int main() { return 0; }"}};
    ft::TableState table;
    ft::TableOptions table_options; table_options.visible_rows=4;
    ft::ContextMenuOptions menu_options;
    ft::ClipboardViewState clipboard;
    bool animate_menus=true;
    std::string status="Right-click anywhere for actions; select a table row for row actions.";
    while(ft::pump()) {
        ft::begin();
        ft::text("Tables and clipboard");
        ft::checkbox("Enable context menu", &menu_options.enabled);
        menu_options.animation_seconds=animate_menus?0.12f:0.0f;
        ft::table("Projects", {"Name", "Kind", "Description"},data,table,table_options);
        ft::ContextMenuItem actions[]={
            {"Copy row", table.selected_row>=0 && ft::clipboard_can_write()},
            {"View row as code", table.selected_row>=0},
            {"",true,false,true},
            {"Striped rows",true,table_options.striped,false,true}
        };
        int action=ft::context_menu("Row actions",actions,4,menu_options);
        if(action==0 || action==1) {
            std::string value;
            for(const auto& cell:data[table.selected_row]) { if(!value.empty()) value+='\t'; value+=cell; }
            if(action==0) status=ft::set_clipboard_text(value.c_str())?"Copied row":"Clipboard copy failed";
            else { clipboard.text=value; clipboard.loaded=true; }
        }
        if(action==3) table_options.striped=!table_options.striped;
        ft::text(status.c_str());
        ft::code_view("Selected source",clipboard.text.c_str(),3);
        ft::clipboard_view("Clipboard inspector",clipboard,6);
        ft::ContextMenuOptions background=menu_options;
        background.scope=ft::ContextMenuScope::Window;
        ft::ContextMenuItem app_actions[]={
            {"Animate menus",true,animate_menus,false,true},
            {"Striped rows",true,table_options.striped,false,true},
            {"Clear preview",!clipboard.text.empty()}
        };
        int app_action=ft::context_menu("Window actions",app_actions,3,background);
        if(app_action==0) animate_menus=!animate_menus;
        if(app_action==1) table_options.striped=!table_options.striped;
        if(app_action==2) clipboard.text.clear();
        ft::end();
    }
    ft::shutdown();
}
