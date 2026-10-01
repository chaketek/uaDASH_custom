"""Replace the main screen of eez/uaDash.eez-project with the FULLMONI-WIDE
"eez002" dashboard design (800x256, https://github.com/tomoya723/FULLMONI-WIDE style).

Only the part used on the uaDASH is taken over: sensor gauges on the left,
tachometer, gear and AFR. Speed, clock, trip/odo, fuel gauge, the lambda table
and warnings without rusEFI data are left out. What is left only covers the
left 3/4 of the 800x256 design, so it is scaled up by BAND_SCALE (widgets,
fonts and images) to use the screen width, in a container centered on that part.

    python tools/eez_import_fullmoni.py <path to eez002.eez-project>
"""

import copy
import json
import os
import shutil
import sys
import uuid

from eez_build import scale_bitmap, scale_font, scale_widget

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PROJECT = os.path.join(ROOT, 'eez', 'uaDash.eez-project')

DESIGN_W, DESIGN_H = 800, 256
# horizontal extent of the kept widgets in the design (rendered), and the scale
# that makes it fill the 800px screen width
CONTENT_X0, CONTENT_X1 = 6, 607
BAND_SCALE = 1.3

# not shown on the uaDASH
DROP = {
    'ui_ImgBack7',        # right background: km/h, trip/total, fuel gauge
    'ui_LblSPD', 'ui_LblTrip', 'ui_LblODO', 'ui_LblTIME', 'ui_BarFUEL',
    'ui_lbl_diff_b1', 'ui_lbl_lambda_b1', 'ui_lbl_diff_b2', 'ui_lbl_lambda_b2',
    'ui_LblAFR_3', 'ui_LblAFR_4', 'ui_LblAFR_7', 'ui_LblAFR_8',
    'ui_ContainerOpening', # opening image
    'ui_NotifyBox',        # FULLMONI host access notification
    # telltales without rusEFI data (FULLMONI analog inputs)
    'ui_ImgWarnExhaust', 'ui_ImgWarnBrake', 'ui_ImageWarnBelt',
}


def new_id():
    return str(uuid.uuid4())


def filter_tree(w):
    w = copy.deepcopy(w)
    w['children'] = [filter_tree(c) for c in w.get('children', []) if c.get('identifier') not in DROP]
    return w


def collect(widgets, fonts, images):
    for w in widgets:
        for sts in w.get('localStyles', {}).get('definition', {}).values():
            for props in sts.values():
                if 'text_font' in props:
                    fonts.add(props['text_font'])
        if w.get('image'):
            images.add(w['image'])
        collect(w.get('children', []), fonts, images)


def main():
    src = json.load(open(sys.argv[1], encoding='utf-8'))
    project = json.load(open(PROJECT, encoding='utf-8'))

    src_screen = src['userPages'][0]['components'][0]
    band_children = [filter_tree(c) for c in src_screen['children'] if c.get('identifier') not in DROP]
    for c in band_children:
        scale_widget(c, False, BAND_SCALE)
    band_w, band_h = round(DESIGN_W * BAND_SCALE), round(DESIGN_H * BAND_SCALE)
    # the band is centered, shift it so the kept part is in the middle of the screen
    band_x = round((DESIGN_W / 2 - (CONTENT_X0 + CONTENT_X1) / 2) * BAND_SCALE)

    old_page = next(p for p in project['userPages'] if p['name'] == 'main_screen')
    old_screen = old_page['components'][0]

    band = {
        'objID': new_id(), 'type': 'LVGLContainerWidget',
        'left': band_x, 'top': 0, 'width': band_w, 'height': band_h,
        'customInputs': [], 'customOutputs': [],
        'style': {'objID': new_id(), 'useStyle': 'default', 'conditionalStyles': [], 'childStyles': []},
        'timeline': [], 'eventHandlers': [], 'identifier': 'dashboardBand',
        'leftUnit': 'px', 'topUnit': 'px', 'widthUnit': 'px', 'heightUnit': 'px',
        'children': band_children,
        'widgetFlags': 'GESTURE_BUBBLE|PRESS_LOCK|SNAPPABLE',
        'hiddenFlag': False, 'hiddenFlagType': 'literal',
        'clickableFlag': False, 'clickableFlagType': 'literal',
        'flagScrollbarMode': '', 'flagScrollDirection': '', 'scrollSnapX': '', 'scrollSnapY': '',
        'checkedStateType': 'literal', 'disabledStateType': 'literal', 'states': '',
        'group': '', 'groupIndex': 0,
        'localStyles': {'objID': new_id(), 'definition': {'MAIN': {'DEFAULT': {'align': 'CENTER'}}}},
    }

    screen = copy.deepcopy(old_screen)
    screen['children'] = [band]
    screen['localStyles']['definition'] = {'MAIN': {'DEFAULT': {'bg_color': '#000000', 'bg_opa': 255}}}
    old_page['components'] = [screen]

    fonts, images = set(), set()
    collect(band_children, fonts, images)
    # replace the fonts and images of an earlier import
    src_fonts = {f['name'] for f in src['fonts']}
    src_images = {b['name'] for b in src['bitmaps']}
    project['fonts'] = [f for f in project['fonts'] if f['name'] not in src_fonts]
    project['bitmaps'] = [b for b in project['bitmaps'] if b['name'] not in src_images]
    have_fonts = {f['name'] for f in project['fonts']}
    have_images = {b['name'] for b in project['bitmaps']}
    src_dir = os.path.dirname(os.path.abspath(sys.argv[1]))
    for f in src['fonts']:
        if f['name'] not in fonts or f['name'] in have_fonts:
            continue
        f = copy.deepcopy(f)
        # the font files are next to the FULLMONI project, keep a copy with this project
        font_file = os.path.basename(f['source']['filePath'])
        dst = os.path.join(os.path.dirname(PROJECT), 'assets', 'fonts', font_file)
        if not os.path.exists(dst):
            shutil.copyfile(os.path.join(src_dir, font_file), dst)
        f['source']['filePath'] = 'assets/fonts/' + font_file
        scale_font(f, BAND_SCALE)
        project['fonts'].append(f)
    for b in src['bitmaps']:
        if b['name'] in images and b['name'] not in have_images:
            b = copy.deepcopy(b)
            scale_bitmap(b, BAND_SCALE)
            project['bitmaps'].append(b)

    with open(PROJECT, 'w', encoding='utf-8', newline='\n') as f:
        json.dump(project, f, indent=2, ensure_ascii=False)
    print('main_screen <- %s: fonts %s, images %s' % (os.path.basename(sys.argv[1]), sorted(fonts), sorted(images)))


if __name__ == '__main__':
    main()
