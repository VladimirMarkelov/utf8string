#include <string.h>
#include <stdlib.h>

#include "utf8string.h"

#ifdef DEBUG
    #include "logger.h"
#endif

#define UTF8_LOGFILE "utf8string.log"

size_t utf8str_len(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

// TODO:
    return 0;
}
