// EEZ Studio native actions (eez/uaDash.eez-project, see eez/actions.md)
// They call the existing event handlers in events.cpp.

#include "ui_eez.h"
#include "events.h"

static lv_dir_t gestureDir() {
  lv_indev_t *indev = lv_indev_get_act();
  lv_dir_t dir = lv_indev_get_gesture_dir(indev);
  lv_indev_wait_release(indev);
  return dir;
}

extern "C" {

// screen changes

void action_main_screen_gesture(lv_event_t *e) {
  switch (gestureDir()) {
    case LV_DIR_TOP:
      upBrightness(e);
      break;
    case LV_DIR_BOTTOM:
      downBribrightness(e);
      break;
    case LV_DIR_LEFT:
      loadScreen(SCREEN_ID_BENCH_SCREEN);
      benchScreenInitSetup(e);
      break;
    case LV_DIR_RIGHT:
      loadScreen(SCREEN_ID_SETTINGS_SCREEN);
      preInitSettingsScreen(e);
      break;
    default:
      break;
  }
}

void action_bench_screen_gesture(lv_event_t *e) {
  if (gestureDir() == LV_DIR_RIGHT) {
    loadScreen(SCREEN_ID_MAIN_SCREEN);
  }
}

void action_settings_screen_gesture(lv_event_t *e) {
  if (gestureDir() == LV_DIR_LEFT) {
    loadScreen(SCREEN_ID_MAIN_SCREEN);
  }
}

void action_engine_setting_screen_init_settup(lv_event_t *e) {
  loadScreen(SCREEN_ID_ENGINE_CONFIG_SCREEN);
  engineSettingScreenInitSettup(e);
}

void action_clear_eng_configuration(lv_event_t *e) {
  loadScreen(SCREEN_ID_BENCH_SCREEN);
  clearEngConfiguration(e);
}

// bench test

void action_bench_ign1(lv_event_t *e) { benchIGN1(e); }
void action_bench_ign2(lv_event_t *e) { benchIGN2(e); }
void action_bench_ign3(lv_event_t *e) { benchIGN3(e); }
void action_bench_ign4(lv_event_t *e) { benchIGN4(e); }
void action_bench_ign5(lv_event_t *e) { benchIGN5(e); }
void action_bench_ign6(lv_event_t *e) { benchIGN6(e); }
void action_bench_ign7(lv_event_t *e) { benchIGN7(e); }
void action_bench_ign8(lv_event_t *e) { benchIGN8(e); }
void action_bench_inj1(lv_event_t *e) { benchINJ1(e); }
void action_bench_inj2(lv_event_t *e) { benchINJ2(e); }
void action_bench_inj3(lv_event_t *e) { benchINJ3(e); }
void action_bench_inj4(lv_event_t *e) { benchINJ4(e); }
void action_bench_inj5(lv_event_t *e) { benchINJ5(e); }
void action_bench_inj6(lv_event_t *e) { benchINJ6(e); }
void action_bench_inj7(lv_event_t *e) { benchINJ7(e); }
void action_bench_inj8(lv_event_t *e) { benchINJ8(e); }
void action_start_stop(lv_event_t *e) { StartStop(e); }
void action_bench_fuel_pump(lv_event_t *e) { benchFuelPump(e); }
void action_bench_fan1(lv_event_t *e) { benchFan1(e); }
void action_bench_fan2(lv_event_t *e) { benchFan2(e); }

// warning settings

void action_rpm_warn_set_plus(lv_event_t *e) { rpmWarnSetPlus(e); }
void action_rpm_warn_set_minus(lv_event_t *e) { rpmWarnSetMinus(e); }
void action_clt_warn_set_plus(lv_event_t *e) { cltWarnSetPlus(e); }
void action_clt_warn_set_minus(lv_event_t *e) { cltWarnSetMinus(e); }
void action_iat_warn_set_plus(lv_event_t *e) { iatWarnSetPlus(e); }
void action_iat_warn_set_minus(lv_event_t *e) { iatWarnSetMinus(e); }
void action_oil_t_warn_set_plus(lv_event_t *e) { oilTWarnSetPlus(e); }
void action_oil_t_warn_set_minus(lv_event_t *e) { oilTWarnSetMinus(e); }
void action_oil_p_warn_set_plus(lv_event_t *e) { oilPWarnSetPlus(e); }
void action_oil_p_warn_set_minus(lv_event_t *e) { oilPWarnSetMinus(e); }
void action_fuel_p_warn_set_plus(lv_event_t *e) { fuelPWarnSetPlus(e); }
void action_fuel_p_warn_set_minus(lv_event_t *e) { fuelPWarnSetMinus(e); }
void action_v_batt_warn_set_plus(lv_event_t *e) { vBattWarnSetPlus(e); }
void action_v_batt_warn_set_minus(lv_event_t *e) { vBattWarnSetMinus(e); }
void action_save_warn_set(lv_event_t *e) { saveWarnSet(e); }
void action_default_warn_set(lv_event_t *e) { defaultWarnSet(e); }

void action_turbo_switch_changed(lv_event_t *e) {
  if (lv_obj_has_state(lv_event_get_target(e), LV_STATE_CHECKED)) {
    setIsTurbo(e);
  } else {
    setIsNaturalA(e);
  }
}

// engine configuration

void action_set_disp48_l(lv_event_t *e) { setDisp48L(e); }
void action_set_disp53_l(lv_event_t *e) { setDisp53L(e); }
void action_set_disp57_l(lv_event_t *e) { setDisp57L(e); }
void action_set_disp60_l(lv_event_t *e) { setDisp60L(e); }
void action_set_disp62_l(lv_event_t *e) { setDisp62L(e); }
void action_set_disp70_l(lv_event_t *e) { setDisp70L(e); }
void action_set_trig24(lv_event_t *e) { setTrig24(e); }
void action_set_trig58(lv_event_t *e) { setTrig58(e); }
void action_set_cam_s1(lv_event_t *e) { setCamS1(e); }
void action_set_cam_s2(lv_event_t *e) { setCamS2(e); }
void action_set_cam_s4(lv_event_t *e) { setCamS4(e); }
void action_eng_set_save(lv_event_t *e) { engSetSave(e); }

}  // extern "C"
