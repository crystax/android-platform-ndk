#include <Python.h>
#include <osdefs.h>

static wchar_t EMPTY_STR[1] = {0};
static wchar_t* module_search_path = NULL;
static wchar_t* prog_path = NULL;
static int calculated = 0;

static wchar_t* prefix = EMPTY_STR;
static wchar_t* exec_prefix = EMPTY_STR;

#ifdef Py_ENABLE_SHARED

static int is_file(const char* filename)
{
    struct stat buf;
    if (stat(filename, &buf) != 0)
        return 0;
    if (!S_ISREG(buf.st_mode))
        return 0;
    return 1;
}

static int is_dir(const char* filename)
{
    struct stat buf;
    if (stat(filename, &buf) != 0)
        return 0;
    if (!S_ISDIR(buf.st_mode))
        return 0;
    return 1;
}

static void eval_full_path_of_executable(char* filename)
{
    int size = readlink("/proc/self/exe", filename, MAXPATHLEN);
    if (size < 0)
        size = 0;
    filename[size] = 0;
}

static void eval_dirname_of_this_dso(char* dso_path)
{
    /* For some android platforms dladdr() doesn't work properly, so just scan '/proc/self/maps' directly */

    void* self = &eval_dirname_of_this_dso;
    FILE *stream = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int ret = 1;
    int i;
    void* addr1;
    void* addr2;
    char perm[64];
    char offset[64];
    char device[64];
    char inode[64];
    char path[MAXPATHLEN + 1];

    stream = fopen("/proc/self/maps", "r");
    if (stream != NULL)
    {
        while ((read = getline(&line, &len, stream)) != -1)
        {
            if ((7 == sscanf(line, "%p-%p %s %s %s %s %s", &addr1, &addr2, perm, offset, device, inode, path)) && (path[0] == '/'))
            {
                if ((addr1 <= self) && (self < addr2))
                {
                    strncpy(dso_path, path, MAXPATHLEN);
                    ret = 0;
                    break;
                }
            }
        }
        free(line);
        fclose(stream);
    }
    if (ret)
    {
        Py_FatalError("Cannot eval dirname of dso from '/proc/self/maps'");
    }
    for (i = strlen(dso_path); i >= 0; --i)
    {
        if (dso_path[i] == '/')
        {
            dso_path[i + 1] = 0;
            break;
        }
    }
}

#endif /*Py_ENABLE_SHARED*/

static void calculate_path(void)
{
    char exe_path[MAXPATHLEN + 1];
    eval_full_path_of_executable(exe_path);

#ifdef Py_ENABLE_SHARED
    char dso_dir_path[MAXPATHLEN + 1];
    eval_dirname_of_this_dso(dso_dir_path);
#endif

    if (!calculated)
    {
        wchar_t* preset_progpath = Py_GetProgramName();
        if (preset_progpath && (preset_progpath[0] == '/'))
            prog_path = preset_progpath;
        else
            prog_path = Py_DecodeLocale(exe_path, NULL);
    }

    int i = strlen(exe_path);
    for (; i >= 0; --i)
    {
        if (exe_path[i] == '/')
        {
            exe_path[i + 1] = 0;
            break;
        }
    }

    wchar_t* stdlib_zip_path_w = NULL;
    int have_stdlib_zip = 0;

    wchar_t* modules_path_w = NULL;
    int have_modules_dir = 0;

#ifdef Py_ENABLE_SHARED
    char stdlib_zip_fname[32];
    snprintf(stdlib_zip_fname, 32, "python%d%d.zip", PY_MAJOR_VERSION, PY_MINOR_VERSION);

    char stdlib_zip_path[MAXPATHLEN + 1];
    strcpy(stdlib_zip_path, dso_dir_path);
    strcat(stdlib_zip_path, stdlib_zip_fname);
    have_stdlib_zip = is_file(stdlib_zip_path);
    if (!have_stdlib_zip)
    {
        char error_message[2*MAXPATHLEN];
        sprintf(error_message, "stdlib not found by path '%s'", stdlib_zip_path);
        Py_FatalError(error_message);
    }
    stdlib_zip_path_w = Py_DecodeLocale(stdlib_zip_path, NULL);

    char modules_path[MAXPATHLEN + 1];
    strcpy(modules_path, exe_path);
    strcat(modules_path, "modules");
    have_modules_dir = is_dir(modules_path);
    if (have_modules_dir)
    {
        modules_path_w = Py_DecodeLocale(modules_path, NULL);
    }
#endif

    size_t search_path_len = 0;
    if (have_stdlib_zip)
    {
        search_path_len += 1 + wcslen(stdlib_zip_path_w);
    }
    if (have_modules_dir)
    {
        search_path_len += 1 + wcslen(modules_path_w);
    }
    if (module_search_path && module_search_path[0])
    {
        search_path_len += 1 + wcslen(module_search_path);
    }

    wchar_t delim[2] = {DELIM, 0};
    wchar_t* new_search_path = (wchar_t*)PyMem_RawMalloc(search_path_len * sizeof(wchar_t));
    new_search_path[0] = 0;

    if (have_stdlib_zip)
    {
        if (new_search_path[0])
            wcscat(new_search_path, delim);
        wcscat(new_search_path, stdlib_zip_path_w);
    }
    if (have_modules_dir)
    {
        if (new_search_path[0])
            wcscat(new_search_path, delim);
        wcscat(new_search_path, modules_path_w);
    }
    if (module_search_path && module_search_path[0])
    {
        if (new_search_path[0])
            wcscat(new_search_path, delim);
        wcscat(new_search_path, module_search_path);
    }

    if (stdlib_zip_path_w)
        PyMem_RawFree(stdlib_zip_path_w);

    if (modules_path_w)
        PyMem_RawFree(modules_path_w);

    if (module_search_path)
        PyMem_RawFree(module_search_path);

    module_search_path = new_search_path;
    calculated = 1;
}

/* External interface */

wchar_t* Py_GetPath(void)
{
    if (!calculated)
        calculate_path();
    return module_search_path;
}

wchar_t* Py_GetPrefix(void)
{
    if (!calculated)
        calculate_path();
    return prefix;
}

wchar_t* Py_GetExecPrefix(void)
{
    if (!calculated)
        calculate_path();
    return exec_prefix;
}

wchar_t* Py_GetProgramFullPath(void)
{
    if (!calculated)
        calculate_path();
    return prog_path;
}

void Py_SetPath(const wchar_t *path)
{
    if (module_search_path != NULL)
    {
        PyMem_RawFree(module_search_path);
        module_search_path = NULL;
    }
    if (path != NULL)
    {
        module_search_path = (wchar_t*)PyMem_RawMalloc((wcslen(path) + 1) * sizeof(wchar_t));
        wcscpy(module_search_path, path);
    }
    calculate_path();
}
