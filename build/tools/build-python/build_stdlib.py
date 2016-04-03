import sys
if sys.version_info[0] < 3:
    import ConfigParser as configparser
else:
    import configparser
import argparse
import os.path
import zipfile


class StdlibConfig:
    def __init__(self, *, ignore_dir_list, ignore_file_list):
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


def build_stdlib():
    build_config = load_build_config()
    ignore_dir_list = get_conf_strings(build_config, 'CONFIG', 'IGNORE_DIR')
    ignore_file_list = get_conf_strings(build_config, 'CONFIG', 'IGNORE_FILE')
    config = StdlibConfig(ignore_dir_list=ignore_dir_list, ignore_file_list=ignore_file_list)

    parser = argparse.ArgumentParser()
    parser.add_argument('--pysrc-root', required=True)
    parser.add_argument('--output-zip', required=True)
    parser.add_argument('--py2', action='store_true')
    args = parser.parse_args()

    dirhere = os.path.normpath(os.path.abspath(os.path.dirname(__file__)))
    stdlib_srcdir = os.path.normpath(os.path.abspath(os.path.join(args.pysrc_root, 'Lib')))
    zipfilename = os.path.normpath(os.path.abspath(args.output_zip))
    display_zipname = os.path.basename(zipfilename)

    catalog = []
    enum_content(config, stdlib_srcdir, catalog)
    catalog += [
        (os.path.join(dirhere, 'site.py'), 'site.py'),
        (os.path.join(dirhere, 'sysconfig.py'), 'sysconfig.py'),
        (os.path.join(dirhere, '_sysconfigdata.py'), '_sysconfigdata.py'),
    ]
    if args.py2:
        catalog += [(os.path.join(dirhere, '_sitebuiltins.py'), '_sitebuiltins.py')]

    print("::: compiling python-stdlib zip package '{0}' ...".format(zipfilename))
    with zipfile.ZipFile(zipfilename, "w", zipfile.ZIP_DEFLATED) as fzip:
        for entry in catalog:
            fname, arcname = entry[0], entry[1]
            fzip.write(fname, arcname)
            print("::: {0} >>> {1}/{2}".format(fname, display_zipname, arcname))


if __name__ == '__main__':
    build_stdlib()
