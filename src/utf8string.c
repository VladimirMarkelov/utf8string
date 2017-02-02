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
            return NULL;
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

    return (*tmp ? (char *)tmp : NULL);
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

        if (dest_sz != NULL) {
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
        *dest_sz = *dest_sz - used;
    }

    return UTF8_OK;
}

enum utf8_result utf8str_upcase(const char *src, char *dest, size_t *dest_sz) {
    return process_utf8str(src, dest, dest_sz, utf8proc_toupper);
}

enum utf8_result utf8str_lowcase(const char *src, char *dest, size_t *dest_sz) {
    return process_utf8str(src, dest, dest_sz, utf8proc_tolower);
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
