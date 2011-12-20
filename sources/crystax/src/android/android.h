#ifndef _WCHAR_SUPPORT_ANDROID_H_99544c48e9174f659a97671e7f64c763
#define _WCHAR_SUPPORT_ANDROID_H_99544c48e9174f659a97671e7f64c763

#include <wchar.h>
#include <stdio.h>
#include <locale.h>

#define CRYSTAX_DEBUG 0

#if CRYSTAX_DEBUG
#include <stdio.h>
#define DBG(fmt, ...) printf("CRYSTAX DBG: " fmt "\n", ##__VA_ARGS__)
#else
#define DBG(...)
#endif

#define __dead2
#define EFTYPE EFAULT
#define EX_OSERR -1

#define reallocf realloc

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) MIN(b, a)
#endif

#define __fflush fflush

#define FLOCKFILE(fp) do {} while(0)
#define FUNLOCKFILE(fp) do {} while(0)

typedef struct {
    char *data;
    size_t size;
} __locale_data_t;

typedef struct
{
    char *encoding;
    struct
    {
        int alias;
        char *aliasto;
        __locale_data_t data;
    } data[_LC_LAST];
} __whole_locale_data_t;

#ifdef __i386__
typedef enum {
  FP_PS=0,  /* 24 bit (single-precision) */
  FP_PRS,   /* reserved */
  FP_PD,    /* 53 bit (double-precision) */
  FP_PE   /* 64 bit (extended-precision) */
} fp_prec_t;

extern fp_prec_t fpsetprec(fp_prec_t p);
#endif

#ifdef __cplusplus
extern "C" {
#endif

const char *_getprogname();
__whole_locale_data_t *__lookup_whole_locale_data(const char *encoding);
__locale_data_t * android_get_part_locale_data(const char *encoding, const char *data);
__locale_data_t * android_get_locale_data(int type, const char *encoding);

mbstate_t *mbstate_for(FILE *fp);

#ifdef __cplusplus
}
#endif

#endif /* _WCHAR_SUPPORT_ANDROID_H_99544c48e9174f659a97671e7f64c763 */
