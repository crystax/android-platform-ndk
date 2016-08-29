#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#ifndef PYTHON3_DSO_REL_PATH
#error PYTHON3_DSO_REL_PATH - must be defined in Makefile
#endif

static void GetExecutablePath(wchar_t* path)
{
  unsigned size = GetModuleFileNameW(0, path, MAX_PATH);
  path[size] = 0;
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

  Py_MainPtr Py_Main = 0;

  GetExecutablePath(executable);
  GetRelativePathFormat(executable, pthfmt);

  _snwprintf(corepath, MAX_PATH, pthfmt, PYTHON3_DSO_REL_PATH);

  core = LoadLibraryExW(corepath, 0, 0);
  if (core == 0)
  {
    DWORD code = GetLastError();
    _fwprintf_p(stderr, L"Fatal Python error: cannot load library: '%s', LoadLibraryExW error code: %d\n", corepath, code);
    goto exit;
  }

  Py_Main = (Py_MainPtr)GetProcAddress(core, "Py_Main");
  if (Py_Main == 0)
  {
    DWORD code = GetLastError();
    _fwprintf_p(stderr, L"Fatal Python error: cannot load symbol: '%s' from library '%s', GetProcAddress error code: %d\n", L"Py_Main", corepath, code);
    goto exit;
  }

  retcode = Py_Main(argc, argv);

exit:
  if (core)
    FreeLibrary(core);

  return retcode;
}
