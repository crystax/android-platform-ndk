#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int rc;
    void *ptr = NULL;

    rc = posix_memalign(&ptr, 0, 0);
    assert(rc == EINVAL);

    rc = posix_memalign(&ptr, 1, 0);
    assert(rc == EINVAL);

    rc = posix_memalign(&ptr, 1, 4);
    assert(rc == EINVAL);

    rc = posix_memalign(&ptr, 5*sizeof(void*), 4);
    assert(rc == EINVAL);

    rc = posix_memalign(&ptr, sizeof(void*), 4);
    assert(rc == 0);
    assert(ptr != NULL);
    free(ptr);

    rc = posix_memalign(&ptr, 2*sizeof(void*), 4);
    assert(rc == 0);
    assert(ptr != NULL);
    free(ptr);

    rc = posix_memalign(&ptr, 1024*sizeof(void*), 4);
    assert(rc == 0);
    assert(ptr != NULL);
    free(ptr);

    return 0;
}
