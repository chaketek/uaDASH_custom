#include "updaters.h"
#include <climits>

ESP32S3_TWAI can;
Preferences preferences;

void TaskHeartbeat(void *pvParameters)
{
  // uint8_t data[] = { 0x66, 0x00, 0x55, 0xAA, 0x00 };
  uint8_t data[] = {(uint8_t)bench_test_magic_numbers_e::BENCH_HEADER, 0x00, 0x55, 0xAA, 0x00};
  uint8_t countHeartbeat = 0;
  uint32_t error_count = 0;
  while (1)
  {
    data[4] = countHeartbeat;
    // 0x77000F 0x66 0x00 0x55 0xAA count -- heartbeat
    // can.send(0x77000F, data, sizeof(data), true);
    if (false == can.send((uint32_t)bench_test_packet_ids_e::DASH_ALIVE, data, sizeof(data), true))
    {
      error_count++;
#ifdef DEBUG
      Serial.printf("TaskHeartbeat error_count: %" PRIu32 "\n", error_count);
#endif
    }
    countHeartbeat++;
    vTaskDelay(pdMS_TO_TICKS(PERIOD_HEARTBEAT_MS));
  }
}

void TaskCANReceiver(void *pvParameters)
{
  uint32_t id;
  uint8_t data[8];
  uint8_t length;
  bool extended;

  Ticker updateUiFast;
  Ticker updateUiMid;
  Ticker updateUiSlow;

  getWarningsSet();
  changeMapWidget = true;
  canEngineConfig = false;
  checkEngineConfig = true;

  bool ret = false;
#ifdef DEBUG
  Serial.println("TaskCANReceiver init");
#endif

  while (!ret)
  {
    ret = can.init();
#ifdef DEBUG
    Serial.printf("can.init : %s\n", ret ? "OK" : "FAIL");
#endif
  }

  ret = can.alertConfigure(TWAI_ALERT_RX_DATA | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_BUS_OFF);
#ifdef DEBUG
  Serial.printf("can.alertConfigure : %s\n", ret ? "OK" : "FAIL");
#endif

  updateUiFast.attach_ms(PERIOD_FAST_MS, fastUpdate);
  updateUiMid.attach_ms(PERIOD_MID_MS, midUpdate);
  updateUiSlow.attach_ms(PERIOD_SLOW_MS, slowUpdate);

  while (1)
  {
    // // Check if message is received
    if (can.getAlerts())
    {
      // One or more messages received. Handle all.
      while (can.receive(&id, data, &length, &extended))
      {
        if (extended && checkEngineConfig)
        {
#ifdef DEBUG
          Serial.printf("receive ext: %s\n", extended ? "yes" : "no");
#endif
          // if (id == 0x77000D) {
          if (id == (uint32_t)bench_test_packet_ids_e::ECU_CONFIG_BROADCAST)
          {
            // if (data[0] == 0x66) {
            if (data[0] == (uint8_t)bench_test_magic_numbers_e::BENCH_HEADER)
            {
              canEngineConfig = true;
              checkEngineConfig = false;
              engineConfig.displacement = data[2];
              engineConfig.trigger = data[3];
              engineConfig.camshape = data[4];
            }
          }
        }
        //  ID 0x200
        // SG_ WarningCounter : 0|16@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ LastError : 16|16@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ RevLimAct : 32|1@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ MainRelayAct : 33|1@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ FuelPumpAct : 34|1@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ CELAct : 35|1@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ EGOHeatAct : 36|1@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ LambdaProtectAct : 37|1@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ CurrentGear : 40|8@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ DistanceTraveled : 48|16@1+ (0.1,0) [0|6553.5] "km" Vector__XXX
        // SG_ Fan : 38|1@1+ (1,0) [0|0] "" Vector__XXX
        // SG_ Fan2 : 39|1@1+ (1,0) [0|0] "" Vector__XXX

        //  ID 0x201
        //  SG_ RPM : 0|16@1+ (1,0) [0|0] "RPM" Vector__XXX
        //  SG_ IgnitionTiming : 16|16@1- (0.02,0) [0|0] "deg" Vector__XXX
        //  SG_ InjDuty : 32|8@1+ (0.5,0) [0|100] "%" Vector__XXX
        //  SG_ IgnDuty : 40|8@1+ (0.5,0) [0|100] "%" Vector__XXX
        //  SG_ VehicleSpeed : 48|8@1+ (1,0) [0|255] "kph" Vector__XXX
        //  SG_ FlexPct : 56|8@1+ (1,0) [0|100] "%" Vector__XXX

        //  ID 0x202
        //  SG_ PPS : 0|16@1- (0.01,0) [0|100] "%" Vector__XXX
        //  SG_ TPS1 : 16|16@1- (0.01,0) [0|100] "%" Vector__XXX
        //  SG_ TPS2 : 32|16@1- (0.01,0) [0|100] "%" Vector__XXX
        //  SG_ Wastegate : 48|16@1- (0.01,0) [0|100] "%" Vector__XXX

        //  ID 0x203
        //  SG_ MAP : 0|16@1+ (0.03333333,0) [0|0] "kPa" Vector__XXX
        //  SG_ CoolantTemp : 16|8@1+ (1,-40) [-40|200] "deg C" Vector__XXX
        //  SG_ IntakeTemp : 24|8@1+ (1,-40) [-40|200] "deg C" Vector__XXX
        //  SG_ AUX1Temp : 32|8@1+ (1,-40) [-40|200] "deg C" Vector__XXX
        //  SG_ AUX2Temp : 40|8@1+ (1,-40) [-40|200] "deg C" Vector__XXX
        //  SG_ MCUTemp : 48|8@1+ (1,-40) [-40|100] "deg C" Vector__XXX
        //  SG_ FuelLevel : 56|8@1+ (0.5,0) [0|0] "%" Vector__XXX

        //  ID 0x204
        //  SG_ OilPress : 16|16@1+ (0.03333333,0) [0|0] "kPa" Vector__XXX
        //  SG_ OilTemperature : 32|8@1+ (1,-40) [-40|215] "deg C" Vector__XXX
        //  SG_ FuelTemperature : 40|8@1+ (1,-40) [-40|215] "deg C" Vector__XXX
        //  SG_ BattVolt : 48|16@1+ (0.001,0) [0|25] "mV" Vector__XXX

        //  ID 0x207
        //  SG_ Lam1 : 0|16@1+ (0.0001,0) [0|2] "lambda" Vector__XXX
        //  SG_ Lam2 : 16|16@1+ (0.0001,0) [0|2] "lambda" Vector__XXX
        //  SG_ FpLow : 32|16@1+ (0.03333333,0) [0|0] "kPa" Vector__XXX
        //  SG_ FpHigh : 48|16@1+ (0.1,0) [0|0] "bar" Vector__XXX
        if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
        {

          switch (id)
          {
          case 0x200:
            myData.gear = data[5];  // CurrentGear
            break;
          case 0x201:
            myData.rpm = (data[1] << 8 | data[0]);
            myData.speed = data[6];
            break;
          case 0x203:
            myData.map = (data[1] << 8 | data[0]) * 0.0333;
            myData.clt = data[2] - 40;
            myData.iat = data[3] - 40;
            myData.fuelLevel = data[7] / 2;
            break;
          case 0x204:
            myData.oilPress = (data[3] << 8 | data[2]) * 0.0333;
            myData.oilTemp = data[4] - 40;
            myData.Vbat = (data[7] << 8 | data[6]) * 0.001;
            break;
          case 0x207:
            myData.afr = (data[1] << 8 | data[0]) * 0.00147;
            myData.fuelPress = (data[5] << 8 | data[4]) * 0.0333;
            break;
          }
          xSemaphoreGive(dataMutex);
        }
      }
    }
#ifdef DEMO_DATA
    // fake sweeping values for testing the display without ECU
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
    {
      uint32_t t = millis() % 8000;
      uint32_t tri = t < 4000 ? t : 8000 - t;  // 0..4000..0
      myData.rpm = 800 + tri * 7000 / 4000;
      myData.speed = tri * 200 / 4000;
      myData.map = 30 + tri * 220 / 4000;
      myData.afr = 10 + tri * 10.0 / 4000;
      myData.clt = 20 + tri * 100 / 4000;
      myData.iat = tri * 80 / 4000;
      myData.oilTemp = 40 + tri * 80 / 4000;
      myData.oilPress = tri * 500 / 4000;
      myData.fuelPress = tri * 500 / 4000;
      myData.fuelLevel = tri * 100 / 4000;
      myData.Vbat = 11 + tri * 3.0 / 4000;
      myData.gear = tri * 6 / 4000;
      xSemaphoreGive(dataMutex);
    }
#endif
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

// Widget updaters
// Main screen: FULLMONI-WIDE "eez002" dashboard design
// (bindings ported from FULLMONI-WIDE Firmware/eez/eez002/ui_binding/ui_dashboard.c)

#define BAR_WATER_MIN 0
#define BAR_WATER_MAX 130     // degC
#define BAR_IAT_MIN 0
#define BAR_IAT_MAX 70        // degC
#define BAR_OILTEMP_MIN 0
#define BAR_OILTEMP_MAX 160   // degC
#define BAR_MAP_MIN 0
#define BAR_MAP_MAX 300       // kPa
#define BAR_OILPRESS_MIN 0
#define BAR_OILPRESS_MAX 500  // kPa
#define BAR_BATT_MIN 100      // 0.1V
#define BAR_BATT_MAX 160      // 0.1V

#define WARN_WATER_COLD 60    // degC, below: cold engine indicator
#define WARN_FUEL_SHOW 5      // %, fuel warning on below, off above WARN_FUEL_HIDE
#define WARN_FUEL_HIDE 10
#define STARTUP_TELLTALE_MS 3000  // all warning lights on after power up (lamp check)

#define TACHO_MAX_RPM 9000
#define PEAK_HOLD_MS 500
#define PEAK_FALL_RPM_PER_UPDATE 100  // per fast update (20ms) -> 5000 rpm/s

static uint32_t telltaleStart;
static bool telltaleDone;
static bool warnFuel;
static uint32_t rpmPeak;
static uint32_t peakHoldStart;
static bool peakFalling;

// needle angle in 0.1 deg: 0 rpm -> 90 deg, 9000 rpm -> 360 deg
static int16_t rpmToAngle(uint32_t rpm)
{
  if (rpm > TACHO_MAX_RPM)
    rpm = TACHO_MAX_RPM;
  return 900 + rpm * 2700 / TACHO_MAX_RPM;
}

static void setVisible(lv_obj_t *obj, bool visible)
{
  if (visible)
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
  else
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}

static void showAllWarnings(bool on)
{
  setVisible(objects.ui_img_warn_master, on);
  setVisible(objects.ui_img_warn_oil_press, on);
  setVisible(objects.ui_img_warn_water_cold, on);
  setVisible(objects.ui_img_warn_water_hot, on);
  setVisible(objects.ui_img_warn_battery, on);
  setVisible(objects.ui_img_warn_fuel, on);
}

// call once after ui_init()
void dashboardInit()
{
  lv_bar_set_range(objects.ui_bar_water_temp, BAR_WATER_MIN, BAR_WATER_MAX);
  lv_bar_set_range(objects.ui_bar_iat, BAR_IAT_MIN, BAR_IAT_MAX);
  lv_bar_set_range(objects.ui_bar_oil_temp, BAR_OILTEMP_MIN, BAR_OILTEMP_MAX);
  lv_bar_set_range(objects.ui_bar_map, BAR_MAP_MIN, BAR_MAP_MAX);
  lv_bar_set_range(objects.ui_bar_oil_press, BAR_OILPRESS_MIN, BAR_OILPRESS_MAX);
  lv_bar_set_range(objects.ui_bar_battery, BAR_BATT_MIN, BAR_BATT_MAX);

  lv_img_set_angle(objects.ui_image_rpm, rpmToAngle(0));
  lv_img_set_angle(objects.ui_image_peak_rpm, rpmToAngle(0));
  lv_arc_set_value(objects.ui_arc_rpm, 0);
  lv_label_set_text(objects.ui_lbl_rpm, "0");

  showAllWarnings(true);
  telltaleStart = lv_tick_get();
  telltaleDone = false;
  rpmPeak = 0;
  peakFalling = false;
  peakHoldStart = lv_tick_get();

  // force the first update of every value
  old_myData.rpm = -1;
  old_myData.clt = INT_MIN;
  old_myData.iat = INT_MIN;
  old_myData.oilTemp = INT_MIN;
  old_myData.fuelLevel = -1;
  old_myData.gear = -1;
  old_myData.afr = -1;
  old_myData.Vbat = -1;
  old_myData.map = -1;
  old_myData.oilPress = -1;
}

static void updateRpmPeak(uint32_t rpm)
{
  uint32_t now = lv_tick_get();
  if (rpm >= rpmPeak)
  {
    rpmPeak = rpm;
    peakHoldStart = now;
    peakFalling = false;
  }
  else if (!peakFalling)
  {
    peakFalling = now - peakHoldStart >= PEAK_HOLD_MS;
  }
  if (peakFalling)
  {
    uint32_t diff = rpmPeak - rpm;
    rpmPeak = diff > PEAK_FALL_RPM_PER_UPDATE ? rpmPeak - PEAK_FALL_RPM_PER_UPDATE : rpm;
  }
  lv_img_set_angle(objects.ui_image_peak_rpm, rpmToAngle(rpmPeak));
}

void fastUpdate()
{
  if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
  {
    if (xSemaphoreTake(uiMutex, portMAX_DELAY) == pdTRUE)
    {
      // RPM: needle, arc, peak hold, value
      uint32_t rpm = myData.rpm < 0 ? 0 : myData.rpm;
      if (myData.rpm != old_myData.rpm)
      {
        lv_img_set_angle(objects.ui_image_rpm, rpmToAngle(rpm));
        lv_arc_set_value(objects.ui_arc_rpm, rpm > TACHO_MAX_RPM ? TACHO_MAX_RPM : rpm);
        lv_label_set_text_fmt(objects.ui_lbl_rpm, "%u", (unsigned)rpm);
        old_myData.rpm = myData.rpm;
      }
      updateRpmPeak(rpm);
      // AFR
      if (myData.afr != old_myData.afr)
      {
        int afr10 = (int)(myData.afr * 10 + 0.5f);
        lv_label_set_text_fmt(objects.ui_lbl_afr, "%d.%d", afr10 / 10, afr10 % 10);
        old_myData.afr = myData.afr;
      }
      xSemaphoreGive(uiMutex);
    }
    xSemaphoreGive(dataMutex);
  }
}

void midUpdate()
{
  if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
  {
    if (xSemaphoreTake(uiMutex, portMAX_DELAY) == pdTRUE)
    {
      // OIL press, shown in 100kPa
      if (myData.oilPress != old_myData.oilPress)
      {
        int op = (int)(myData.oilPress / 10 + 0.5f);
        lv_bar_set_value(objects.ui_bar_oil_press, (int)myData.oilPress, LV_ANIM_OFF);
        lv_label_set_text_fmt(objects.ui_lbl_oil_press, "%d.%d", op / 10, op % 10);
        old_myData.oilPress = myData.oilPress;
      }
      // MAP kPa
      if (myData.map != old_myData.map)
      {
        lv_bar_set_value(objects.ui_bar_map, (int)myData.map, LV_ANIM_OFF);
        lv_label_set_text_fmt(objects.ui_lbl_map, "%d", (int)(myData.map + 0.5f));
        old_myData.map = myData.map;
      }
      // Gear, 0: neutral
      if (myData.gear != old_myData.gear)
      {
        if (myData.gear <= 0)
          lv_label_set_text(objects.ui_lbl_gear, "N");
        else
          lv_label_set_text_fmt(objects.ui_lbl_gear, "%d", myData.gear);
        old_myData.gear = myData.gear;
      }
      xSemaphoreGive(uiMutex);
    }
    xSemaphoreGive(dataMutex);
  }
}

void slowUpdate()
{
  if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
  {
    if (xSemaphoreTake(uiMutex, portMAX_DELAY) == pdTRUE)
    {
      if (myData.clt != old_myData.clt)
      {
        lv_bar_set_value(objects.ui_bar_water_temp, myData.clt, LV_ANIM_OFF);
        lv_label_set_text_fmt(objects.ui_lbl_water_temp, "%d", myData.clt);
        old_myData.clt = myData.clt;
      }
      if (myData.iat != old_myData.iat)
      {
        lv_bar_set_value(objects.ui_bar_iat, myData.iat, LV_ANIM_OFF);
        lv_label_set_text_fmt(objects.ui_lbl_iat, "%d", myData.iat);
        old_myData.iat = myData.iat;
      }
      if (myData.oilTemp != old_myData.oilTemp)
      {
        lv_bar_set_value(objects.ui_bar_oil_temp, myData.oilTemp, LV_ANIM_OFF);
        lv_label_set_text_fmt(objects.ui_lbl_oil_temp, "%d", myData.oilTemp);
        old_myData.oilTemp = myData.oilTemp;
      }
      if (myData.Vbat != old_myData.Vbat)
      {
        int bv = (int)(myData.Vbat * 10 + 0.5f);
        lv_bar_set_value(objects.ui_bar_battery, bv, LV_ANIM_OFF);
        lv_label_set_text_fmt(objects.ui_lbl_battery, "%d.%d", bv / 10, bv % 10);
        old_myData.Vbat = myData.Vbat;
      }

      // warning lights, all on for the lamp check after power up
      if (!telltaleDone)
      {
        telltaleDone = lv_tick_get() - telltaleStart >= STARTUP_TELLTALE_MS;
      }
      if (telltaleDone)
      {
        if (myData.fuelLevel > WARN_FUEL_HIDE)
          warnFuel = false;
        else if (myData.fuelLevel < WARN_FUEL_SHOW)
          warnFuel = true;
        bool waterHot = myData.clt >= warningSet.clt;
        bool waterCold = myData.clt < WARN_WATER_COLD;
        bool oilPress = myData.oilPress < warningSet.oilPress;
        bool battery = myData.Vbat < warningSet.vBatt;
        setVisible(objects.ui_img_warn_water_hot, waterHot);
        setVisible(objects.ui_img_warn_water_cold, waterCold && !waterHot);
        setVisible(objects.ui_img_warn_oil_press, oilPress);
        setVisible(objects.ui_img_warn_battery, battery);
        setVisible(objects.ui_img_warn_fuel, warnFuel);
        setVisible(objects.ui_img_warn_master, waterHot || oilPress || battery || warnFuel);
      }
      xSemaphoreGive(uiMutex);
    }
    xSemaphoreGive(dataMutex);
  }
}

void getWarningsSet()
{
  preferences.begin("warn", true);
  warningSet.rpm = preferences.getInt("rpm", DEF_WARN_RPM);
  warningSet.iat = preferences.getInt("iat", DEF_WARN_IAT);
  warningSet.clt = preferences.getInt("clt", DEF_WARN_CLT);
  warningSet.oilTemp = preferences.getInt("oilT", DEF_WARN_OIL_T);
  warningSet.oilPress = preferences.getInt("oilP", DEF_WARN_OIL_P);
  warningSet.fuelPress = preferences.getInt("fuelP", DEF_WARN_FUEL_P);
  warningSet.vBatt = preferences.getFloat("vBatt", DEF_WARN_VBATT);
  warningSet.isTurbo = preferences.getBool("turbo", DEF_IS_TURBO);
  preferences.end();
#ifdef DEBUG
  Serial.printf("getWarningsSet > rpm: %d, iat: %d, clt: %d, oilT: %d, oilP: %d, fuelP: %d, vB: %0.1f, isTurbo: %d\n",
                warningSet.rpm, warningSet.iat, warningSet.clt, warningSet.oilTemp,
                warningSet.oilPress, warningSet.fuelPress, warningSet.vBatt, warningSet.isTurbo);
#endif
}

void updateWarningsSet()
{
#ifdef DEBUG
  Serial.println("updateWarningsSet> start");
#endif

  preferences.begin("warn", false);

  if (warningSet.rpm != preferences.getInt("rpm", DEF_WARN_RPM))
  {
    preferences.putInt("rpm", warningSet.rpm);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.rpm");
#endif
  }
  if (warningSet.iat != preferences.getInt("iat", DEF_WARN_IAT))
  {
    preferences.putInt("iat", warningSet.iat);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.iat");
#endif
  }
  if (warningSet.clt != preferences.getInt("clt", DEF_WARN_CLT))
  {
    preferences.putInt("clt", warningSet.clt);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.clt");
#endif
  }
  if (warningSet.oilTemp != preferences.getInt("oilT", DEF_WARN_OIL_T))
  {
    preferences.putInt("oilT", warningSet.oilTemp);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.oilTemp");
#endif
  }
  if (warningSet.oilPress != preferences.getInt("oilP", DEF_WARN_OIL_P))
  {
    preferences.putInt("oilP", warningSet.oilPress);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.oilPress");
#endif
  }
  if (warningSet.fuelPress != preferences.getInt("fuelP", DEF_WARN_FUEL_P))
  {
    preferences.putInt("fuelP", warningSet.fuelPress);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.fuelPress");
#endif
  }
  if (warningSet.vBatt != preferences.getFloat("vBatt", DEF_WARN_VBATT))
  {
    preferences.putFloat("vBatt", warningSet.vBatt);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.vBatt");
#endif
  }
  if (warningSet.isTurbo != preferences.getBool("turbo", DEF_IS_TURBO))
  {
    preferences.putBool("turbo", warningSet.isTurbo);
#ifdef DEBUG
    Serial.println("updateWarningsSet> warningSet.isTurbo");
#endif
  }
  preferences.end();
}

void preInitWarnScreen(bool def)
{
#ifdef DEBUG
  Serial.println("preInitWarnScreen > start");
#endif
  lv_label_set_text(ui_FWValLabel, DASH_TAG);
  lv_label_set_text_fmt(ui_rpmWarnVal, "%d0", warningSet.rpm);
  lv_label_set_text_fmt(ui_iatWarnVal, "%d", warningSet.iat);
  lv_label_set_text_fmt(ui_cltWarnVal, "%d", warningSet.clt);
  lv_label_set_text_fmt(ui_oilTWarnVal, "%d", warningSet.oilTemp);
  lv_label_set_text_fmt(ui_oilPWarnVal, "%.1f", warningSet.oilPress / 100.0);
  lv_label_set_text_fmt(ui_fuelPWarnVal, "%.1f", warningSet.fuelPress / 100.0);
  lv_label_set_text_fmt(ui_vBattWarnVal, "%.1f", warningSet.vBatt);
  if (warningSet.isTurbo)
  {
    lv_obj_add_state(ui_turboSwitch, LV_STATE_CHECKED);
#ifdef DEBUG
    Serial.println("add_state: turboSwitch 1");
#endif
  }
  else
  {
    lv_obj_clear_state(ui_turboSwitch, LV_STATE_CHECKED);
#ifdef DEBUG
    Serial.println("add_state: turboSwitch 0");
#endif
  }
}

void setDefaultWarnSet()
{
  preferences.begin("warn", false);
  preferences.putInt("rpm", DEF_WARN_RPM);
  preferences.putInt("iat", DEF_WARN_IAT);
  preferences.putInt("clt", DEF_WARN_CLT);
  preferences.putInt("oilT", DEF_WARN_OIL_T);
  preferences.putInt("oilP", DEF_WARN_OIL_P);
  preferences.putInt("fuelP", DEF_WARN_FUEL_P);
  preferences.putFloat("vBatt", DEF_WARN_VBATT);
  preferences.putBool("turbo", DEF_IS_TURBO);
  preferences.end();
  preInitWarnScreen(true);
}

void setTurbo()
{
#ifdef DEBUG
  Serial.printf("setIsTurbo > start (isTurbo= %d)\n", warningSet.isTurbo);
#endif

  warningSet.isTurbo = true;
  changeMapWidget = true;

#ifdef DEBUG
  Serial.printf("setIsTurbo > end (isTurbo= %d)\n", warningSet.isTurbo);
#endif
}

void setNA()
{
#ifdef DEBUG
  Serial.printf("setIsNaturalA > end (isTurbo= %d)\n", warningSet.isTurbo);
#endif

  warningSet.isTurbo = false;
  changeMapWidget = true;

#ifdef DEBUG
  Serial.printf("setIsNaturalA > end (isTurbo= %d)\n", warningSet.isTurbo);
#endif
}

void rpmWarnSet(bool up)
{
  if (up)
  {
    if (warningSet.rpm < 800)
    {
      warningSet.rpm += 10;
    }
  }
  else
  {
    if (warningSet.rpm > 100)
    {
      warningSet.rpm -= 10;
    }
  }
  lv_label_set_text_fmt(ui_rpmWarnVal, "%d0", warningSet.rpm);
}

void cltWarnSet(bool up)
{
  if (up)
  {
    if (warningSet.clt < 120)
    {
      warningSet.clt += 1;
    }
  }
  else
  {
    if (warningSet.clt > 0)
    {
      warningSet.clt -= 1;
    }
  }
  lv_label_set_text_fmt(ui_cltWarnVal, "%d", warningSet.clt);
}

void iatWarnSet(bool up)
{
  if (up)
  {
    if (warningSet.iat < 80)
    {
      warningSet.iat += 1;
    }
  }
  else
  {
    if (warningSet.iat > 0)
    {
      warningSet.iat -= 1;
    }
  }
  lv_label_set_text_fmt(ui_iatWarnVal, "%d", warningSet.iat);
}

void oilTWarnSet(bool up)
{
  if (up)
  {
    if (warningSet.oilTemp < 120)
    {
      warningSet.oilTemp += 1;
    }
  }
  else
  {
    if (warningSet.oilTemp > 0)
    {
      warningSet.oilTemp -= 1;
    }
  }
  lv_label_set_text_fmt(ui_oilTWarnVal, "%d", warningSet.oilTemp);
}

void oilPWarnSet(bool up)
{
  if (up)
  {
    if (warningSet.oilPress < 500)
    {
      warningSet.oilPress += 10;
    }
  }
  else
  {
    if (warningSet.oilPress > 0)
    {
      warningSet.oilPress -= 10;
    }
  }
  lv_label_set_text_fmt(ui_oilPWarnVal, "%.1f", warningSet.oilPress / 100.0);
}

void fuelPWarnSet(bool up)
{
  if (up)
  {
    if (warningSet.fuelPress < 500)
    {
      warningSet.fuelPress += 10;
    }
  }
  else
  {
    if (warningSet.fuelPress > 100)
    {
      warningSet.fuelPress -= 10;
    }
  }
  lv_label_set_text_fmt(ui_fuelPWarnVal, "%.1f", warningSet.fuelPress / 100.0);
}

void vBattWarnSet(bool up)
{
  if (up)
  {
    if (warningSet.vBatt < 16)
    {
      warningSet.vBatt += 0.1;
    }
  }
  else
  {
    if (warningSet.vBatt > 5)
    {
      warningSet.vBatt -= 0.1;
    }
  }
  lv_label_set_text_fmt(ui_vBattWarnVal, "%.1f", warningSet.vBatt);
}

void benchScreenSetup()
{
  if (canEngineConfig)
  {
    lv_obj_clear_flag(ui_EngSetup, LV_OBJ_FLAG_HIDDEN);
  }
}

void setCheckBoxDisplacement()
{
  switch (engineConfig.displacement)
  {
  case 48:
    lv_obj_add_state(ui_CheckboxDisp48, LV_STATE_CHECKED);
    break;
  case 53:
    lv_obj_add_state(ui_CheckboxDisp53, LV_STATE_CHECKED);
    break;
  case 57:
    lv_obj_add_state(ui_CheckboxDisp57, LV_STATE_CHECKED);
    break;
  case 60:
    lv_obj_add_state(ui_CheckboxDisp60, LV_STATE_CHECKED);
    break;
  case 62:
    lv_obj_add_state(ui_CheckboxDisp62, LV_STATE_CHECKED);
    break;
  case 70:
    lv_obj_add_state(ui_CheckboxDisp70, LV_STATE_CHECKED);
    break;
  default:
    lv_obj_add_state(ui_CheckboxDisp48, LV_STATE_CHECKED);
    engineConfig.displacement = 48;
  }
}

void setCheckBoxTrigger()
{
  switch (engineConfig.trigger)
  {
  case 24:
    lv_obj_add_state(ui_CheckboxTrig24, LV_STATE_CHECKED);
    break;
  case 58:
    lv_obj_add_state(ui_CheckboxTrig58, LV_STATE_CHECKED);
    break;
  default:
    lv_obj_add_state(ui_CheckboxTrig24, LV_STATE_CHECKED);
    engineConfig.trigger = 24;
  }
}

void setCheckBoxCamshape()
{
  switch (engineConfig.camshape)
  {
  case 1:
    lv_obj_add_state(ui_CheckboxCamshape1, LV_STATE_CHECKED);
    break;
  case 2:
    lv_obj_add_state(ui_CheckboxCamshape2, LV_STATE_CHECKED);
    break;
  case 4:
    lv_obj_add_state(ui_CheckboxCamshape4, LV_STATE_CHECKED);
    break;
  default:
    lv_obj_add_state(ui_CheckboxCamshape1, LV_STATE_CHECKED);
    engineConfig.camshape = 1;
  }
}

void engineSettingScreenSettup()
{
  setCheckBoxDisplacement();
  setCheckBoxTrigger();
  setCheckBoxCamshape();
}

void clearEngConf()
{
  clearCheckBoxDisplacement();
  clearCheckBoxTrig();
  clearCheckBoxCamS();
  checkEngineConfig = true;
}

void engSet()
{
  // uint8_t data[] = { 0x66, 0x00, 0x00, 0x00, 0x00 };
  uint8_t data[] = {(uint8_t)bench_test_magic_numbers_e::BENCH_HEADER, 0x00, 0x00, 0x00, 0x00};
  // 0x77000E 0x66 0x00 displ trigg camshape  -- setup engine configuraton
  data[2] = engineConfig.displacement;
  data[3] = engineConfig.trigger;
  data[4] = engineConfig.camshape;
  // can.send(0x77000E, data, sizeof(data), true);
  can.send((uint32_t)bench_test_packet_ids_e::ECU_CAN_BUS_SETTINGS_CONTROL, data, sizeof(data), true);
  checkEngineConfig = true;
}

void clearCheckBoxDisplacement()
{
  lv_obj_clear_state(ui_CheckboxDisp48, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxDisp53, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxDisp57, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxDisp60, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxDisp62, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxDisp70, LV_STATE_CHECKED);
}

void clearCheckBoxTrig()
{
  lv_obj_clear_state(ui_CheckboxTrig24, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxTrig58, LV_STATE_CHECKED);
}

void clearCheckBoxCamS()
{
  lv_obj_clear_state(ui_CheckboxCamshape1, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxCamshape2, LV_STATE_CHECKED);
  lv_obj_clear_state(ui_CheckboxCamshape4, LV_STATE_CHECKED);
}

void setEngDisp(int engDisp)
{
  clearCheckBoxDisplacement();
  engineConfig.displacement = engDisp;
  setCheckBoxDisplacement();
}

void setEngTrig(int trig)
{
  clearCheckBoxTrig();
  engineConfig.trigger = trig;
  setCheckBoxTrigger();
}

void setEngCamS(int cams)
{
  clearCheckBoxCamS();
  engineConfig.camshape = cams;
  setCheckBoxCamshape();
}