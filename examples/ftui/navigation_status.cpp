#define FT_IMPLEMENTATION
#include "../../ft.hpp"

static const char* kBatteryMaskSvg =
    "<svg width='160' height='64' viewBox='0 0 160 64' xmlns='http://www.w3.org/2000/svg'>"
    "<rect x='4' y='12' width='132' height='40' rx='6' ry='6' fill='white'/>"
    "<rect x='140' y='24' width='16' height='16' rx='3' ry='3' fill='white'/>"
    "</svg>";

int main(int argc, char** argv) {
    ft::Config cfg;
    cfg.title = "FTUI Navigation + Status";
    cfg.width = 1040;
    cfg.height = 720;
    cfg.backdrop_effect = ft::BackdropEffect::BayerDither;
    cfg.dither_size = 5;

    if (!ft::create_window(cfg, argc, argv)) return 1;
    ft::set_style(ft::ghostty_green_style());

    static const char* pages[] = {
        "Dashboard",
        "Network",
        "Services",
        "Logs",
        "Settings"
    };

    int page = 0;
    bool relay = true;
    float progress = 0.73f;

    while (ft::pump()) {
        ft::begin();

        ft::side_layout(220.0f, [&]() {
            ft::side_menu("Navigation", pages, 5, &page);

            ft::content([&]() {
                if (page == 0) {
                    ft::text("Dashboard");
                    ft::text_wrapped("Side menus are useful when a utility app outgrows a tab strip.");
                    ft::progress_bar(progress, "Loading assets");

                    ft::ProgressStyle battery;
                    battery.label = "Battery mask";
                    battery.mask_svg = kBatteryMaskSvg;
                    battery.height = 54.0f;
                    battery.wave_front = true;
                    battery.glint = true;
                    ft::progress_bar(progress, battery);

                    ft::row(3, [&]() {
                        if (ft::button("Saved")) ft::toast_success("Configuration saved");
                        if (ft::button("Warn")) ft::toast_warning("High CPU usage");
                        if (ft::button("Error")) ft::toast_error("Connection failed");
                    });
                } else if (page == 1) {
                    ft::text("Network");
                    ft::checkbox("Relay enabled", &relay);
                    if (ft::button("Restart relay")) {
                        ft::Toast t;
                        t.message = "Relay restarted";
                        t.type = ft::ToastType::Success;
                        t.duration_ms = 5000;
                        t.dismissible = true;
                        ft::toast(t);
                    }
                } else if (page == 2) {
                    ft::text("Services");
                    ft::progress_bar(0.42f, "Service rollout");
                } else if (page == 3) {
                    ft::text("Logs");
                    if (ft::button("Refresh logs")) ft::toast_info("Logs refreshed");
                    ft::text_wrapped("Toast calls belong naturally in event handlers, with no user-created manager or container.");
                } else {
                    ft::text("Settings");
                    if (ft::button("Clear toasts")) ft::clear_toasts();
                    ft::slider_float("Demo progress", &progress, 0.0f, 1.0f);
                }
            });
        });

        ft::end();
    }

    ft::shutdown();
    return 0;
}
