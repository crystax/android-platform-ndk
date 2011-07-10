#ifndef _WCHAR_SUPPORT_ANDROID_H_99544c48e9174f659a97671e7f64c763
#define _WCHAR_SUPPORT_ANDROID_H_99544c48e9174f659a97671e7f64c763

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

typedef struct {
    char *data;
    size_t size;
} __locale_data_t;

#ifdef __cplusplus
extern "C" {
#endif

const char *_getprogname();
__locale_data_t * android_get_part_locale_data(const char *encoding, const char *data);
__locale_data_t * android_get_locale_data(int type, const char *encoding);

#ifdef __cplusplus
}
#endif

#endif /* _WCHAR_SUPPORT_ANDROID_H_99544c48e9174f659a97671e7f64c763 */
