"""Replace the main screen of eez/uaDash.eez-project with the FULLMONI-WIDE
"eez002" dashboard design (800x256, https://github.com/tomoya723/FULLMONI-WIDE style).

Only the part used on the uaDASH is taken over: sensor gauges on the left,
tachometer, gear and AFR. Speed, clock, trip/odo, fuel gauge, the lambda table
and warnings without rusEFI data are left out. The 800x256 design is put
unchanged into a container in the middle of the 800x480 screen.

    python tools/eez_import_fullmoni.py <path to eez002.eez-project>
"""

import copy
import json
import os
import shutil
import sys
import uuid

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PROJECT = os.path.join(ROOT, 'eez', 'uaDash.eez-project')

DESIGN_W, DESIGN_H = 800, 256

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

    old_page = next(p for p in project['userPages'] if p['name'] == 'main_screen')
    old_screen = old_page['components'][0]

    band = {
        'objID': new_id(), 'type': 'LVGLContainerWidget',
        'left': 0, 'top': 0, 'width': DESIGN_W, 'height': DESIGN_H,
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
        project['fonts'].append(f)
    project['bitmaps'] += [b for b in src['bitmaps'] if b['name'] in images and b['name'] not in have_images]

    with open(PROJECT, 'w', encoding='utf-8', newline='\n') as f:
        json.dump(project, f, indent=2, ensure_ascii=False)
    print('main_screen <- %s: fonts %s, images %s' % (os.path.basename(sys.argv[1]), sorted(fonts), sorted(images)))


if __name__ == '__main__':
    main()
