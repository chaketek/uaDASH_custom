"""Convert the SquareLine Studio generated UI (squareline_prj/export/ui_*Screen.c) into an
EEZ Studio LVGL project (eez/uaDash.eez-project).

The layout is taken at the 800x480 design size (UI_SX/UI_SY scaling macros are
ignored). Fonts are replaced by the HeadUpDaisy pixel font (see FONT_MAP), the
SquareLine event handlers become EEZ native actions (see eez/actions.md).

Requirements: python 3, deno (runs lv_font_conv), the font file FONT_FILE.

    python tools/squareline_to_eez.py
"""

import base64
import copy
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import uuid

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FIRMWARE = os.path.join(ROOT, 'squareline_prj', 'export')  # SquareLine export (no longer used by the firmware)
OUT_DIR = os.path.join(ROOT, 'eez')
OUT_PROJECT = os.path.join(OUT_DIR, 'uaDash.eez-project')
TEMPLATE = os.environ.get('EEZ_TEMPLATE')  # an LVGL 8.4 .eez-project (EEZ example "Meter")

FONT_FILE = os.environ.get(
    'HUD_FONT', os.path.expandvars(r'%LOCALAPPDATA%\Microsoft\Windows\Fonts\x14y24pxHeadUpDaisy.ttf'))
FONT_REL_PATH = 'assets/fonts/x14y24pxHeadUpDaisy.ttf'

DISPLAY_W, DISPLAY_H = 800, 480

SCREENS = [
    ('ui_mainScreen.c', 'ui_mainScreen', 'main_screen'),
    ('ui_benchScreen.c', 'ui_benchScreen', 'bench_screen'),
    ('ui_settingsScreen.c', 'ui_settingsScreen', 'settings_screen'),
    ('ui_engineConfigScreen.c', 'ui_engineConfigScreen', 'engine_config_screen'),
]

# HeadUpDaisy is a 14x24 dot font (1200 units per em, 50 units per dot): sizes that are
# multiples of 24px keep every dot on whole pixels.
HUD_FONTS = {
    # name: (size px, bpp, ranges, symbols)
    'hud_12': (12, 4, '0x20-0x7e', ''),
    'hud_24': (24, 4, '0x20-0x7e', ''),
    'hud_168': (168, 4, '', '0123456789'),
}
FONT_MAP = {
    'ui_font_FontLabel': 'hud_24',          # 16px labels, buttons
    'ui_font_FontIndicator': 'hud_24',      # 18px values
    'ui_font_FontRPM': 'hud_24',            # 26px gear numbers, rpm value
    'UI_FONT_MONTSERRAT_26': 'hud_24',      # settings values
    'lv_font_montserrat_26': 'hud_24',
    'ui_font_FontIndicatorLabel': 'hud_12', # 10px scale ticks
    'ui_font_FontSpeed': 'hud_168',         # 160px speed (digit height ~112px)
}
HUD_CHAR_W = 14 / 24  # character advance per px of font size
# SquareLine fonts that only have capital letters (lower case looks the same):
# texts using them are upper-cased to keep the look with HeadUpDaisy
CAPS_ONLY_FONTS = {'ui_font_FontLabel', 'ui_font_FontRPM', 'ui_font_FontSpeed'}

# LVGL 8 default flags per widget (CLICKABLE and HIDDEN are separate properties in EEZ)
BASE_FLAGS = ['CLICK_FOCUSABLE', 'GESTURE_BUBBLE', 'PRESS_LOCK', 'SCROLLABLE', 'SCROLL_CHAIN_HOR',
              'SCROLL_CHAIN_VER', 'SCROLL_ELASTIC', 'SCROLL_MOMENTUM', 'SCROLL_WITH_ARROW', 'SNAPPABLE']
WIDGETS = {
    # SquareLine create function: (EEZ type, flags, clickable)
    'screen': ('LVGLScreenWidget', BASE_FLAGS, True),
    'container': ('LVGLContainerWidget', BASE_FLAGS, True),
    'obj': ('LVGLPanelWidget', BASE_FLAGS, True),
    'label': ('LVGLLabelWidget', BASE_FLAGS, False),
    'btn': ('LVGLButtonWidget', [f for f in BASE_FLAGS if f != 'SCROLLABLE'] + ['SCROLL_ON_FOCUS'], True),
    'bar': ('LVGLBarWidget', [f for f in BASE_FLAGS if f != 'SCROLLABLE'], True),
    'checkbox': ('LVGLCheckboxWidget', BASE_FLAGS + ['CHECKABLE', 'SCROLL_ON_FOCUS'], True),
    'switch': ('LVGLSwitchWidget', [f for f in BASE_FLAGS if f != 'SCROLLABLE'] + ['CHECKABLE', 'SCROLL_ON_FOCUS'], True),
}

warnings = []


def new_id():
    return str(uuid.uuid4())


def snake(name):
    s = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', name)
    s = re.sub(r'([A-Z]+)([A-Z][a-z])', r'\1_\2', s)
    return s.lower()


# ---------------------------------------------------------------- C parsing

def strip_comments(s):
    s = re.sub(r'/\*.*?\*/', '', s, flags=re.S)
    return re.sub(r'//[^\n]*', '', s)


def c_string(lit):
    escapes = {'n': '\n', 't': '\t'}
    return re.sub(r'\\(.)', lambda m: escapes.get(m.group(1), m.group(1)), lit[1:-1])


def parse_value(v):
    v = v.strip()
    m = re.fullmatch(r'UI_S[XY]?\((-?\d+)\)', v)
    if m:
        return int(m.group(1))
    if re.fullmatch(r'-?\d+', v):
        return int(v)
    if v == 'LV_SIZE_CONTENT':
        return 'content'
    m = re.fullmatch(r'lv_pct\((-?\d+)\)', v)
    if m:
        return ('%', int(m.group(1)))
    m = re.fullmatch(r'lv_color_hex\(0x([0-9A-Fa-f]{1,6})\)', v)
    if m:
        return '#%06x' % int(m.group(1), 16)
    m = re.fullmatch(r'&(\w+)', v)
    if m:
        font = FONT_MAP.get(m.group(1))
        if font is None:
            warnings.append('unknown font %s' % m.group(1))
        return ('font', font or m.group(1))
    m = re.fullmatch(r'LV_(?:TEXT_ALIGN|GRAD_DIR|BORDER_SIDE|BASE_DIR)_(\w+)', v)
    if m:
        return m.group(1)
    if v in ('true', 'false'):
        return v == 'true'
    if v.startswith('"'):
        return c_string(v)
    raise ValueError('value %r' % v)


def parse_selector(sel):
    parts = [p.strip() for p in sel.split('|')]
    part = next((p[len('LV_PART_'):] for p in parts if p.startswith('LV_PART_')), 'MAIN')
    states = [p[len('LV_STATE_'):] for p in parts if p.startswith('LV_STATE_')]
    return part, '|'.join(states) if states else 'DEFAULT'


def split_args(args):
    out, depth, cur = [], 0, ''
    for ch in args:
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
            continue
        depth += ch == '('
        depth -= ch == ')'
        cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def parse_events(src):
    """ui_event_X() -> list of (event code, gesture dir, called functions)"""
    events = {}
    for m in re.finditer(r'void (ui_event_\w+)\(lv_event_t ?\* ?e\)\s*\{(.*?)\n\}', src, re.S):
        body = m.group(2)
        branches = re.split(r'\n\s*if\s*\(', body)[1:]
        lst = []
        for b in branches:
            code = re.search(r'event_code == LV_EVENT_(\w+)', b)
            gdir = re.search(r'gesture_dir\(lv_indev_get_act\(\)\) == LV_DIR_(\w+)', b)
            calls = [c for c in re.findall(r'\b([a-zA-Z_]\w*)\(e\)', b) if c != 'lv_event_get_code']
            screen = re.search(r'_ui_screen_change\(&ui_(\w+)', b)
            lst.append((code.group(1) if code else None, gdir.group(1) if gdir else None, calls,
                        screen.group(1) if screen else None))
        events[m.group(1)] = lst
    return events


def parse_screen(fname, screen_var):
    src = open(os.path.join(FIRMWARE, fname), encoding='utf-8').read()
    events = parse_events(src)
    init = re.search(r'void %s_screen_init\(void\)\s*\{(.*?)\n\}' % screen_var, src, re.S).group(1)
    objs, order = {}, []
    for stmt in strip_comments(init).split(';'):
        stmt = ' '.join(stmt.split())
        if not stmt:
            continue
        m = re.fullmatch(r'(ui_\w+) = lv_(\w+)_create\((\w+)\)', stmt)
        if m:
            name, kind, parent = m.groups()
            kind = 'screen' if parent == 'NULL' else kind
            objs[name] = {'name': name, 'kind': kind, 'parent': None if parent == 'NULL' else parent,
                          'styles': {}, 'add': [], 'clear': [], 'events': []}
            order.append(name)
            continue
        m = re.fullmatch(r'(\w+)\((ui_\w+)(?:, (.*))?\)', stmt)
        if not m:
            warnings.append('%s: skipped %s' % (fname, stmt))
            continue
        fn, name, rest = m.group(1), m.group(2), m.group(3) or ''
        o = objs.get(name)
        if o is None:
            warnings.append('%s: unknown object in %s' % (fname, stmt))
            continue
        args = split_args(rest)
        if fn == 'lv_obj_remove_style_all':
            o['kind'] = 'container' if o['kind'] == 'obj' else o['kind']
        elif fn in ('lv_obj_set_width', 'lv_obj_set_height', 'lv_obj_set_x', 'lv_obj_set_y'):
            o[fn.rsplit('_', 1)[1]] = parse_value(args[0])
        elif fn == 'lv_obj_set_align':
            o['align'] = args[0].replace('LV_ALIGN_', '')
        elif fn in ('lv_label_set_text', 'lv_checkbox_set_text'):
            o['text'] = parse_value(args[0])
        elif fn == 'lv_label_set_long_mode':
            o['longMode'] = args[0].replace('LV_LABEL_LONG_', '')
        elif fn in ('lv_obj_add_flag', 'lv_obj_clear_flag'):
            flags = [f.strip().replace('LV_OBJ_FLAG_', '') for f in args[0].split('|')]
            o['add' if fn == 'lv_obj_add_flag' else 'clear'] += flags
        elif fn in ('lv_obj_add_state', 'lv_obj_clear_state'):
            if fn == 'lv_obj_add_state':
                o.setdefault('states', []).extend(s.strip().replace('LV_STATE_', '') for s in args[0].split('|'))
        elif fn.startswith('lv_obj_set_style_'):
            prop = fn[len('lv_obj_set_style_'):]
            part, state = parse_selector(args[1])
            val = parse_value(args[0])
            if isinstance(val, tuple) and val[0] == 'font':
                val = val[1]
                if (part, state) == ('MAIN', 'DEFAULT'):
                    o['src_font'] = args[0].lstrip('&')
            o['styles'].setdefault(part, {}).setdefault(state, {})[prop] = val
        elif fn == 'lv_bar_set_range':
            o['min'], o['max'] = int(args[0]), int(args[1])
        elif fn == 'lv_bar_set_value':
            o['value'] = int(args[0])
        elif fn == 'lv_bar_set_start_value':
            o['valueStart'] = int(args[0])
        elif fn == 'lv_bar_set_mode':
            o['mode'] = args[0].replace('LV_BAR_MODE_', '')
        elif fn == 'lv_obj_add_event_cb':
            o['events'].append(events.get(args[0], []))
        else:
            warnings.append('%s: unhandled %s' % (fname, stmt))
    return objs, order


# ---------------------------------------------------------------- EEZ project

actions = {}      # action name -> description for actions.md


def add_action(name, desc):
    actions.setdefault(name, desc)
    return name


def event_handlers(o, screen_name):
    out = []
    for branches in o['events']:
        codes = {b[0] for b in branches}
        for code in sorted(codes):
            bs = [b for b in branches if b[0] == code]
            if code == 'GESTURE':
                desc = '; '.join('%s: %s' % (b[1], ', '.join(
                    (['load ' + b[3]] if b[3] else []) + b[2]) or '-') for b in bs)
                name = add_action('%s_gesture' % screen_name, 'swipe on %s -> %s' % (screen_name, desc))
            elif code == 'VALUE_CHANGED':
                calls = sorted({c for b in bs for c in b[2]})
                name = add_action('%s_changed' % snake(o['name'][3:]),
                                  '%s value changed -> %s (by checked state)' % (o['name'], ', '.join(calls)))
            else:
                calls = [c for b in bs for c in b[2]]
                if len(calls) != 1:
                    warnings.append('%s %s calls %s' % (o['name'], code, calls))
                loads = ['load ' + b[3] for b in bs if b[3]]
                name = add_action(snake(calls[0]) if calls else snake(o['name'][3:]) + '_' + code.lower(),
                                  '%s %s -> %s' % (o['name'], code, ', '.join(loads + calls)))
            out.append({'objID': new_id(), 'eventName': code, 'handlerType': 'action',
                        'action': name, 'userData': 0})
    return out


def text_width(o, font):
    size = HUD_FONTS[font][0] if font in HUD_FONTS else 14
    return int(round(len(o.get('text', '')) * size * HUD_CHAR_W))


def font_of(o):
    return o['styles'].get('MAIN', {}).get('DEFAULT', {}).get('text_font')


def size_prop(o, key, default):
    v = o.get(key, default)
    if v == 'content':
        font = font_of(o)
        if key == 'width':
            return text_width(o, font), 'content'
        return (HUD_FONTS[font][0] if font in HUD_FONTS else 16), 'content'
    if isinstance(v, tuple):
        return v[1], '%'
    return v, 'px'


def make_widget(o, objs, screen_name):
    eez_type, defaults, clickable = WIDGETS[o['kind']]
    flags = set(defaults)
    for f in o['add']:
        if f == 'CLICKABLE':
            clickable = True
        elif f != 'HIDDEN':
            flags.add(f)
    for f in o['clear']:
        if f == 'CLICKABLE':
            clickable = False
        else:
            flags.discard(f)
    if o['kind'] == 'screen':
        w, wu, h, hu = DISPLAY_W, 'px', DISPLAY_H, 'px'
    else:
        w, wu = size_prop(o, 'width', 'content' if o['kind'] in ('label', 'checkbox') else 100)
        h, hu = size_prop(o, 'height', 'content' if o['kind'] in ('label', 'checkbox') else 50)

    styles = copy.deepcopy(o['styles'])
    if o.get('align'):
        styles.setdefault('MAIN', {}).setdefault('DEFAULT', {})['align'] = o['align']

    widget = {
        'objID': new_id(), 'type': eez_type,
        'left': o.get('x', 0), 'top': o.get('y', 0), 'width': w, 'height': h,
        'customInputs': [], 'customOutputs': [],
        'style': {'objID': new_id(), 'useStyle': 'default', 'conditionalStyles': [], 'childStyles': []},
        'timeline': [], 'eventHandlers': event_handlers(o, screen_name),
        'identifier': o['name'][3:],
        'leftUnit': 'px', 'topUnit': 'px', 'widthUnit': wu, 'heightUnit': hu,
        'children': [],
        'widgetFlags': '|'.join(sorted(flags)),
        'hiddenFlag': 'HIDDEN' in o['add'], 'hiddenFlagType': 'literal',
        'clickableFlag': clickable, 'clickableFlagType': 'literal',
        'flagScrollbarMode': '', 'flagScrollDirection': '', 'scrollSnapX': '', 'scrollSnapY': '',
        'checkedState': 'CHECKED' in o.get('states', []), 'checkedStateType': 'literal',
        'disabledStateType': 'literal', 'states': '',
        'group': '', 'groupIndex': 0,
        'localStyles': {'objID': new_id(), 'definition': styles},
    }
    if o.get('src_font') in CAPS_ONLY_FONTS and 'text' in o:
        o['text'] = o['text'].upper()
    if o['kind'] == 'label':
        font = font_of(o)
        if wu == 'px' and o.get('longMode') == 'CLIP' and font in HUD_FONTS and text_width(o, font) > w:
            warnings.append('%s: "%s" widened %dpx -> %dpx to fit the font'
                            % (o['name'], o.get('text'), w, text_width(o, font)))
            widget['width'] = text_width(o, font)
        if hu == 'px' and font in HUD_FONTS and h < HUD_FONTS[font][0]:
            # the SquareLine fonts had tight line heights, don't clip the HUD glyphs
            widget['height'], widget['heightUnit'] = HUD_FONTS[font][0], 'content'
        widget.update({'text': o.get('text', ''), 'textType': 'literal',
                       'longMode': o.get('longMode', 'WRAP'), 'recolor': False, 'previewValue': ''})
    elif o['kind'] == 'checkbox':
        widget['text'] = o.get('text', '')
    elif o['kind'] == 'bar':
        widget.update({'min': o.get('min', 0), 'max': o.get('max', 100), 'mode': o.get('mode', 'NORMAL'),
                       'value': o.get('value', 0), 'valueType': 'literal',
                       'valueStart': o.get('valueStart', 0), 'valueStartType': 'literal'})
    for child in [c for c in objs.values() if c['parent'] == o['name']]:
        widget['children'].append(make_widget(child, objs, screen_name))
    return widget


def make_page(fname, screen_var, page_name):
    objs, order = parse_screen(fname, screen_var)
    screen = make_widget(objs[screen_var], objs, page_name)
    return {
        'objID': new_id(), 'components': [screen], 'connectionLines': [], 'localVariables': [],
        'componentGroups': [], 'userProperties': [], 'name': page_name,
        'left': 0, 'top': 0, 'width': DISPLAY_W, 'height': DISPLAY_H,
        'createAtStart': True, 'deleteOnScreenUnload': False,
    }, len(order)


# ---------------------------------------------------------------- fonts

def make_font(name, size, bpp, ranges, symbols):
    tmp = tempfile.mkdtemp()
    base = ['deno', 'run', '-A', 'npm:lv_font_conv@1.5.2', '--font', FONT_FILE, '--size', str(size),
            '--bpp', str(bpp), '--no-compress', '--no-prefilter']
    if ranges:
        base += ['-r', ranges]
    if symbols:
        base += ['--symbols=' + symbols]
    c_path = os.path.join(tmp, 'ui_font_%s.c' % name)
    bin_path = os.path.join(tmp, 'ui_font_%s.bin' % name)
    subprocess.run(base + ['--format', 'lvgl', '-o', c_path], check=True, capture_output=True)
    subprocess.run(base + ['--format', 'bin', '-o', bin_path], check=True, capture_output=True)
    c_src = open(c_path, encoding='utf-8').read().replace(tmp + os.sep, '').replace(FONT_FILE, FONT_REL_PATH)
    line_height = int(re.search(r'\.line_height = (\d+)', c_src).group(1))
    base_line = int(re.search(r'\.base_line = (-?\d+)', c_src).group(1))
    encodings = []
    for r in filter(None, ranges.split(',')):
        a, b = (int(x, 16) for x in r.split('-'))
        encodings.append({'from': a, 'to': b})
    return {
        'objID': new_id(), 'name': name, 'renderingEngine': 'LVGL',
        'source': {'objID': new_id(), 'filePath': FONT_REL_PATH, 'size': size},
        'embeddedFontFile': base64.b64encode(open(FONT_FILE, 'rb').read()).decode(),
        'bpp': bpp, 'threshold': 128,
        'height': line_height, 'ascent': line_height - base_line, 'descent': base_line,
        'lvglGlyphs': {'encodings': encodings, 'symbols': symbols},
        'lvglBinFile': base64.b64encode(open(bin_path, 'rb').read()).decode(),
        'lvglSourceFile': c_src,
    }


# ---------------------------------------------------------------- main

def main():
    if not TEMPLATE:
        sys.exit('set EEZ_TEMPLATE to an LVGL 8.4 example project (eez-project-examples/examples/LVGL/Meter.eez-project)')
    project = json.load(open(TEMPLATE, encoding='utf-8'))
    general = project['settings']['general']
    general.update({'lvglVersion': '8.4.0', 'flowSupport': False,
                    'displayWidth': DISPLAY_W, 'displayHeight': DISPLAY_H,
                    'description': 'uaDASH rusEFI dashboard (converted from SquareLine by tools/squareline_to_eez.py)'})
    general.pop('image', None)
    project['settings']['build']['destinationFolder'] = '../firmware/src/ui_eez/800x480'  # see tools/eez_build.py
    project['settings']['build']['lvglInclude'] = 'lvgl.h'  # as in the firmware (Arduino/ESP-IDF)
    for f in project['settings']['build']['files']:
        # the template's ui.c starts its own first screen
        f['template'] = f['template'].replace('loadScreen(SCREEN_ID_MAIN);', 'loadScreen(SCREEN_ID_MAIN_SCREEN);')
    project['variables'] = {'objID': new_id(), 'globalVariables': [], 'structures': [], 'enums': []}
    project['bitmaps'] = []

    pages, total = [], 0
    for fname, var, page in SCREENS:
        p, n = make_page(fname, var, page)
        pages.append(p)
        total += n
    project['userPages'] = pages
    project['actions'] = [{'objID': new_id(), 'components': [], 'connectionLines': [], 'localVariables': [],
                           'componentGroups': [], 'userProperties': [], 'name': n,
                           'implementationType': 'native'} for n in actions]
    project['fonts'] = [make_font(n, *v) for n, v in HUD_FONTS.items()]

    os.makedirs(os.path.join(OUT_DIR, os.path.dirname(FONT_REL_PATH)), exist_ok=True)
    shutil.copyfile(FONT_FILE, os.path.join(OUT_DIR, FONT_REL_PATH))  # font source for EEZ Studio
    with open(OUT_PROJECT, 'w', encoding='utf-8', newline='\n') as f:
        json.dump(project, f, indent=2, ensure_ascii=False)
    with open(os.path.join(OUT_DIR, 'actions.md'), 'w', encoding='utf-8', newline='\n') as f:
        f.write('# EEZ native actions\n\nGenerated by tools/squareline_to_eez.py. EEZ Studio generates '
                '`void action_<name>(lv_event_t *e)` declarations, these map to the existing handlers:\n\n'
                '| action | original SquareLine event |\n|---|---|\n')
        for n, d in actions.items():
            f.write('| `action_%s` | %s |\n' % (n, d.replace('|', '\\|')))
    print('objects %d, pages %d, actions %d, fonts %d -> %s' % (total, len(pages), len(actions),
                                                                len(project['fonts']), OUT_PROJECT))
    for w in warnings:
        print('warning:', w)


if __name__ == '__main__':
    main()
