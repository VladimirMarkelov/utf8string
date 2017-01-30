#include <string.h>
#include <stdlib.h>

#include "utf8string.h"

#ifdef DEBUG
    #include "logger.h"
#endif

#define UTF8_LOGFILE "utf8string.log"

struct utf8string {
    char *str;
    /* maximum available capacity. Increased automatically if it is needed */
    size_t capacity;
    /* bytes used - can be -1, in this case it is equal strlen(str) */
    size_t used;
    /* size of string in UTF8 characters. -1 if it is not calculated yet */
    size_t len;
};

/*
 * Internal functions
 */
static struct utf8string *utf8str_allocate_struct() {
    struct utf8string *str = (struct utf8string *)malloc(sizeof(*str));
    if (str == NULL) {
#ifdef DEBUG
        log_error(UTF8_LOGFILE, 1, "Allocate: Out of memory");
#endif
        return NULL;
    }

    str->used = (size_t)-1;
    str->len = (size_t)-1;
    str->str = NULL;
    str->capacity = 0;

    return str;
}

static int utf8str_reallocate_buffer(struct utf8string *ustr, size_t buf_size) {
    if (ustr == NULL) {
        return 0;
    }

    if (buf_size < UTF8_INITIAL_BUFFER || buf_size == (size_t)-1) {
        buf_size = UTF8_INITIAL_BUFFER;
    }

    if (ustr->capacity >= buf_size) {
        return 1;
    }

    char *tmp = (char *)malloc(sizeof(char) * buf_size);
    if (tmp == NULL) {
#ifdef DEBUG
        log_error(UTF8_LOGFILE, 1, "Reallocate: Out of memory");
#endif
        return 0;
    }

    *tmp = '\0';
    if (ustr->str != NULL) {
        strcpy(tmp, ustr->str);
        free(ustr->str);
    }

    ustr->capacity = buf_size;
    ustr->str = tmp;

    return 1;
}

/*
 * Basic operations
 */
struct utf8string * utf8str_new() {
    struct utf8string *str = utf8str_allocate_struct();
    if (str == NULL) {
        return NULL;
    }

    utf8str_reallocate_buffer(str, 0);

    if (str->str == NULL) {
        free(str);
#ifdef DEBUG
        log_error(UTF8_LOGFILE, 1, "str_new: Out of memory");
#endif
        return NULL;
    }

    return str;
}

void utf8str_free(struct utf8string *str) {
    if (str == NULL) {
        return;
    }

    if (str->str != NULL) {
        free(str->str);
    }

    free(str);
}

struct utf8string * utf8str_from_char(const char *str) {
    struct utf8string *ustr = utf8str_allocate_struct();
    if (ustr == NULL) {
        return NULL;
    }

    size_t len = (str == NULL ? 0 : strlen(str));

    if (! utf8str_reallocate_buffer(ustr, len + 1)) {
#ifdef DEBUG
        log_error(UTF8_LOGFILE, 1, "from_char: Out of memory");
#endif
        free(ustr);
        return NULL;
    }

    strcpy(ustr->str, str);
    ustr->used = len;

    return ustr;
}

const char * utf8str_to_char(const struct utf8string *str) {
    if (str == NULL) {
        return NULL;
    }

    return str->str;
}

struct utf8string * utf8str_duplicate(const struct utf8string *str) {
    if (str == NULL) {
        return NULL;
    }

    struct utf8string *ustr = utf8str_allocate_struct();
    if (ustr == NULL) {
#ifdef DEBUG
        log_error(UTF8_LOGFILE, 1, "duplicate: Out of memory");
#endif
        return NULL;
    }

    if (! utf8str_reallocate_buffer(ustr, str->used + 1)) {
#ifdef DEBUG
        log_error(UTF8_LOGFILE, 1, "duplicate: Out of memory");
#endif
        free(ustr);
        return NULL;
    }

    strcpy(ustr->str, str->str);
    ustr->used = str->used;
    ustr->len = str->len;

    return ustr;
}

void utf8str_clear(struct utf8string *str) {
    if (str == NULL || str->used == 0) {
        return;
    }

    str->used = 0;
    str->len = 0;
    *(str->str) = '\0';
}

void utf8str_reset(struct utf8string *str) {
    if (str == NULL) {
        return;
    }

    if (str->capacity == UTF8_INITIAL_BUFFER) {
        utf8str_clear(str);
    } else {
        free(str->str);
        str->str = NULL;
        str->used = 0;
        str->capacity = 0;
        if (! utf8str_reallocate_buffer(str, UTF8_INITIAL_BUFFER)) {
#ifdef DEBUG
        log_error(UTF8_LOGFILE, 1, "reset: Out of memory");
#endif
            return;
        }
    }
}

int utf8str_set_capacity(struct utf8string *str, size_t new_capacity) {
    if (str == NULL) {
        return 1;
    }

    if (new_capacity < UTF8_INITIAL_BUFFER) {
        return 2;
    }

    if (new_capacity <= str->capacity) {
        return 0;
    }

    if (! utf8str_reallocate_buffer(str, new_capacity)) {
        return 3;
    }

    return 0;
}

size_t utf8str_capacity(const struct utf8string *str) {
    if (str == NULL) {
        return 0;
    }

    return str->capacity;
}

size_t utf8str_used(const struct utf8string *str) {
    if (str == NULL) {
        return 0;
    }

    if (str->used != (size_t)-1) {
        return str->used;
    }

    size_t len = strlen(str->str);
    ((struct utf8string*)str)->used = len;
    return len;
}

size_t utf8str_len(const struct utf8string *str) {
// TODO:
    return 0;
}
