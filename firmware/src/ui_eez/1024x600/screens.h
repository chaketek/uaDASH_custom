#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN_SCREEN = 1,
    SCREEN_ID_BENCH_SCREEN = 2,
    SCREEN_ID_SETTINGS_SCREEN = 3,
    SCREEN_ID_ENGINE_CONFIG_SCREEN = 4,
    _SCREEN_ID_LAST = 4
};

typedef struct _objects_t {
    lv_obj_t *main_screen;
    lv_obj_t *bench_screen;
    lv_obj_t *settings_screen;
    lv_obj_t *engine_config_screen;
    lv_obj_t *dashboard_band;
    lv_obj_t *ui_container_dashboard;
    lv_obj_t *ui_img_tacho;
    lv_obj_t *ui_arc_rpm;
    lv_obj_t *ui_lbl_rpm;
    lv_obj_t *ui_image_rpm;
    lv_obj_t *ui_image_peak_rpm;
    lv_obj_t *ui_image2;
    lv_obj_t *ui_bar_water_temp;
    lv_obj_t *ui_lbl_water_temp;
    lv_obj_t *ui_bar_iat;
    lv_obj_t *ui_lbl_iat;
    lv_obj_t *ui_bar_oil_temp;
    lv_obj_t *ui_lbl_oil_temp;
    lv_obj_t *ui_bar_map;
    lv_obj_t *ui_lbl_map;
    lv_obj_t *ui_bar_oil_press;
    lv_obj_t *ui_lbl_oil_press;
    lv_obj_t *ui_bar_battery;
    lv_obj_t *ui_lbl_battery;
    lv_obj_t *ui_lbl_afr;
    lv_obj_t *ui_lbl_gear;
    lv_obj_t *ui_container_telltale;
    lv_obj_t *ui_img_telltale;
    lv_obj_t *ui_img_warn_master;
    lv_obj_t *ui_img_warn_oil_press;
    lv_obj_t *ui_img_warn_water_cold;
    lv_obj_t *ui_img_warn_water_hot;
    lv_obj_t *ui_img_warn_battery;
    lv_obj_t *ui_img_warn_fuel;
    lv_obj_t *container_ign;
    lv_obj_t *ign1;
    lv_obj_t *label_ign9;
    lv_obj_t *ign2;
    lv_obj_t *label_ign1;
    lv_obj_t *ign3;
    lv_obj_t *label_ign3;
    lv_obj_t *ign4;
    lv_obj_t *label_ign4;
    lv_obj_t *ign5;
    lv_obj_t *label_ign5;
    lv_obj_t *ign6;
    lv_obj_t *label_ign6;
    lv_obj_t *ign7;
    lv_obj_t *label_ign7;
    lv_obj_t *ign8;
    lv_obj_t *label_ign8;
    lv_obj_t *container_inj;
    lv_obj_t *inj1;
    lv_obj_t *label_inj1;
    lv_obj_t *inj2;
    lv_obj_t *label_inj2;
    lv_obj_t *inj3;
    lv_obj_t *label_inj3;
    lv_obj_t *inj4;
    lv_obj_t *label_inj4;
    lv_obj_t *inj5;
    lv_obj_t *label_inj5;
    lv_obj_t *inj6;
    lv_obj_t *label_inj6;
    lv_obj_t *inj7;
    lv_obj_t *label_inj7;
    lv_obj_t *inj8;
    lv_obj_t *label_inj8;
    lv_obj_t *label_bench_screen;
    lv_obj_t *label_bench_ign;
    lv_obj_t *label_bench_ihj;
    lv_obj_t *start;
    lv_obj_t *label8;
    lv_obj_t *fuel_pump;
    lv_obj_t *label_fuel_pump;
    lv_obj_t *fan2;
    lv_obj_t *label_fan2;
    lv_obj_t *fan1;
    lv_obj_t *label_fan1;
    lv_obj_t *debug_status;
    lv_obj_t *eng_setup;
    lv_obj_t *label_eng_setup;
    lv_obj_t *label_settings_screen;
    lv_obj_t *rpm_warn_container;
    lv_obj_t *rpm_warn_button_plus;
    lv_obj_t *label9;
    lv_obj_t *rpm_warn_val;
    lv_obj_t *rpm_warning_label;
    lv_obj_t *rpm_warn_button_minus;
    lv_obj_t *label10;
    lv_obj_t *label20;
    lv_obj_t *clt_warn_container;
    lv_obj_t *clt_warn_button_plus;
    lv_obj_t *label11;
    lv_obj_t *clt_warn_val;
    lv_obj_t *clt_warn_button_minus;
    lv_obj_t *label12;
    lv_obj_t *clt_warning_label;
    lv_obj_t *label13;
    lv_obj_t *iat_warn_container;
    lv_obj_t *iat_warn_button_plus;
    lv_obj_t *label14;
    lv_obj_t *iat_warn_val;
    lv_obj_t *iat_warn_button_minus;
    lv_obj_t *label15;
    lv_obj_t *label16;
    lv_obj_t *iat_warning_label;
    lv_obj_t *oil_twarn_container;
    lv_obj_t *oil_twarn_button_plus;
    lv_obj_t *label17;
    lv_obj_t *oil_twarn_val;
    lv_obj_t *oil_twarn_button_minus;
    lv_obj_t *label18;
    lv_obj_t *label19;
    lv_obj_t *oil_twarning_label;
    lv_obj_t *oil_pwarn_container;
    lv_obj_t *oil_pwarn_button_plus;
    lv_obj_t *label88;
    lv_obj_t *oil_pwarn_val;
    lv_obj_t *oil_pwarn_button_minus;
    lv_obj_t *label21;
    lv_obj_t *label22;
    lv_obj_t *oil_pwarning_label;
    lv_obj_t *fuel_pwarn_container;
    lv_obj_t *fuel_pwarn_button_plus;
    lv_obj_t *label23;
    lv_obj_t *fuel_pwarn_val;
    lv_obj_t *fuel_pwarn_button_minus;
    lv_obj_t *label24;
    lv_obj_t *label25;
    lv_obj_t *fuel_pwarning_label;
    lv_obj_t *v_batt_warn_container;
    lv_obj_t *v_batt_warn_button_plus;
    lv_obj_t *label26;
    lv_obj_t *v_batt_warn_val;
    lv_obj_t *v_batt_warn_button_minus;
    lv_obj_t *label27;
    lv_obj_t *label28;
    lv_obj_t *v_batt_warning_label;
    lv_obj_t *turbo_map_label;
    lv_obj_t *turbo_switch;
    lv_obj_t *save_button;
    lv_obj_t *label29;
    lv_obj_t *default_warn_settings_button;
    lv_obj_t *label30;
    lv_obj_t *fw_label;
    lv_obj_t *fw_val_label;
    lv_obj_t *label_engine_conf;
    lv_obj_t *label_displacement;
    lv_obj_t *label_trigger;
    lv_obj_t *label_cam;
    lv_obj_t *checkbox_disp48;
    lv_obj_t *checkbox_disp53;
    lv_obj_t *checkbox_disp57;
    lv_obj_t *checkbox_disp60;
    lv_obj_t *checkbox_disp62;
    lv_obj_t *checkbox_disp70;
    lv_obj_t *checkbox_trig24;
    lv_obj_t *checkbox_trig58;
    lv_obj_t *checkbox_camshape1;
    lv_obj_t *checkbox_camshape2;
    lv_obj_t *checkbox_camshape4;
    lv_obj_t *eng_conf_save_button;
    lv_obj_t *label_save;
    lv_obj_t *eng_conf_back_button;
    lv_obj_t *label_back;
} objects_t;

extern objects_t objects;

void create_screen_main_screen();
void tick_screen_main_screen();

void create_screen_bench_screen();
void tick_screen_bench_screen();

void create_screen_settings_screen();
void tick_screen_settings_screen();

void create_screen_engine_config_screen();
void tick_screen_engine_config_screen();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/