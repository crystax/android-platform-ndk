#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
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

typedef int (*Py_MainPtr)(int, char **);

int main(int argc, char** argv)
{
  char executable[MY_POSIX_MAX_PATH + 1] = {0};
  char pthfmt[MY_POSIX_MAX_PATH + 1]     = {0};
  char corepath[MY_POSIX_MAX_PATH + 1]   = {0};
  void* core = NULL;
  int retcode = 126;

  Py_MainPtr Py_Main = NULL;

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

  retcode = Py_Main(argc, argv);

exit:
  if (core != NULL)
    dlclose(core);

  return retcode;
}
