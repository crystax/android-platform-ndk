/* This tests here to ensure that popen() works
 * Earlier, there was crashes on call of popen().
 * See https://tracker.crystax.net/issues/958 for details.
 */

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdio.h>
#include <assert.h>

int main()
{
    FILE *fp;
    char buf[4096];

    fp = popen("ls 2>&1", "r");
    assert(fp);

    while (!feof(fp)) {
        size_t n = fread(buf, 1, sizeof(buf), fp);
        fwrite(buf, 1, n, stdout);
    }

    pclose(fp);

    return 0;
}
