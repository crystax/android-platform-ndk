#ifdef NDEBUG
#undef NDEBUG
#endif
#include <sys/cdefs.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>

extern "C"
int test_wstring_erase()
{
  std::wstring data = L"abcdefghijkl";
  data.erase(data.begin(), data.begin() + 1);
  assert(data == L"bcdefghijkl");
	return 0;
}
