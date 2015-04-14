#include <sqlite3.h>
#include <stdio.h>

int main()
{
    printf("sqlite3 version: %s\n", sqlite3_libversion());
    printf("sqlite3 source id: %s\n", sqlite3_sourceid());
    return 0;
}
