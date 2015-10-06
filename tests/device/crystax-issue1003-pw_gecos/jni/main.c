#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

static int ccounter = 0;

static const char *SHELL = NULL;

void check(struct passwd *p, const char *u)
{
    ++ccounter;

    printf("=== Check %p...\n", p);

    assert(p != NULL);

    assert(p->pw_uid == getuid());
    printf("%d: pw_uid: %d\n", ccounter, (int)p->pw_uid);

    assert(p->pw_gid == getgid());
    printf("%d: pw_gid: %d\n", ccounter, (int)p->pw_gid);

    assert(p->pw_name != NULL);
    printf("%d: pw_name: '%s'\n", ccounter, p->pw_name);
    if (u) {
        assert(strcmp(p->pw_name, u) == 0);
    }
    else {
        assert(strcmp(p->pw_name, "") != 0);
    }

    if (p->pw_passwd)
        printf("%d: pw_passwd: HIDDEN (%zu bytes long)\n", ccounter, strlen(p->pw_passwd));

    assert(p->pw_gecos != NULL);
    printf("%d: pw_gecos: '%s'\n", ccounter, p->pw_gecos);

    assert(p->pw_dir != NULL);
    printf("%d: pw_dir: '%s'\n", ccounter, p->pw_dir);

    assert(p->pw_shell != NULL);
    printf("%d: pw_shell: '%s'\n", ccounter, p->pw_shell);
    assert(strcmp(p->pw_shell, SHELL) == 0);
}

int main()
{
    struct passwd *p;
    struct passwd pwd;
    char *buf;
    size_t buflen;
    int rc;
    char *username = NULL;

#if __ANDROID__
    SHELL = "/system/bin/sh";
#else
    SHELL = getenv("SHELL");
#endif

    p = getpwuid(-1);
    assert(p == NULL);

    p = getpwuid(getuid());
    check(p, NULL);

    username = strdup(p->pw_name);

    buflen = sysconf(_SC_GETPW_R_SIZE_MAX);
    buf = malloc(buflen);

    rc = getpwuid_r(-1, &pwd, buf, buflen, &p);
    assert(rc == 0);
    assert(p == NULL);

    rc = getpwuid_r(getuid(), &pwd, buf, buflen, &p);
    assert(rc == 0);
    check(p, username);

    p = getpwnam("b275601f14244dc6bc44eec6507f4f96");
    assert(p == NULL);

    p = getpwnam(username);
    check(p, username);

    rc = getpwnam_r("b275601f14244dc6bc44eec6507f4f96", &pwd, buf, buflen, &p);
    assert(rc == 0);
    assert(p == NULL);

    rc = getpwnam_r(username, &pwd, buf, buflen, &p);
    assert(rc == 0);
    check(p, username);

#if __ANDROID__
    p = getpwent();
    check(p, username);

    p = getpwent();
    assert(p == NULL);

    setpwent();
    p = getpwent();
    check(p, username);

    p = getpwent();
    assert(p == NULL);
#endif

    printf("OK\n");
    return 0;
}
