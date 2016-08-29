#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>

#if defined(__linux__)
#  define MY_POSIX_MAX_PATH PATH_MAX
#elif defined(__APPLE__)
#  include <libproc.h>
#  define MY_POSIX_MAX_PATH PROC_PIDPATHINFO_MAXSIZE
#endif

#ifndef PYTHON_DSO_REL_PATH
#error PYTHON_DSO_REL_PATH - must be defined in Makefile
#endif

static char NULL_PTR_STR[] = "NULL";

#if defined(__linux__)
static void GetExecutablePath(char* path)
{
  int size = readlink("/proc/self/exe", path, MY_POSIX_MAX_PATH);
  if (size < 0)
    size = 0;
  path[size] = 0;
}
#elif defined(__APPLE__)
static void GetExecutablePath(char* path)
{
  int size = proc_pidpath(getpid(), path, MY_POSIX_MAX_PATH);
  if (size < 0)
    size = 0;
  path[size] = 0;
}
#endif

static void GetRelativePathFormat(char* base, char* fmt)
{
  unsigned idx;
  char *p, *end;
  end = strrchr(base, '/');
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
typedef void* (*PyMem_RawMallocPtr)(size_t);
typedef void (*PyMem_RawFreePtr)(void*);
typedef wchar_t* (*Py_DecodeLocalePtr)(const char*, size_t*);


int main(int argc, char** argv)
{
  char executable[MY_POSIX_MAX_PATH + 1] = {0};
  char pthfmt[MY_POSIX_MAX_PATH + 1]     = {0};
  char corepath[MY_POSIX_MAX_PATH + 1]   = {0};
  void* core = NULL;
  int retcode = 126;
  int i;

  Py_MainPtr Py_Main = NULL;
  PyMem_RawMallocPtr PyMem_RawMalloc = NULL;
  PyMem_RawFreePtr PyMem_RawFree = NULL;
  Py_DecodeLocalePtr Py_DecodeLocale = NULL;

  GetExecutablePath(executable);
  GetRelativePathFormat(executable, pthfmt);

  snprintf(corepath, MY_POSIX_MAX_PATH, pthfmt, PYTHON_DSO_REL_PATH);

  core = dlopen(corepath, RTLD_LAZY);
  if (core == NULL)
  {
    const char* lasterr = dlerror();
    if (lasterr == NULL)
      lasterr = NULL_PTR_STR;
    fprintf(stderr, "Fatal Python error: cannot load library: '%s', dlerror: %s\n", corepath, lasterr);
    goto exit;
  }

  Py_Main = (Py_MainPtr)dlsym(core, "Py_Main");
  if (Py_Main == NULL)
  {
    const char* lasterr = dlerror();
    if (lasterr == NULL)
      lasterr = NULL_PTR_STR;
    fprintf(stderr, "Fatal Python error: cannot load symbol: '%s' from library '%s', dlerror: %s\n", "Py_Main", corepath, lasterr);
    goto exit;
  }

  PyMem_RawMalloc = (PyMem_RawMallocPtr)dlsym(core, "PyMem_RawMalloc");
  if (PyMem_RawMalloc == NULL)
  {
    const char* lasterr = dlerror();
    if (lasterr == NULL)
      lasterr = NULL_PTR_STR;
    fprintf(stderr, "Fatal Python error: cannot load symbol: '%s' from library '%s', dlerror: %s\n", "PyMem_RawMalloc", corepath, lasterr);
    goto exit;
  }

  PyMem_RawFree = (PyMem_RawFreePtr)dlsym(core, "PyMem_RawFree");
  if (PyMem_RawFree == NULL)
  {
    const char* lasterr = dlerror();
    if (lasterr == NULL)
      lasterr = NULL_PTR_STR;
    fprintf(stderr, "Fatal Python error: cannot load symbol: '%s' from library '%s', dlerror: %s\n", "PyMem_RawFree", corepath, lasterr);
    goto exit;
  }

  Py_DecodeLocale = (Py_DecodeLocalePtr)dlsym(core, "Py_DecodeLocale");
  if (Py_DecodeLocale == NULL)
  {
    const char* lasterr = dlerror();
    if (lasterr == NULL)
      lasterr = NULL_PTR_STR;
    fprintf(stderr, "Fatal Python error: cannot load symbol: '%s' from library '%s', dlerror: %s\n", "Py_DecodeLocale", corepath, lasterr);
    goto exit;
  }

  wchar_t** argv_copy = (wchar_t **)PyMem_RawMalloc(sizeof(wchar_t*)*(argc+1));
  wchar_t** argv_copy2 = (wchar_t **)PyMem_RawMalloc(sizeof(wchar_t*)*(argc+1));

  char* oldloc = strdup(setlocale(LC_ALL, NULL));
  setlocale(LC_ALL, "");
  for (i = 0; i < argc; ++i)
  {
    argv_copy[i] = Py_DecodeLocale(argv[i], NULL);
    if (argv_copy[i] == NULL)
    {
      free(oldloc);
      fprintf(stderr, "Fatal Python error: unable to decode the command line argument #%i\n", i + 1);
      goto exit;
    }
    argv_copy2[i] = argv_copy[i];
  }
  argv_copy2[argc] = argv_copy[argc] = NULL;
  setlocale(LC_ALL, oldloc);
  free(oldloc);

  retcode = Py_Main(argc, argv_copy);

  for (i = 0; i < argc; i++)
  {
    PyMem_RawFree(argv_copy2[i]);
  }
  PyMem_RawFree(argv_copy);
  PyMem_RawFree(argv_copy2);

exit:
  if (core != NULL)
    dlclose(core);

  return retcode;
}
