#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#ifndef PYTHON_DSO_REL_PATH
#error PYTHON_DSO_REL_PATH - must be defined in Makefile
#endif

static void GetExecutablePath(wchar_t* path)
{
  unsigned size = GetModuleFileNameW(NULL, path, MAX_PATH);
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

  Py_MainPtr Py_Main = NULL;

  GetExecutablePath(executable);
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
