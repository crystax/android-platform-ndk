#include <assert.h>
#include <time.h>
#include <stdio.h>

int main()
{
    time_t when;
    struct tm *local;

    when = time(NULL);
    local = gmtime(&when);
    assert(local != NULL);

    const char* tm_zone = local->tm_zone;
    assert(tm_zone != NULL);

    printf("date: %d.%d.%d, time: %d:%d:%d, zone: %s\n",
        local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
        local->tm_hour, local->tm_min, local->tm_sec,
        tm_zone);

    return 0;
}
