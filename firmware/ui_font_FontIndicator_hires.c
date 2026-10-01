/*******************************************************************************
 * Size: 22 px
 * Bpp: 1
 * Opts: --bpp 1 --size 22 --font Fussion-3zgAz.ttf -o ui_font_FontIndicator.c --format lvgl --no-compress --no-prefilter --symbols=-.0123456789
 ******************************************************************************/

#include "ui.h"

#ifndef UI_FONT_FONTINDICATOR
#define UI_FONT_FONTINDICATOR 1
#endif

#if UI_FONT_FONTINDICATOR && defined(UI_HIRES)

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+002D "-" */
    0xff, 0xff, 0xff,

    /* U+002E "." */
    0xff, 0xfe,

    /* U+0030 "0" */
    0xf, 0xff, 0xe1, 0xff, 0xff, 0x3e, 0x1, 0xf7,
    0xc0, 0x1f, 0xf8, 0x1, 0xff, 0x80, 0x1f, 0xf8,
    0x1, 0xff, 0x80, 0x1f, 0xf8, 0x1, 0xff, 0x80,
    0x1f, 0xf8, 0x1, 0xff, 0x80, 0x1f, 0xf8, 0x3,
    0xff, 0xff, 0xfe, 0x7f, 0xff, 0xc0,

    /* U+0031 "1" */
    0xfe, 0xff, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
    0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,

    /* U+0032 "2" */
    0x7f, 0xff, 0xc7, 0xff, 0xfe, 0x0, 0x3, 0xf0,
    0x0, 0x1f, 0x0, 0x1, 0xf0, 0x0, 0x1f, 0x3f,
    0xff, 0xf7, 0xff, 0xfe, 0xfc, 0x0, 0xf, 0x80,
    0x0, 0xf8, 0x0, 0xf, 0x80, 0x0, 0xf8, 0x0,
    0xf, 0xff, 0xfe, 0xff, 0xff, 0xe0,

    /* U+0033 "3" */
    0xff, 0xff, 0xcf, 0xff, 0xfe, 0x0, 0x3, 0xf0,
    0x0, 0x1f, 0x0, 0x1, 0xf0, 0x0, 0x1f, 0x3f,
    0xff, 0xe3, 0xff, 0xfe, 0x0, 0x1, 0xf0, 0x0,
    0x1f, 0x0, 0x1, 0xf0, 0x0, 0x1f, 0x0, 0x3,
    0xff, 0xff, 0xfe, 0xff, 0xff, 0xc0,

    /* U+0034 "4" */
    0x3, 0xff, 0xf0, 0x7f, 0xff, 0xf, 0x81, 0xf1,
    0xf0, 0x1f, 0x3e, 0x1, 0xf7, 0xc0, 0x1f, 0xf8,
    0x1, 0xff, 0x80, 0x1f, 0xf8, 0x1, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xf0, 0x0, 0x1f, 0x0, 0x1,
    0xf0, 0x0, 0x1f, 0x0, 0x1, 0xf0,

    /* U+0035 "5" */
    0x7f, 0xff, 0xef, 0xff, 0xfe, 0xf8, 0x0, 0xf,
    0x80, 0x0, 0xf8, 0x0, 0xf, 0x80, 0x0, 0xff,
    0xff, 0xf7, 0xff, 0xff, 0x0, 0x1, 0xf0, 0x0,
    0x1f, 0x0, 0x1, 0xf0, 0x0, 0x1f, 0x0, 0x3,
    0xf7, 0xff, 0xfe, 0x7f, 0xff, 0xc0,

    /* U+0036 "6" */
    0xf, 0xff, 0xe1, 0xff, 0xfe, 0x3e, 0x0, 0x7,
    0xc0, 0x0, 0xf8, 0x0, 0xf, 0x80, 0x0, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xf8, 0x1, 0xff, 0x80,
    0x1f, 0xf8, 0x1, 0xff, 0x80, 0x1f, 0xf8, 0x3,
    0xff, 0xff, 0xfe, 0x7f, 0xff, 0xc0,

    /* U+0037 "7" */
    0xff, 0xff, 0xff, 0xff, 0xf0, 0x0, 0x7c, 0x0,
    0x1f, 0x0, 0x7, 0xc0, 0x3, 0xf0, 0x1, 0xf8,
    0x0, 0xfc, 0x0, 0x7e, 0x0, 0x3f, 0x0, 0x1f,
    0x80, 0xf, 0xc0, 0x7, 0xe0, 0x3, 0xf0, 0x1,
    0xf8, 0x0,

    /* U+0038 "8" */
    0x3f, 0xff, 0xc3, 0xff, 0xff, 0xbf, 0x0, 0x7f,
    0xf0, 0x1, 0xff, 0x80, 0xf, 0xfc, 0x0, 0x7f,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0, 0xff,
    0xc0, 0x7, 0xfe, 0x0, 0x3f, 0xf0, 0x1, 0xf7,
    0xc0, 0x1f, 0xbf, 0xff, 0xf8, 0xff, 0xff, 0x80,

    /* U+0039 "9" */
    0x3f, 0xff, 0xc7, 0xff, 0xfe, 0xfc, 0x3, 0xff,
    0x80, 0x1f, 0xf8, 0x1, 0xff, 0x80, 0x1f, 0xff,
    0xff, 0xf7, 0xff, 0xff, 0x0, 0x1, 0xf0, 0x0,
    0x1f, 0x0, 0x1, 0xf0, 0x0, 0x1f, 0x0, 0x3,
    0xf7, 0xff, 0xfe, 0x7f, 0xff, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 233, .box_w = 12, .box_h = 2, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 3, .adv_w = 117, .box_w = 5, .box_h = 3, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 5, .adv_w = 362, .box_w = 20, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 164, .box_w = 8, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 362, .box_w = 20, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 350, .box_w = 20, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 361, .box_w = 20, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 362, .box_w = 20, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 362, .box_w = 20, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 248, .adv_w = 327, .box_w = 18, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 282, .adv_w = 373, .box_w = 21, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 362, .box_w = 20, .box_h = 15, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint8_t glyph_id_ofs_list_0[] = {
    0, 1, 0, 2, 3, 4, 5, 6,
    7, 8, 9, 10, 11
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 45, .range_length = 13, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = glyph_id_ofs_list_0, .list_length = 13, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 0, 1, 2, 3, 4,
    5, 6, 7, 3, 8
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 0, 1, 2, 3, 4, 5,
    6, 1, 7, 8, 9
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    -2, 0, -4, -6, -6, -3, -13, -4,
    -4, -7, -16, 0, -14, -13, -6, -18,
    -11, -7, -4, -15, -7, 0, -10, -6,
    -18, -6, -7, -2, -12, -4, -6, 0,
    -3, -13, -4, -4, -5, -16, -8, -13,
    -8, 0, -18, -6, -13, -5, -16, -8,
    -13, -8, -12, -18, -6, -13, -12, -13,
    -15, -13, -17, -7, 0, -14, -10, -3,
    -15, -7, -13, -7, -5, -18, -5, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 8,
    .right_class_cnt     = 9,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ui_font_FontIndicator = {
#else
lv_font_t ui_font_FontIndicator = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UI_FONT_FONTINDICATOR*/

