#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define TEST_FILE_NAME  "./test-getline.data"
#define BUF_SIZE        128


const char *DATA_1[] = {
    "Part0_0 Part0_1\n",
    "Part1_0 Part1_1\n",
    "Part2_0 Part2_1\n"
};
const int LINE_NUM_1 = sizeof(DATA_1) / sizeof(char *);

const char *DATA_2[] = {
    "Part0 ",
    "Part1 ",
    "Part2 ",
    "Part3 ",
    "Part4 "
};
const int LINE_NUM_2 = sizeof(DATA_2) / sizeof(char *);


static int make_test_file();


int test_getline()
{
    char buf[BUF_SIZE];
    FILE *f;
    int rc;
    int i;

    if (make_test_file(DATA_1, LINE_NUM_1) != 0) {
        printf("test_getline: failed to make test file\n");
        return 1;
    }

    f = fopen(TEST_FILE_NAME, "r");
    if (f == NULL) {
        rc = errno;
        printf("test_getline: failed to open temp file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    for (i=0; i<LINE_NUM_1; ++i) {
        char *s = NULL;
        int num = 0;
        if (getline(&s, &num, f) == -1) {
            rc = errno;
            printf("test_getline: failed to read temp file; error %i; %s\n", rc, strerror(rc));
            return 1;
        }
        if (strcmp(s, DATA_1[i]) != 0) {
            printf("test_getline: expected line: '%s'\n", DATA_1[i]);
            printf("test_getline: read line:     '%s'\n", s);
            return 1;
        }
    }

    if (fclose(f) != 0) {
        rc = errno;
        printf("failed to close test file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    if (unlink(TEST_FILE_NAME) != 0) {
        rc = errno;
        printf("failed to close test file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    printf("getline - ok\n");
    return 0;
}


int test_getdelim()
{
    char buf[BUF_SIZE];
    FILE *f;
    int rc;
    int i;

    if (make_test_file(DATA_2, LINE_NUM_2) != 0) {
        printf("test_getdelim: failed to make test file\n");
        return 1;
    }

    f = fopen(TEST_FILE_NAME, "r");
    if (f == NULL) {
        rc = errno;
        printf("test_getdelim: failed to open temp file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    for (i=0; i<LINE_NUM_2; ++i) {
        char *s = NULL;
        int num = 0;
        if (getdelim(&s, &num, ' ', f) == -1) {
            rc = errno;
            printf("test_getdelim: failed to read temp file; error %i; %s\n", rc, strerror(rc));
            return 1;
        }
        if (strcmp(s, DATA_2[i]) != 0) {
            printf("test_getdelim: expected: '%s'\n", DATA_2[i]);
            printf("test_getdelim: read:     '%s'\n", s);
            return 1;
        }
    }

    if (fclose(f) != 0) {
        rc = errno;
        printf("failed to close test file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    if (unlink(TEST_FILE_NAME) != 0) {
        rc = errno;
        printf("failed to close test file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    printf("getdelim - ok\n");
    return 0;
}


int make_test_file(const char *data[], int num)
{
    FILE *f;
    int rc, i;

    f = fopen(TEST_FILE_NAME, "w");
    if (f == NULL) {
        rc = errno;
        printf("failed to open test file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    for (i=0; i<num; ++i) {
        if (fprintf(f, "%s", data[i]) < 0) {
            rc = errno;
            printf("failed to write line %i to test file; error %i; %s\n", i, rc, strerror(rc));
            return 1;
        }
    }

    if (fclose(f) != 0) {
        rc = errno;
        printf("failed to close test file; error %i; %s\n", rc, strerror(rc));
        return 1;
    }

    return 0;
}
