#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

int main()
{
    char hostname[4096];
    int rc;
    FILE *fp;
    char nethostname[4096];
    char *s;

    rc = gethostname(hostname, sizeof(hostname));
    assert(rc == 0);
    printf("hostname:    %s\n", hostname);
    assert(strcmp(hostname, "localhost") != 0);

    fp = popen("getprop net.hostname", "r");
    assert(fp != NULL);
    assert(fgets(nethostname, sizeof(nethostname), fp) != NULL);
    /* Get rid of control characters since getprop put \r to the end of string */
    for (s = nethostname; *s != '\0'; ++s) {
        if (iscntrl(*s))
            *s = '\0';
    }
    fclose(fp);

    printf("nethostname: %s\n", nethostname);
    assert(strcmp(hostname, nethostname) == 0);

    printf("OK\n");
    return 0;
}
