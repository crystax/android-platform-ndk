// When built with GCC 4.7 for armeabi and run on armeabi emulator under
// Windows or Mac OS X this test hangs.
// The reason -- 'include <iostream>'.
// That is why this test is.

#include <iostream>


int main()
{
	return 0;
}
