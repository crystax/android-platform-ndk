#include <Python.h>
#include <osdefs.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef __APPLE__
#include <libproc.h>
#endif

static wchar_t EMPTY_STR[1] = {0};
static wchar_t* module_search_path = NULL;
static wchar_t* prog_path = NULL;
static int calculated = 0;

static wchar_t* prefix = EMPTY_STR;
static wchar_t* exec_prefix = EMPTY_STR;

static int py_wstat(const wchar_t* path, struct stat *buf)
{
    int err;
    char *fname;
    fname = Py_EncodeLocale(path, NULL);
    if (fname == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    err = stat(fname, buf);
    PyMem_Free(fname);
    return err;
}

static int is_file(wchar_t* filename)
{
    struct stat buf;
    if (py_wstat(filename, &buf) != 0)
        return 0;
    if (!S_ISREG(buf.st_mode))
        return 0;
    return 1;
}

static int is_dir(wchar_t* filename)
{
    struct stat buf;
    if (py_wstat(filename, &buf) != 0)
        return 0;
    if (!S_ISDIR(buf.st_mode))
        return 0;
    return 1;
}

static int is_full_path(const wchar_t* path)
{
#ifdef _WIN32
    wchar_t letter = 0;
    wchar_t anchor = 0;
    if (path && path[0] && path[1])
    {
        letter = path[0];
        letter &= ~0x20;
        anchor = path[1];
    }
    if (anchor == ':' && 'A' <= letter && letter <= 'Z')
    {
      return 1;
    }
    return 0;
#else
    if (path && path[0] == '/')
    {
        return 1;
    }
    return 0;
#endif
}

static wchar_t* eval_full_path_of_executable()
{
    wchar_t* exe_path_w = NULL;
#ifdef _WIN32
    wchar_t buffer[MAXPATHLEN + 1];
    wchar_t reparsed_path[MAXPATHLEN + 1];
    DWORD attr;
    DWORD size;
    DWORD (WINAPI* GetFinalPathNameByHandleW_Ptr)(HANDLE, LPWSTR, DWORD, DWORD);
    HANDLE fh_exe;
    size = GetModuleFileNameW(NULL, buffer, MAXPATHLEN);
    if (size == 0)
        Py_FatalError("Cannot eval path of executable: got 0 from GetModuleFileNameW");
    if (size  > MAXPATHLEN)
        size = MAXPATHLEN;
    buffer[size] = 0;
    GetFinalPathNameByHandleW_Ptr = (DWORD (WINAPI *)(HANDLE, LPWSTR, DWORD, DWORD))GetProcAddress(GetModuleHandle("kernel32"), "GetFinalPathNameByHandleW");
    if (GetFinalPathNameByHandleW_Ptr)
    {
        attr = GetFileAttributesW(buffer);
        if (attr == INVALID_FILE_ATTRIBUTES)
            Py_FatalError("Cannot eval path of executable: got INVALID_FILE_ATTRIBUTES from GetFileAttributesW");
        if (attr & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            fh_exe = CreateFileW(buffer, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (fh_exe == INVALID_HANDLE_VALUE)
                Py_FatalError("Cannot eval path of executable: got INVALID_HANDLE_VALUE from CreateFileW");
            size = (*GetFinalPathNameByHandleW_Ptr)(fh_exe, reparsed_path, MAX_PATH, 0);
            if (size == 0)
            {
                CloseHandle(fh_exe);
                Py_FatalError("Cannot eval path of executable: got 0 from GetFinalPathNameByHandleW");
            }
            CloseHandle(fh_exe);
            if (size > MAX_PATH)
              size = MAX_PATH;
            reparsed_path[size] = 0;
            if (wcsncmp(L"\\\\?\\", reparsed_path, 4) == 0)
                wcsncpy(buffer, &reparsed_path[4], MAX_PATH - 3);
            else
                wcsncpy(buffer, reparsed_path, MAX_PATH + 1);
        }
    }
    exe_path_w = PyMem_RawMalloc((wcslen(buffer) + 1) * sizeof(wchar_t));
    wcscpy(exe_path_w, buffer);
#else
    char exe_path[MAXPATHLEN + 1];
#ifdef __APPLE__
    int size = proc_pidpath(getpid(), exe_path, MAXPATHLEN);
#else
    int size = readlink("/proc/self/exe", exe_path, MAXPATHLEN);
#endif
    if (size < 0)
        size = 0;
    exe_path[size] = 0;
    exe_path_w = Py_DecodeLocale(exe_path, NULL);
#endif
    return exe_path_w;
}

static wchar_t* eval_dirname_of_this_dso()
{
    wchar_t* dso_path = NULL;
#ifdef _WIN32
    extern HANDLE PyWin_DLLhModule;
    if (PyWin_DLLhModule == NULL)
    {
        Py_FatalError("Cannot eval dirname of dso: HANDLE PyWin_DLLhModule is NULL");
    }
    wchar_t buffer[MAXPATHLEN + 1];
    GetModuleFileNameW(PyWin_DLLhModule, buffer, MAXPATHLEN);
    size_t i = wcslen(buffer);
    for (; i >= 0; --i)
    {
        if (buffer[i] == '\\')
        {
            buffer[i + 1] = 0;
            break;
        }
    }
    dso_path = PyMem_RawMalloc((wcslen(buffer) + 1) * sizeof(wchar_t));
    wcscpy(dso_path, buffer);
#else
    Dl_info dl;
    memset(&dl, 0, sizeof(dl));
    int ret = dladdr((void*)eval_dirname_of_this_dso, &dl);
    if (!ret || !dl.dli_fname || (dl.dli_fname[0] != '/'))
    {
        Py_FatalError("Cannot eval dirname of dso: bad result in dladdr()");
    }

    dso_path = Py_DecodeLocale(dl.dli_fname, NULL);
    size_t i = wcslen(dso_path);
    for (; i >= 0; --i)
    {
        if (dso_path[i] == '/')
        {
            dso_path[i + 1] = 0;
            break;
        }
    }
#endif
    return dso_path;
}

static void calculate_path(void)
{
    if (!calculated)
    {
        wchar_t* preset_progpath = Py_GetProgramName();
        if (is_full_path(preset_progpath))
        {
            prog_path = preset_progpath;
        }
        else
        {
            prog_path = eval_full_path_of_executable();
        }
    }

    wchar_t* dso_dir_path = eval_dirname_of_this_dso();

    wchar_t stdlib_zip_fname[32];
    swprintf(stdlib_zip_fname, 32, L"python%d%d.zip", PY_MAJOR_VERSION, PY_MINOR_VERSION);
    wchar_t stdlib_zip_path[MAXPATHLEN + 1];
    wcscpy(stdlib_zip_path, dso_dir_path);
    wcscat(stdlib_zip_path, stdlib_zip_fname);
    int have_stdlib_zip = is_file(stdlib_zip_path);

    int have_py_home = 0;
    wchar_t* py_home = NULL;
    if (!have_stdlib_zip)
    {
        py_home = Py_GetPythonHome();
        if (py_home && py_home[0])
            have_py_home = is_dir(py_home);
    }

    wchar_t modules_path[MAXPATHLEN + 1];
    wcscpy(modules_path, dso_dir_path);
    wcscat(modules_path, L"modules");
    int have_modules_dir = is_dir(modules_path);

    size_t search_path_len = 0;
    if (have_stdlib_zip)
    {
        search_path_len += 1 + wcslen(stdlib_zip_path);
    }
    if (have_py_home)
    {
        search_path_len += 1 + wcslen(py_home);
    }
    if (have_modules_dir)
    {
        search_path_len += 1 + wcslen(modules_path);
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
        wcscat(new_search_path, stdlib_zip_path);
    }
    if (have_py_home)
    {
        if (new_search_path[0])
            wcscat(new_search_path, delim);
        wcscat(new_search_path, py_home);
    }
    if (have_modules_dir)
    {
        if (new_search_path[0])
            wcscat(new_search_path, delim);
        wcscat(new_search_path, modules_path);
    }
    if (module_search_path && module_search_path[0])
    {
        if (new_search_path[0])
            wcscat(new_search_path, delim);
        wcscat(new_search_path, module_search_path);
    }

    PyMem_RawFree(dso_dir_path);

    if (module_search_path)
        PyMem_RawFree(module_search_path);

    module_search_path = new_search_path;
    calculated = 1;
}

/* External interface */

#ifdef _WIN32
int _Py_CheckPython3()
{
    return 0;
}
#endif

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

void Py_SetPath(const wchar_t* path)
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
