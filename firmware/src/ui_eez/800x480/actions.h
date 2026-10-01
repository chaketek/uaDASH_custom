#ifndef EEZ_LVGL_UI_EVENTS_H
#define EEZ_LVGL_UI_EVENTS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void action_main_screen_gesture(lv_event_t * e);
extern void action_bench_screen_gesture(lv_event_t * e);
extern void action_bench_ign1(lv_event_t * e);
extern void action_bench_ign2(lv_event_t * e);
extern void action_bench_ign3(lv_event_t * e);
extern void action_bench_ign4(lv_event_t * e);
extern void action_bench_ign5(lv_event_t * e);
extern void action_bench_ign6(lv_event_t * e);
extern void action_bench_ign7(lv_event_t * e);
extern void action_bench_ign8(lv_event_t * e);
extern void action_bench_inj1(lv_event_t * e);
extern void action_bench_inj2(lv_event_t * e);
extern void action_bench_inj3(lv_event_t * e);
extern void action_bench_inj4(lv_event_t * e);
extern void action_bench_inj5(lv_event_t * e);
extern void action_bench_inj6(lv_event_t * e);
extern void action_bench_inj7(lv_event_t * e);
extern void action_bench_inj8(lv_event_t * e);
extern void action_start_stop(lv_event_t * e);
extern void action_bench_fuel_pump(lv_event_t * e);
extern void action_bench_fan2(lv_event_t * e);
extern void action_bench_fan1(lv_event_t * e);
extern void action_engine_setting_screen_init_settup(lv_event_t * e);
extern void action_settings_screen_gesture(lv_event_t * e);
extern void action_rpm_warn_set_plus(lv_event_t * e);
extern void action_rpm_warn_set_minus(lv_event_t * e);
extern void action_clt_warn_set_plus(lv_event_t * e);
extern void action_clt_warn_set_minus(lv_event_t * e);
extern void action_iat_warn_set_plus(lv_event_t * e);
extern void action_iat_warn_set_minus(lv_event_t * e);
extern void action_oil_t_warn_set_plus(lv_event_t * e);
extern void action_oil_t_warn_set_minus(lv_event_t * e);
extern void action_oil_p_warn_set_plus(lv_event_t * e);
extern void action_oil_p_warn_set_minus(lv_event_t * e);
extern void action_fuel_p_warn_set_plus(lv_event_t * e);
extern void action_fuel_p_warn_set_minus(lv_event_t * e);
extern void action_v_batt_warn_set_plus(lv_event_t * e);
extern void action_v_batt_warn_set_minus(lv_event_t * e);
extern void action_turbo_switch_changed(lv_event_t * e);
extern void action_save_warn_set(lv_event_t * e);
extern void action_default_warn_set(lv_event_t * e);
extern void action_set_disp48_l(lv_event_t * e);
extern void action_set_disp53_l(lv_event_t * e);
extern void action_set_disp57_l(lv_event_t * e);
extern void action_set_disp60_l(lv_event_t * e);
extern void action_set_disp62_l(lv_event_t * e);
extern void action_set_disp70_l(lv_event_t * e);
extern void action_set_trig24(lv_event_t * e);
extern void action_set_trig58(lv_event_t * e);
extern void action_set_cam_s1(lv_event_t * e);
extern void action_set_cam_s2(lv_event_t * e);
extern void action_set_cam_s4(lv_event_t * e);
extern void action_eng_set_save(lv_event_t * e);
extern void action_clear_eng_configuration(lv_event_t * e);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_EVENTS_H*/