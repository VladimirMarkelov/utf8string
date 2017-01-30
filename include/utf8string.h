struct utf8string;

#define UTF8_INITIAL_BUFFER 64

/* Basic operations */
struct utf8string * utf8str_new();
void utf8str_free(struct utf8string *str);
struct utf8string * utf8str_from_char(const char *str);
const char * utf8str_to_char(const struct utf8string *str);
struct utf8string * utf8str_duplicate(const struct utf8string *str);
void utf8str_clear(struct utf8string *str);
void utf8str_reset(struct utf8string *str);
int utf8str_set_capacity(struct utf8string *str, size_t new_capacity);
size_t utf8str_capacity(const struct utf8string *str);
size_t utf8str_used(const struct utf8string *str);

size_t utf8str_len(const struct utf8string *str);
