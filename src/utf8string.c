#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utf8proc.h"
#include "utf8string.h"

#ifdef DEBUG
    #include "logger.h"
#endif

#define UTF8_LOGFILE "utf8string.log"

size_t utf8str_count(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    size_t len = 0, sz;
    utf8proc_uint8_t *tmp = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;
    while (*tmp) {
        sz = utf8proc_iterate(tmp, -1, &cp);

        if (cp == -1) {
            return (size_t)-1;
        }

        tmp += sz;
        len++;
    }

    return len;
}

size_t utf8str_char_length(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    unsigned int c = *str;

    if (c < 128) {
        return 1;
    }

    if ((c - 0xc2) > (0xf4 - 0xc2)) {
        return (size_t)-1;
    }

    if (c < 0xe0) {
        return 2;
    } else if (c < 0xf0) {
        return 3;
    } else {
        return 4;
    }
}

enum utf8_result utf8str_is_valid(const char *str, size_t len) {
    if (str == NULL) {
        return UTF8_INVALID_ARG;
    }

    size_t checked = 0, sz;
    utf8proc_uint8_t *tmp = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;

    while (*tmp) {
        sz = utf8proc_iterate(tmp, -1, &cp);

        if (cp == -1) {
            size_t bt = utf8str_char_length(tmp);
            size_t ln = len == 0 ? strlen(str) : len;

            if (bt + checked > ln) {
                return UTF8_UNFINISHED;
            }

            return UTF8_INVALID_UTF;
        }

        tmp += sz;
        checked += sz;

        if (len > 0) {
            if (checked > len) {
                return UTF8_UNFINISHED;
            } else if (checked == len) {
                return UTF8_OK;
            }
        }
    }

    return len == 0 ? UTF8_OK : UTF8_TOO_SHORT;
}

const char* utf8str_at_index(const char *str, ssize_t index) {
    if (str == NULL || index == 0) {
        return str;
    }

    if (index < 0) {
        size_t l = utf8str_count(str);
        if (l < -index) {
            return str;
        }

        index = l + index;
        if (index == 0) {
            return str;
        }
    }

    size_t len = 0, sz;
    utf8proc_uint8_t *tmp = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;
    while (*tmp) {
        sz = utf8proc_iterate(tmp, -1, &cp);

        if (cp == -1) {
            return NULL;
        }

        tmp += sz;
        len++;

        if (len == index) {
            return (char *)tmp;
        }
    }

    return (char *)tmp;
}

typedef utf8proc_int32_t (*utf8str_func) (utf8proc_int32_t);
static size_t cp_length(utf8proc_uint32_t cp) {
    if (cp < 0) {
        return 0;
    } else if (cp < 0x80) {
        return 1;
    } else if (cp < 0x800) {
        return 2;
    } else if (cp < 0x10000) {
        return 3;
    } else if (cp < 0x110000) {
        return 4;
    } else {
        return 0;
    }
}

static enum utf8_result process_utf8str(const char *src, char *dest, size_t *dest_sz, utf8str_func fn) {
    if (src == NULL) {
        return UTF8_INVALID_ARG;
    }

    size_t len = 0, used = 0, lendst;
    utf8proc_uint8_t *usrc = (utf8proc_uint8_t*)src;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)dest;
    utf8proc_int32_t cpsrc, cpdst;

    while (*usrc) {
        len = utf8proc_iterate(usrc, -1, &cpsrc);

        if (cpsrc == -1) {
            return UTF8_INVALID_UTF;
        }

        cpdst = (*fn)(cpsrc);
        usrc += len;

        lendst = cp_length(cpdst);

        if (dest != NULL && dest_sz != NULL) {
            if (lendst + used >= *dest_sz) {
                return UTF8_BUFFER_SMALL;
            }
        }

        if (dest != NULL) {
            lendst = utf8proc_encode_char(cpdst, udst);
            udst += lendst;
        }

        used += lendst;
    }

    if (dest != NULL) {
        *udst = '\0';
    }
    if (dest_sz) {
        *dest_sz = used;
    }

    return UTF8_OK;
}

static enum utf8_result process_utf8str_inplace(char *src, utf8str_func fn, size_t count) {
    if (src == NULL) {
        return UTF8_INVALID_ARG;
    }

    size_t len = 0, used = 0, lendst, processed = 0;
    utf8proc_uint8_t *usrc = (utf8proc_uint8_t*)src;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)src;
    utf8proc_int32_t cpsrc, cpdst;

    while (*usrc && (count == 0 || processed < count)) {
        len = utf8proc_iterate(usrc, -1, &cpsrc);

        if (cpsrc == -1) {
            return UTF8_INVALID_UTF;
        }

        cpdst = (*fn)(cpsrc);
        usrc += len;

        lendst = cp_length(cpdst);
        if (lendst > len) {
            return UTF8_BUFFER_SMALL;
        }

        lendst = utf8proc_encode_char(cpdst, udst);
        udst += lendst;
        ++processed;
    }

    return UTF8_OK;
}

enum utf8_result utf8str_upcase(const char *src, char *dest, size_t *dest_sz) {
    return process_utf8str(src, dest, dest_sz, utf8proc_toupper);
}

enum utf8_result utf8str_lowcase(const char *src, char *dest, size_t *dest_sz) {
    return process_utf8str(src, dest, dest_sz, utf8proc_tolower);
}

enum utf8_result utf8str_upcase_inplace(char *src, size_t count) {
    return process_utf8str_inplace(src, utf8proc_toupper, count);
}

enum utf8_result utf8str_lowcase_inplace(char *src, size_t count) {
    return process_utf8str_inplace(src, utf8proc_tolower, count);
}


enum utf8_result utf8str_equal_no_case(const char *orig, const char *cmp) {
    return utf8str_nequal_no_case(orig, cmp, 0);
}

enum utf8_result utf8str_nequal_no_case(const char *orig, const char *cmp, size_t maxlen) {
    if (orig == NULL && cmp == NULL) {
        return UTF8_EQUAL;
    }
    if (orig == NULL || cmp == NULL) {
        return UTF8_NEQUAL;
    }

    size_t cnt, compared = 0;
    utf8proc_uint8_t *uorig = (utf8proc_uint8_t*)orig;
    utf8proc_uint8_t *ucmp = (utf8proc_uint8_t*)cmp;
    utf8proc_int32_t cporig, cpcmp;

    while (*uorig && *ucmp) {
        cnt = utf8proc_iterate(uorig, -1, &cporig);
        if (cporig == -1) {
            return UTF8_INVALID_UTF;
        }
        uorig += cnt;
        cporig = utf8proc_toupper(cporig);

        cnt = utf8proc_iterate(ucmp, -1, &cpcmp);
        if (cpcmp == -1) {
            return UTF8_INVALID_UTF;
        }
        ucmp += cnt;
        cpcmp = utf8proc_toupper(cpcmp);

        if (cporig != cpcmp) {
            return UTF8_NEQUAL;
        }

        compared++;
        if (compared >= maxlen && maxlen != 0) {
            return UTF8_EQUAL;
        }
    }

    return (*uorig == '\0' && *ucmp == '\0') ? UTF8_EQUAL : UTF8_NEQUAL;
}


enum utf8_result utf8str_starts_with(const char *orig, const char *cmp) {
    if (orig == NULL && cmp == NULL) {
        return UTF8_EQUAL;
    }
    if (orig == NULL || cmp == NULL) {
        return UTF8_NEQUAL;
    }

    size_t cnt = 0;
    utf8proc_uint8_t *uorig = (utf8proc_uint8_t*)orig;
    utf8proc_uint8_t *ucmp = (utf8proc_uint8_t*)cmp;
    utf8proc_int32_t cporig, cpcmp;

    while (*uorig && *ucmp) {
        cnt = utf8proc_iterate(uorig, -1, &cporig);
        if (cporig == -1) {
            return UTF8_INVALID_UTF;
        }
        uorig += cnt;

        cnt = utf8proc_iterate(ucmp, -1, &cpcmp);
        if (cpcmp == -1) {
            return UTF8_INVALID_UTF;
        }
        ucmp += cnt;

        if (cporig != cpcmp) {
            return UTF8_NEQUAL;
        }
    }

    return (*ucmp == '\0') ? UTF8_EQUAL : UTF8_NEQUAL;
}

enum utf8_result utf8str_ends_with(const char *orig, const char *cmp) {
    if (orig == NULL && cmp == NULL) {
        return UTF8_EQUAL;
    }
    if (orig == NULL || cmp == NULL) {
        return UTF8_NEQUAL;
    }

    size_t cmplen = utf8str_count(cmp);
    if (cmplen == (size_t)-1) {
        return UTF8_INVALID_UTF;
    }
    const char *ends = utf8str_at_index(orig, -(ssize_t)cmplen);
    if (ends == NULL) {
        return UTF8_NEQUAL;
    }

    return utf8str_starts_with(ends, cmp);
}

static utf8proc_category_t utf8str_get_category(const char *str) {
    if (str == NULL || *str == '\0') {
        return -1;
    }

    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;
    size_t cnt = utf8proc_iterate(ustr, -1, &cp);
    if (cp == -1) {
        return -1;
    }

    return utf8proc_category(cp);
}

int utf8str_isdigit(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_ND ||
           ctg == UTF8PROC_CATEGORY_NL ||
           ctg == UTF8PROC_CATEGORY_NO;
}

int utf8str_isspace(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_ZS ||
           (*str >= 0x09 && *str <= 0x0D);
}

static int utf8str_isspace_cp(utf8proc_int32_t cp) {
    utf8proc_category_t ctg = utf8proc_category(cp);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_ZS ||
           (cp >= 0x09 && cp <= 0x0D);
}

int utf8str_ispunct(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_PC ||
           ctg == UTF8PROC_CATEGORY_PD ||
           ctg == UTF8PROC_CATEGORY_PS ||
           ctg == UTF8PROC_CATEGORY_PE ||
           ctg == UTF8PROC_CATEGORY_PF ||
           ctg == UTF8PROC_CATEGORY_PO ||
           ctg == UTF8PROC_CATEGORY_PI;
}

int utf8str_islower(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_LL;
}

int utf8str_isupper(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_LU ||
           ctg == UTF8PROC_CATEGORY_LT;
}

int utf8str_iscntrl(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_CC;
}

int utf8str_isalpha(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg == UTF8PROC_CATEGORY_LU ||
           ctg == UTF8PROC_CATEGORY_LL ||
           ctg == UTF8PROC_CATEGORY_LO ||
           ctg == UTF8PROC_CATEGORY_LT;
}

int utf8str_isprint(const char *str) {
    utf8proc_category_t ctg = utf8str_get_category(str);
    if (ctg == -1) {
        return 0;
    }

    return ctg != UTF8PROC_CATEGORY_CN &&
           ctg != UTF8PROC_CATEGORY_LM &&
           ctg != UTF8PROC_CATEGORY_MN &&
           ctg != UTF8PROC_CATEGORY_ME &&
           ctg != UTF8PROC_CATEGORY_MC &&
           ctg != UTF8PROC_CATEGORY_ZS &&
           ctg != UTF8PROC_CATEGORY_ZL &&
           ctg != UTF8PROC_CATEGORY_ZP &&
           ctg != UTF8PROC_CATEGORY_CC &&
           ctg != UTF8PROC_CATEGORY_CF &&
           ctg != UTF8PROC_CATEGORY_CO &&
           ctg != UTF8PROC_CATEGORY_CS &&
           ctg != UTF8PROC_CATEGORY_CN;
}

size_t utf8str_width(const char *str, size_t len) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    size_t width = 0;
    size_t cnt = 0, sz;
    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;
    while (*ustr) {
        sz = utf8proc_iterate(ustr, -1, &cp);

        if (cp == -1) {
            return (size_t)-1;
        }

        width += utf8proc_charwidth(cp);
        ustr += sz;
        cnt++;

        if (len > 0 && cnt == len) {
            return width;
        }
    }

    return width;
}

int utf8str_substr(const char *str, ssize_t start, ssize_t len, char *dest, size_t *dest_sz) {
    const char *begin = utf8str_at_index(str, start);
    if (begin == NULL) {
        return UTF8_INVALID_ARG;
    }

    size_t cnt = 0, sz, sz_used = 0;
    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)begin;
    utf8proc_uint8_t *udest = (utf8proc_uint8_t*)dest;
    utf8proc_int32_t cp;

    while (*ustr) {
        if (cnt >= len && len >= 0) {
            break;
        }

        sz = utf8proc_iterate(ustr, -1, &cp);
        if (cp == -1) {
            return UTF8_INVALID_UTF;
        }

        if (dest_sz != NULL && *dest_sz > 0 && *dest_sz <= sz_used + sz) {
            return UTF8_BUFFER_SMALL;
        }

        ustr += sz;
        sz_used += sz;
        ++cnt;
        if (udest != NULL) {
            utf8proc_encode_char(cp, udest);
            udest += sz;
        }
    }

    if (udest != NULL) {
        *udest = '\0';
    }
    if (dest_sz != NULL) {
        *dest_sz = sz_used;
    }

    return UTF8_OK;
}

const char* utf8str_char_next(const char *str) {
    if (str == NULL || *str == '\0') {
        return str;
    }

    const char *new_str = str;
    ++new_str;
    while (*new_str) {
        unsigned char c = *new_str;

        if (c < 0x80) {
            return new_str;
        }
        if (c > 0xC0) {
            return new_str;
        }

        ++new_str;
    }

    return new_str;
}

const char* utf8str_char_back(const char *str) {
    return utf8str_char_back_safe(str, NULL);
}

const char* utf8str_char_back_safe(const char *str, const char *stopper) {
    if (str == NULL || (stopper != NULL && str == stopper)) {
        return str;
    }

    const char *new_str = str;
    --new_str;
    if (stopper != NULL && stopper == new_str) {
        return new_str;
    }

    while ((((unsigned char)*new_str) & 0xC0) == 0x80) {
        --new_str;
        if (stopper != NULL && stopper == str) {
            return new_str;
        }
    }

    return new_str;
}

enum utf8_result utf8str_reverse(char *str) {
    if (str == NULL || *str == '\0') {
        return UTF8_OK;
    }

    size_t sz = strlen(str);
    char *copy = (char *)malloc(sizeof(char) * (sz + 1));
    if (copy == NULL) {
        return UTF8_OUT_OF_MEMORY;
    }

    char *stopper = str;
    size_t cnt;
    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)copy;
    utf8proc_int32_t cp;
    char *dup = str + sz;

    do {
        dup = (char *)utf8str_char_back_safe(dup, stopper);
        utf8proc_uint8_t *tmp = (utf8proc_uint8_t*)dup;
        int len = utf8proc_iterate(tmp, -1, &cp);
        if (cp == -1) {
            free(copy);
            return UTF8_INVALID_UTF;
        }
        utf8proc_encode_char(cp, ustr);
        ustr += len;
    } while (dup != stopper);
    copy[sz] = '\0';

    strcpy(str, copy);
    free(copy);
    return UTF8_OK;
}

enum utf8_result utf8str_titlecase(char *str) {
    if (str == NULL || *str == '\0') {
        return UTF8_OK;
    }

    int space = 1;
    utf8proc_uint8_t *usrc = (utf8proc_uint8_t*)str;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cpsrc, cpdst;
    size_t len;

    while (*usrc) {
        len = utf8proc_iterate(usrc, -1, &cpsrc);

        if (cpsrc == -1) {
            return UTF8_INVALID_UTF;
        }

        cpdst = cpsrc;
        if (utf8str_isspace_cp(cpsrc)) {
            space = 1;
        } else if (space) {
            cpdst = utf8proc_toupper(cpsrc);
            if (cp_length(cpdst) != len) {
                return UTF8_BUFFER_SMALL;
            }
            space = 0;
        }
        utf8proc_encode_char(cpdst, udst);

        usrc += len;
        udst += len;
    }

    return UTF8_OK;
}

enum utf8_result utf8str_scrub(char *str, char replace) {
    if (replace & 0x80) {
        return UTF8_INVALID_ARG;
    }
    if (str == NULL || *str == '\0') {
        return UTF8_OK;
    }

    utf8proc_uint8_t *usrc = (utf8proc_uint8_t*)str;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;
    size_t len;

    while (*usrc) {
        len = utf8proc_iterate(usrc, -1, &cp);

        if (cp == -1) {
            if (replace != '\0') {
                *udst = replace;
                udst++;
            }

            usrc++;
            continue;
        }

        if (usrc != udst) {
            utf8proc_encode_char(cp, udst);
        }

        usrc += len;
        udst += len;
    }
    if (usrc != udst) {
        *udst = '\0';
    }

    return UTF8_OK;
}

int utf8str_word_count(const char *str, const char *sep) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    int between = 1, cnt = 0;
    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;
    size_t len;

    while (*ustr) {
        len = utf8proc_iterate(ustr, -1, &cp);

        if (cp == -1) {
            return -1;
        }

        int issep = 0;
        if (sep == NULL || *sep == '\0') {
            issep = utf8str_isspace_cp(cp);
        } else {
            char srch[5] = {0};
            utf8proc_encode_char(cp, srch);
            issep = (strstr(sep, srch) != NULL);
        }

        if (issep) {
            between = 1;
        } else if (between) {
            between = 0;
            cnt++;
        }

        ustr += len;
    }

    return cnt;
}

enum utf8_result utf8str_translate(const char *src, char *dst, size_t *dst_sz, const char *what, const char *with) {
    if (what == NULL || (dst == NULL && dst_sz == NULL)) {
        return UTF8_INVALID_ARG;
    }
    if (src == NULL || *src == '\0') {
        if (dst) {
            *dst = '\0';
        }
        return UTF8_OK;
    }

    size_t whatlen = utf8str_count(what);
    if (whatlen == (size_t)-1) {
        return UTF8_INVALID_ARG;
    }
    size_t withlen = utf8str_count(with);
    if (withlen == (size_t)-1) {
        return UTF8_INVALID_ARG;
    }

    utf8proc_int32_t *whatarr = (utf8proc_int32_t *)malloc(sizeof(utf8proc_int32_t) * whatlen);
    if (whatarr == NULL) {
        return UTF8_OUT_OF_MEMORY;
    }
    utf8proc_int32_t *witharr = (utf8proc_int32_t *)malloc(sizeof(utf8proc_int32_t) * whatlen);
    if (witharr == NULL) {
        free(whatarr);
        return UTF8_OUT_OF_MEMORY;
    }

    utf8proc_uint8_t *utmp = (utf8proc_uint8_t*)what;
    utf8proc_int32_t cp, dcp;
    size_t idx = 0, len, dlen;

    for (idx = 0; idx < whatlen; ++idx) {
        len = utf8proc_iterate(utmp, -1, &cp);

        if (cp == -1) {
            free(whatarr);
            free(witharr);
            return UTF8_INVALID_UTF;
        }

        whatarr[idx] = cp;
        utmp += len;
    }

    utmp = (utf8proc_uint8_t*)with;
    for (idx = 0; idx < whatlen; ++idx) {
        if (*utmp == '\0') {
            witharr[idx] = 0;
            continue;
        }

        len = utf8proc_iterate(utmp, -1, &cp);

        if (cp == -1) {
            free(whatarr);
            free(witharr);
            return UTF8_INVALID_UTF;
        }

        witharr[idx] = cp;
        utmp += len;
    }

    size_t used = 0;
    utf8proc_uint8_t *usrc = (utf8proc_uint8_t*)src;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)dst;

    while (*usrc) {
        len = utf8proc_iterate(usrc, -1, &cp);

        if (cp == -1) {
            free(whatarr);
            free(witharr);
            return UTF8_INVALID_UTF;
        }

        dcp = cp;
        for (size_t i = 0; i < whatlen; ++i) {
            if (whatarr[i] == cp) {
                dcp = witharr[i];
                break;
            }
        }

        if (dcp != 0 && dst_sz != NULL && *dst_sz != 0 && used + cp_length(dcp) >= *dst_sz) {
            free(whatarr);
            free(witharr);
            if (udst) {
                *udst = '\0';
            }
            return UTF8_BUFFER_SMALL;
        }

        if (udst != NULL && dcp != 0) {
            dlen = utf8proc_encode_char(dcp, udst);
            udst += dlen;
        }

        used += cp_length(dcp);
        usrc += len;
    }

    if (udst != NULL) {
        *udst = '\0';
    }
    if (dst_sz != NULL) {
        *dst_sz = used;
    }

    free(whatarr);
    free(witharr);
    return UTF8_OK;
}

enum utf8_result utf8str_expand_tabs(const char *src, char *dst, size_t *dst_sz, size_t tab_sz) {
    if (tab_sz == 0 || tab_sz > 128) {
        return UTF8_INVALID_ARG;
    }
    if (dst == NULL && dst_sz == NULL) {
        return UTF8_INVALID_ARG;
    }
    if (src == NULL || *src == '\0') {
        if (dst != NULL) {
            *dst = '\0';
        }
        if (dst_sz != NULL) {
            *dst_sz = 0;
        }
        return UTF8_OK;
    }

    utf8proc_uint8_t *usrc = (utf8proc_uint8_t*)src;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)dst;
    utf8proc_int32_t cp;
    size_t len;
    size_t used = 0;

    while (*usrc) {
        len = utf8proc_iterate(usrc, -1, &cp);

        if (cp == -1) {
            return UTF8_INVALID_UTF;
        }

        if (cp == 0x09) {
            for (int i = 0; i < tab_sz; ++i) {
                if (dst_sz != NULL && *dst_sz != 0 && *dst_sz <= used + 1) {
                    return UTF8_BUFFER_SMALL;
                }

                if (udst != NULL) {
                    *udst = ' ';
                    ++udst;
                }

                ++used;
            }
        } else {
            if (dst_sz != NULL && *dst_sz != 0 && *dst_sz <= used + len) {
                return UTF8_BUFFER_SMALL;
            }

            if (udst != NULL) {
                utf8proc_encode_char(cp, udst);
                udst += len;
            }

            used += len;
        }

        usrc += len;
    }

    if (dst_sz != NULL) {
        *dst_sz = used;
    }

    return UTF8_OK;
}

enum utf8_result utf8str_squeeze(char *str, const char *what) {
    if (str == NULL || *str == '\0') {
        return UTF8_OK;
    }

    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)str;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp, prevcp = 0;
    size_t len;

    while (*ustr) {
        len = utf8proc_iterate(ustr, -1, &cp);

        if (cp == -1) {
            return UTF8_INVALID_UTF;
        }

        if (prevcp == 0 || prevcp != cp) {
            prevcp = cp;
            if (ustr != udst) {
                utf8proc_encode_char(cp, udst);
            }
            ustr += len;
            udst += len;
            continue;
        }

        if (what == NULL || *what == '\0') {
            if (prevcp == cp) {
                ustr += len;
            } else {
                if (ustr != udst) {
                    utf8proc_encode_char(cp, udst);
                }

                prevcp = cp;
                ustr += len;
                udst += len;
            }
        } else {
            char cstr[5] = {0};
            utf8proc_encode_char(cp, cstr);
            if (prevcp == cp && strstr(what, cstr) != NULL) {
                ustr += len;
            } else {
                if (ustr != udst) {
                    utf8proc_encode_char(cp, udst);
                }

                prevcp = cp;
                ustr += len;
                udst += len;
            }
        }
    }

    if (udst != ustr) {
        *udst = '\0';
    }

    return UTF8_OK;
}

enum utf8_result utf8str_strip(char *str, const char *what) {
    enum utf8_result res = utf8str_rstrip(str, what);
    if (res == UTF8_OK) {
        res = utf8str_lstrip(str, what);
    }

    return res;
}

enum utf8_result utf8str_rstrip(char *str, const char *what) {
    if (str == NULL || *str == '\0') {
        return UTF8_OK;
    }

    const char *lineend = &str[strlen(str)];
    utf8proc_uint8_t *stopper = (utf8proc_uint8_t*)str;

    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)utf8str_char_back(lineend);
    utf8proc_int32_t cp;
    size_t len;
    char cstr[5];

    while (ustr != stopper) {
        len = utf8proc_iterate(ustr, -1, &cp);

        if (cp == -1) {
            return UTF8_INVALID_UTF;
        }

        int remove = 0;
        if (what == NULL || *what == '\0') {
            remove = utf8str_isspace_cp(cp);
        } else {
            utf8proc_encode_char(cp, cstr);
            cstr[len] = '\0';
            remove = strstr(what, cstr) != NULL;
        }

        if (! remove) {
            break;
        }

        *ustr = '\0';
        ustr = (char *)utf8str_char_back(ustr);
    }

    return UTF8_OK;
}

enum utf8_result utf8str_lstrip(char *str, const char *what) {
    if (str == NULL || *str == '\0') {
        return UTF8_OK;
    }

    utf8proc_uint8_t *ustr = (utf8proc_uint8_t*)str;
    utf8proc_uint8_t *udst = (utf8proc_uint8_t*)str;
    utf8proc_int32_t cp;
    size_t len;
    char cstr[5];

    while (*ustr) {
        len = utf8proc_iterate(ustr, -1, &cp);

        if (cp == -1) {
            return UTF8_INVALID_UTF;
        }

        int remove = 0;
        if (what == NULL || *what == '\0') {
            remove = utf8str_isspace_cp(cp);
        } else {
            utf8proc_encode_char(cp, cstr);
            cstr[len] = '\0';
            remove = strstr(what, cstr) != NULL;
        }

        if (! remove) {
            break;
        }

        ustr += len;
    }

    if (ustr != udst) {
        while (*ustr) {
            len = utf8proc_iterate(ustr, -1, &cp);

            if (cp == -1) {
                return UTF8_INVALID_UTF;
            }

            utf8proc_encode_char(cp, udst);

            ustr += len;
            udst += len;
        }

        *udst = '\0';
    }

    return UTF8_OK;
}

enum utf8_result utf8str_rjustify(char *str, const char *with, size_t sz) {
    if (str == NULL) {
        return UTF8_INVALID_ARG;
    }

    const char *filling = (with == NULL || *with == '\0') ? " " : with;
    size_t fill_len = utf8str_count(filling);
    if (fill_len == (size_t)-1) {
        return UTF8_INVALID_UTF;
    }
    size_t str_len = utf8str_count(str);
    if (str_len == (size_t)-1) {
        return UTF8_INVALID_UTF;
    }

    if (str_len > sz) {
        return UTF8_TOO_LONG;
    }

    size_t to_add = sz - str_len;
    while (to_add > 0) {
        if (to_add >= fill_len) {
            strcat(str, filling);
            to_add -= fill_len;
        } else {
            strncat(str, filling, to_add);
            to_add = 0;
        }
    }

    return UTF8_OK;
}
