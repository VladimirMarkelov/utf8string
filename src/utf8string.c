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

enum utf8_error utf8str_is_valid(const char *str, size_t len) {
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

const char* utf8_at_index(const char *str, ssize_t index) {
    if (str == NULL || index == 0) {
        return str;
    }

    return str;
}
