#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#ifndef PYTHON_DSO_REL_PATH
#error PYTHON_DSO_REL_PATH - must be defined in Makefile
#endif

static int GetExecutablePath(wchar_t* path)
{
  DWORD code;
  DWORD attr;
  DWORD size;
  wchar_t reparsed_path[MAX_PATH + 1];
  HANDLE fh_exe;
  DWORD (WINAPI* GetFinalPathNameByHandleW_Ptr)(HANDLE, LPWSTR, DWORD, DWORD);

  size = GetModuleFileNameW(NULL, path, MAX_PATH);
  if (size == 0)
  {
    code = GetLastError();
    _fwprintf_p(stderr, L"Fatal Python error: cannot eval path of executable, GetModuleFileNameW error code: %d\n", code);
    return 0;
  }
  if (size > MAX_PATH)
    size = MAX_PATH;
  path[size] = 0;
  GetFinalPathNameByHandleW_Ptr = (DWORD (WINAPI *)(HANDLE, LPWSTR, DWORD, DWORD))GetProcAddress(GetModuleHandle("kernel32"), "GetFinalPathNameByHandleW");
  if (GetFinalPathNameByHandleW_Ptr == NULL)
    return 1;

  attr = GetFileAttributesW(path);
  if (attr == INVALID_FILE_ATTRIBUTES)
  {
    code = GetLastError();
    _fwprintf_p(stderr, L"Fatal Python error: cannot eval path of executable, GetFileAttributesW error code: %d\n", code);
    return 0;
  }
  if (attr & FILE_ATTRIBUTE_REPARSE_POINT)
  {
    fh_exe = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fh_exe == INVALID_HANDLE_VALUE)
    {
      code = GetLastError();
      _fwprintf_p(stderr, L"Fatal Python error: cannot eval path of executable, CreateFileW error code: %d\n", code);
      return 0;
    }
    size = (*GetFinalPathNameByHandleW_Ptr)(fh_exe, reparsed_path, MAX_PATH, 0);
    if (size == 0)
    {
      code = GetLastError();
      CloseHandle(fh_exe);
      _fwprintf_p(stderr, L"Fatal Python error: cannot eval path of executable, GetFinalPathNameByHandleW error code: %d\n", code);
      return 0;
    }
    CloseHandle(fh_exe);
    if (size > MAX_PATH)
      size = MAX_PATH;
    reparsed_path[size] = 0;

    if (wcsncmp(L"\\\\?\\", reparsed_path, 4) == 0)
      wcsncpy(path, &reparsed_path[4], MAX_PATH - 3);
    else
      wcsncpy(path, reparsed_path, MAX_PATH + 1);
  }
  return 1;
}

static void GetRelativePathFormat(wchar_t* base, wchar_t* fmt)
{
  unsigned idx;
  wchar_t *p, *end;
  end = wcsrchr(base, '\\');
  for (idx = 0, p = base; *p; ++p, ++idx)
  {
    fmt[idx] = *p;
    if (p == end)
      break;
  }
  fmt[++idx] = '%';
  fmt[++idx] = 's';
  fmt[++idx] = 0;
}

typedef int (*Py_MainPtr)(int, wchar_t**);


int wmain(int argc, wchar_t** argv)
{
  wchar_t executable[MAX_PATH + 1] = {0};
  wchar_t pthfmt[MAX_PATH + 1]     = {0};
  wchar_t corepath[MAX_PATH + 1]   = {0};
  HMODULE core = NULL;
  int retcode = 126;

  Py_MainPtr Py_Main = NULL;

  if (!GetExecutablePath(executable))
    goto exit;

  GetRelativePathFormat(executable, pthfmt);

  _snwprintf(corepath, MAX_PATH, pthfmt, PYTHON_DSO_REL_PATH);

  core = LoadLibraryExW(corepath, NULL, 0);
  if (core == NULL)
  {
    DWORD code = GetLastError();
    _fwprintf_p(stderr, L"Fatal Python error: cannot load library: '%s', LoadLibraryExW error code: %d\n", corepath, code);
    goto exit;
  }

  Py_Main = (Py_MainPtr)GetProcAddress(core, "Py_Main");
  if (Py_Main == NULL)
  {
    DWORD code = GetLastError();
    _fwprintf_p(stderr, L"Fatal Python error: cannot load symbol: '%s' from library '%s', GetProcAddress error code: %d\n", L"Py_Main", corepath, code);
    goto exit;
  }

  retcode = Py_Main(argc, argv);

exit:
  if (core != NULL)
    FreeLibrary(core);

  return retcode;
}
