#pragma once

// UI is designed (SquareLine) for 800x480. Boards with a larger panel scale
// coordinates and use bigger fonts (ui_font_*_hires.c) when UI_HIRES is set.

#if defined(WAVESHARE_S3_LCD7B)
#define UI_HIRES
#define UI_SCREEN_ANIM LV_SCR_LOAD_ANIM_FADE_ON  // replaces move animations
#define UI_HOR_RES 1024
#define UI_VER_RES 600
#else
#define UI_HOR_RES 800
#define UI_VER_RES 480
#endif

#define UI_DESIGN_HOR_RES 800
#define UI_DESIGN_VER_RES 480

// rounded scaling, identity on 800x480 boards
#define UI_SCALE_(v, num, den) ((lv_coord_t)(((v) * (num) + ((v) >= 0 ? (den) / 2 : -(den) / 2)) / (den)))
#define UI_SX(v) UI_SCALE_(v, UI_HOR_RES, UI_DESIGN_HOR_RES)
#define UI_SY(v) UI_SCALE_(v, UI_VER_RES, UI_DESIGN_VER_RES)
#define UI_S(v)  UI_SY(v)  // non-directional sizes (radius, outline): smaller factor

#ifdef UI_HIRES
#define UI_FONT_MONTSERRAT_26 lv_font_montserrat_32
#else
#define UI_FONT_MONTSERRAT_26 lv_font_montserrat_26
#endif
