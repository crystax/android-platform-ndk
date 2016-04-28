from __future__ import print_function
import sys
if sys.version_info[0] < 3:
    import ConfigParser as configparser
else:
    import configparser
import argparse
import io
import marshal
import os.path


FROZEN_H = 'python-stdlib.h'
FROZEN_C = 'python-stdlib.c'
FROZEN_C_TABLE = '_PyImport_FrozenStdlibModules'
FROZEN_MK = 'python-stdlib.list'


PY2_ENCODING_ISO_8859_1 = (
    'encodings/punycode.py',
    'encodings/string_escape.py',
    'sqlite3/__init__.py',
    'sqlite3/dbapi2.py',
    'inspect.py',
    'shlex.py',
    'tarfile.py',
)

PY2_ENCODING_UTF8 = (
    'ast.py',
    'sre_compile.py',
)

PY2_ENCODING_LATIN_1 = (
    'heapq.py',
    'pydoc.py',
)


class StdlibConfig:
    def __init__(self, ignore_dir_list, ignore_file_list):
        self.ignore_dir_list = ignore_dir_list
        self.ignore_file_list = ignore_file_list


def dir_in_interest(config, arch_path):
    for exclusion in config.ignore_dir_list:
        if arch_path == exclusion:
            return False
    return True


def file_in_interest(config, arch_path):
    if arch_path in config.ignore_file_list:
        return False
    return True


def in_interest(config, arch_path, is_dir, pathbits):
    name = pathbits[-1]
    if is_dir:
        if (name == '__pycache__' or name == 'test' or name == 'tests'):
            return False
        if arch_path.startswith('plat-'):
            return False
    else:
        if not arch_path.endswith('.py'):
            return False
    if is_dir:
        return dir_in_interest(config, arch_path)
    else:
        return file_in_interest(config, arch_path)


def enum_content(config, seed, catalog, pathbits = None):
    if pathbits is None:
        fs_path = seed
        is_dir = True
    else:
        fs_path = os.path.join(seed, *pathbits)
        is_dir = os.path.isdir(fs_path)
    if pathbits is not None:
        arc_path = '/'.join(pathbits)
        if not in_interest(config, arc_path, is_dir, pathbits):
            return
        if not is_dir:
            catalog.append((fs_path, arc_path))
    else:
        pathbits = []
    if is_dir:
        files = []
        dirs = []
        for name in os.listdir(fs_path):
            p = os.path.join(fs_path, name)
            if os.path.isdir(p):
                dirs.append(name)
            else:
                files.append(name)
        for name in sorted(dirs):
            pathbits.append(name)
            enum_content(config, seed, catalog, pathbits)
            del pathbits[-1]
        for name in sorted(files):
            pathbits.append(name)
            enum_content(config, seed, catalog, pathbits)
            del pathbits[-1]


def load_build_config():
    conf_path = os.path.join(os.path.normpath(os.path.abspath(os.path.dirname(__file__))), 'stdlib.config')
    config = configparser.RawConfigParser()
    config.read(conf_path)
    return config


def get_conf_strings(config, section, option):
    return config.get(section, option).split()


class ModuleInfo:
    def __init__(self, py_name, c_name, c_sz_name, frozen_size, c_file):
        self.py_name = py_name
        self.c_name = c_name
        self.c_sz_name = c_sz_name
        self.frozen_size = frozen_size
        self.c_file = c_file


def strip_coding_comment(pytext):
    import re
    lines = pytext.splitlines(True)
    coding_re = re.compile(r"^\s*#.*coding[:=]\s*([-\w.]+)", re.UNICODE)
    for i in range(2):
        if i < len(lines):
            if coding_re.match(lines[i]):
                lines[i] = ""
    return ''.join(lines)


def assemble_source(co_fname, mangled, source_file, output, py2):
    encoding = 'utf8'
    if py2:
        if co_fname in PY2_ENCODING_ISO_8859_1:
            encoding = 'iso-8859-1'
        elif co_fname in PY2_ENCODING_LATIN_1:
         encoding = 'latin-1'
    with io.open(source_file, mode='rt', encoding=encoding) as source:
        pytext = source.read()
    if py2:
        if co_fname in PY2_ENCODING_ISO_8859_1 or co_fname in PY2_ENCODING_UTF8 or co_fname in PY2_ENCODING_LATIN_1:
            pytext = strip_coding_comment(pytext)
    ast = compile(pytext, co_fname, 'exec', 0, 1)
    code = marshal.dumps(ast)
    output.write('unsigned char {0}[] = {{'.format(mangled))
    for i in range(0, len(code), 16):
        output.write('\n    ')
        for c in code[i:i+16]:
            if sys.version_info[0] < 3:
                output.write('{: >3d}, '.format(ord(c)))
            else:
                output.write('{: >3d}, '.format(c))
    output.write('\n};\n')
    return len(code)


def to_string(v):
    if sys.version_info[0] < 3:
        if isinstance(v, unicode):
            return v
        return unicode(v)
    else:
        if isinstance(v, str):
            return v
        return str(v)


def freeze_stdlib():
    build_config = load_build_config()
    ignore_dir_list = get_conf_strings(build_config, 'CONFIG', 'IGNORE_DIR')
    ignore_file_list = get_conf_strings(build_config, 'CONFIG', 'IGNORE_FILE')
    config = StdlibConfig(ignore_dir_list=ignore_dir_list, ignore_file_list=ignore_file_list)

    parser = argparse.ArgumentParser()
    parser.add_argument('--stdlib-dir', required=True)
    parser.add_argument('--output-dir', required=True)
    parser.add_argument('--py2', action='store_true')
    args = parser.parse_args()

    dirhere = os.path.normpath(os.path.abspath(os.path.dirname(__file__)))

    catalog = []
    enum_content(config, args.stdlib_dir, catalog)
    catalog += [
        (os.path.join(dirhere, 'site.py'), 'site.py'),
        (os.path.join(dirhere, 'sysconfig.py'), 'sysconfig.py'),
        (os.path.join(dirhere, '_sysconfigdata.py'), '_sysconfigdata.py'),
    ]
    if args.py2:
        catalog += [(os.path.join(dirhere, '_sitebuiltins.py'), '_sitebuiltins.py')]

    print("::: Generating byte-code for python-stdlib in '{}' ...".format(args.output_dir))
    inittab = []
    bootstrap = []
    for entry in catalog:
        source_file, arcname = entry[0], entry[1]

        fbits = arcname.split('/')
        is_package = False
        if len(fbits) > 1 and fbits[-1] == '__init__.py':
            is_package = True
            mod_name = '.'.join(fbits[0: len(fbits) - 1])
        else:
            modbits = fbits[:]
            modbits[-1] = modbits[-1].partition('.')[0]
            mod_name = '.'.join(modbits)
        mangled_origin = mod_name.replace('.', '_')
        mangled = '_Py_M_{}'.format(mangled_origin)
        sz_mangled = '_Py_Msz_{}'.format(mangled_origin)
        c_output_file = 'M_{}.c'.format(mangled_origin)

        print('::: freeze ::: {} >>> {}'.format(arcname, c_output_file))

        with open(os.path.join(args.output_dir, c_output_file), mode='w') as output:
            frozen_size = assemble_source(arcname, mangled, source_file, output, args.py2)
        if is_package:
            frozen_size = -frozen_size

        m = ModuleInfo(py_name=mod_name, c_name=mangled, c_sz_name=sz_mangled, frozen_size=frozen_size, c_file=c_output_file)
        inittab.append(m)

        if not args.py2:
            if mod_name == "importlib._bootstrap":
                print('::: link ::: {} >>> {}'.format('_frozen_importlib', 'importlib._bootstrap'))
                bootstrap += [('_frozen_importlib', mangled, sz_mangled)]
            elif mod_name == "importlib._bootstrap_external":
                print('::: link ::: {} >>> {}'.format('_frozen_importlib_external', 'importlib._bootstrap_external'))
                bootstrap += [('_frozen_importlib_external', mangled, sz_mangled)]

    print('::: generate ::: {}'.format(FROZEN_H))
    with io.open(os.path.join(args.output_dir, FROZEN_H), mode='wt', encoding='ascii') as frozen_h:
        frozen_h.write(to_string('#pragma once\n\n'))
        for i in range(len(inittab)):
            m = inittab[i]
            frozen_h.write(to_string('extern unsigned char {}[];\n'.format(m.c_name)))
            frozen_h.write(to_string('#define {} ((int)({}))\n'.format(m.c_sz_name, m.frozen_size)))
            if (i + 1) < len(inittab):
                frozen_h.write(to_string('\n'))

    print('::: generate ::: {}'.format(FROZEN_C))
    with io.open(os.path.join(args.output_dir, FROZEN_C), mode='wt', encoding='ascii') as frozen_c:
        frozen_c.write(to_string('#include <Python.h>\n'))
        frozen_c.write(to_string('#include <{}>\n'.format(FROZEN_H)))
        frozen_c.write(to_string('\n'))
        frozen_c.write(to_string('struct _frozen {}[] = {{\n'.format(FROZEN_C_TABLE)))
        for py_name, c_name, c_sz_name in bootstrap:
            frozen_c.write(to_string('    {{"{}", {}, {}}},\n'.format(py_name, c_name, c_sz_name)))
        for m in inittab:
            frozen_c.write(to_string('    {{"{}", {}, {}}},\n'.format(m.py_name, m.c_name, m.c_sz_name)))
        frozen_c.write(to_string('    {0, 0, 0}\n'))
        frozen_c.write(to_string('};\n'))

    print('::: generate ::: {}'.format(FROZEN_MK))
    with io.open(os.path.join(args.output_dir, FROZEN_MK), mode='wt', encoding='ascii') as frozen_mk:
        for m in inittab:
            frozen_mk.write(to_string('{}\n'.format(m.c_file)))


if __name__ == '__main__':
    freeze_stdlib()
