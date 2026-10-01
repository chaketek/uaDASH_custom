#if !(defined(WAVESHARE_S3_LCD7B))  // tools/eez_build.py: board variant
#include "images.h"

const ext_img_desc_t images[10] = {
    { "ui_img_back6_png", &img_ui_img_back6_png },
    { "ui_img_marker_red_bar_80x7s_png", &img_ui_img_marker_red_bar_80x7s_png },
    { "ui_img_marker_red_bar_20x7s_png", &img_ui_img_marker_red_bar_20x7s_png },
    { "ui_img_ws_masterwarning_png", &img_ui_img_ws_masterwarning_png },
    { "ui_img_ws_oilpresswarning_png", &img_ui_img_ws_oilpresswarning_png },
    { "ui_img_ws_watarcool_png", &img_ui_img_ws_watarcool_png },
    { "ui_img_ws_waterwarning_png", &img_ui_img_ws_waterwarning_png },
    { "ui_img_ws_batterywarning_png", &img_ui_img_ws_batterywarning_png },
    { "ui_img_ws_fuelcheck_png", &img_ui_img_ws_fuelcheck_png },
    { "ui_img_back5_png", &img_ui_img_back5_png },
};
#endif
