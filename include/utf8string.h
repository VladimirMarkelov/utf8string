#ifndef UTF8STRING_20170130
#define UTF8STRING_20170130

#ifdef __cplusplus
extern "C"
{
#endif

enum utf8_result {
    UTF8_OK,
    UTF8_EQUAL,
    UTF8_NEQUAL,
    UTF8_INVALID_ARG,
    UTF8_INVALID_UTF,
    UTF8_UNFINISHED,
    UTF8_TOO_SHORT,
    UTF8_TOO_LONG,
    UTF8_BUFFER_SMALL,
    UTF8_OUT_OF_MEMORY,
    UTF8_NO_WORDS,
    UTF8_INVALID_ITERATOR,
};

struct utf8str_iter_internal_t;
/** Structure to keep information about word iterator.
 *  See functions utf8str_iter_*
 */
struct utf8str_iter_t {
    /** Internal information for iterator housekeeping.
     * Should not be exposed to any client
     */
    struct utf8str_iter_internal_t *info;
    /** Holds the pointer to the first character of the current word. It is
     *  valid only if result is UTF8_OK
     */
    char *begin;
    /** Holds the pointer to the first character after the current word. It is
     *  valid only if result is UTF8_OK.
     */
    char *end;
    /** The number of UTF8 characters in the current word - if result is UTF8_OK
     *  or in the last word if result is UTF8_NO_WORDS.
     *  To get word size in bytes just subtract begin from end
     */
    size_t char_count;
    /** The result of the last operation. Can be:
     *  UTF8_OK - the next word was found after calling utf8str_iter_next
     *  UTF8_NO_WORDS - the end of the string is reached, no word found. In this
     *      case begin, end, and char_count values are undefined
     *  UTF8_INVALID_ITERATOR - iterator is initialized but utf8str_iter_next
     *      has not been called yet, so all others fields are invalid
     */
    enum utf8_result result;
};

/* Basic operations */

/**
 * Returns number of UTF8 characters in the string or (size_t)-1 if str points to invalid UTF8 sequence
 */
size_t utf8str_count(const char *str);

/**
 * Returns number of bytes occupied by the first UTF8 character in the string or
 *   0 if the str is NULL or points to empty string. There is no checking whether
 *   UTF string is valid
 */
size_t utf8str_char_length(const char *str);

/**
 * Checks if a string is a valid UTF8 sequence
 * \param[in] len - number of bytes to test. If len is greater than the string
 *   lenght or len is 0 then the whole string is tested
 * Returns one of:
 *   UTF8_OK - the byte sequence is a valid UTF8 string (or string is empty)
 *   UTF8_INVALID_ARG - str is NULL
 *   UTF8_INVALID_UTF - string is not empty and is not a valid UTF8 sequence
 *   UTF8_UNFINISHED - valid UTF8 sequence ends with incomplete UTF8 character
 */

enum utf8_result utf8str_is_valid(const char *str, size_t len);

/** Returns pointer to a bytes that starts UTF8 character at position index
 *  If str is NULL or str points to invalid UTF8 sequence it returns NULL
 * \param[in] index - character index to find in the string. It can be negative,
 *   in this case the metod looks for the index from the end of the string. E.g,
 *   index=-1 means to get a pointer to the last UTF8 character.
 *   If index exceeds the number of characters in the string than the function
 *   returns pointer to the trailing zero byte(in case of index is positive) or
 *   original str(in case of index is negative)
 */
const char* utf8str_at_index(const char *str, ssize_t index);

/** Transforms the string to uppercase letters. It does not change original string
 * \param[in] src - original string
 * \param[out] dest - uppercased string will be copied to this buffer
 * \param[in,out] dest_sz - if dest_sz is NULL or equal 0 then the function does
 *   no check if the destination size is large enoug letters. It does not change original string
 *   \param[in] src - original string
 *   \param[out] dest - uppercased string will be copied to this buffer. It can be NULL, in this
 *       case the function only tries to transform the source and returns the required size
 *       for the result(without trailing '\0' character) if dest_sz is not NULL.
 *   \param[in,out] dest_sz - size of destination is bytes. If dest_sz is NULL or equal 0 then
 *     the function does no check if the destination size is large enough. If dest_sz is not
 *     NULL then at the end the function assigns the required size in bytes for the result
 *     (without ending '\0' character).
 * Can return:
 *  UTF8_OK - conversion successful
 *  UTF8_INVALID_ARG - in case of src is NULL
 *  UTF8_INVALID_UTF - in case of src is not a valid UTF8 sequence
 *  UTF8_BUFFER_SMALL - in case of dest_sz is not NULL and is not 0 and it is smaller than
 *      the space required for the converted string
 */
enum utf8_result utf8str_upcase(const char *src, char *dest, size_t *dest_sz);

/** Transforms the string to lowercase letters. It does not change original string
 * \param[in] src - original string
 * \param[out] dest - lowercased string will be copied to this buffer
 * \param[in,out] dest_sz - if dest_sz is NULL or equal 0 then the function does
 *   no check if the destination size is large enoug letters. It does not change original string
 *   \param[in] src - original string
 *   \param[out] dest - uppercased string will be copied to this buffer. It can be NULL, in this
 *       case the function only tries to transform the source and returns the required size
 *       for the result(without trailing '\0' character) if dest_sz is not NULL.
 *   \param[in,out] dest_sz - size of destination is bytes. If dest_sz is NULL or equal 0 then
 *     the function does no check if the destination size is large enough. If dest_sz is not
 *     NULL then at the end the function assigns the required size in bytes for the result
 *     (without ending '\0' character).
 * Can return:
 *  UTF8_OK - conversion successful
 *  UTF8_INVALID_ARG - in case of src is NULL
 *  UTF8_INVALID_UTF - in case of src is not a valid UTF8 sequence
 *  UTF8_BUFFER_SMALL - in case of dest_sz is not NULL and is not 0 and it is smaller than
 *      the space required for the converted string
 */
enum utf8_result utf8str_lowcase(const char *src, char *dest, size_t *dest_sz);

/** Transforms the string to uppercase one inplace.
 * \param[in] count - number of UTF8 characters to transform. If it is 0 then the whole
 *    string is transformed
 * Returns:
 *   UTF8_OK - conversion is successful
 *   UTF8_INVALID_ARG - str is NULL
 *   UTF8_INVALID_UTF - str points to the invalid UTF8 sequence
 *   UTF8_BUFFER_SMALL - if any uppercased characters require more bytes to store than
 *     original one, the transformation breaks and this error is returned
 */
enum utf8_result utf8str_upcase_inplace(char *src, size_t count);

/** Transforms the string to lowercase one inplace.
 * \param[in] count - number of UTF8 characters to transform. If it is 0 then the whole
 *    string is transformed
 * Returns:
 *   UTF8_OK - conversion is successful
 *   UTF8_INVALID_ARG - str is NULL
 *   UTF8_INVALID_UTF - str points to the invalid UTF8 sequence
 *   UTF8_BUFFER_SMALL - if any lowercased characters require more bytes to store than
 *     original one, the transformation breaks and this error is returned
 */
enum utf8_result utf8str_lowcase_inplace(char *src, size_t count);

/* Checks if two UTF8 strings equal(ignoring case).
 * Returns:
 *   UTF8_EQUAL - strings are the same
 *   UTF8_NEQUAL - strings differ
 *   UTF8_INVALID_UTF - one string(or both strings) has invalid UTF8 sequence
 */
enum utf8_result utf8str_equal_no_case(const char *orig, const char *cmp);

/* Checks if both UTF8 strings starts with the same characters(ignoring case).
 * \param[in] len - maximum number of characters to compare. If len is 0 then
 *    the function compares the whole strings(as utf8str_equal_no_case does)
 * Returns:
 *   UTF8_EQUAL - strings are the same
 *   UTF8_NEQUAL - strings differ
 *   UTF8_INVALID_UTF - one string(or both strings) has invalid UTF8 sequence
 */
enum utf8_result utf8str_nequal_no_case(const char *orig, const char *cmp, size_t len);

/* Checks if UTF8 string starts with prefix(casesensitive).
 * Returns:
 *   UTF8_EQUAL - strings are the same
 *   UTF8_NEQUAL - strings differ
 *   UTF8_INVALID_UTF - one string(or both strings) has invalid UTF8 sequence
 */
enum utf8_result utf8str_starts_with(const char *orig, const char *cmp);

/* Checks if UTF8 string ends with suffix(casesensitive).
 * Returns:
 *   UTF8_EQUAL - strings are the same
 *   UTF8_NEQUAL - strings differ
 *   UTF8_INVALID_UTF - one string(or both strings) has invalid UTF8 sequence
 */
enum utf8_result utf8str_ends_with(const char *orig, const char *cmp);

/** Retuns non-zero value if the first character of str is digit
 * Digit is any character from UTF8 categories:
 *     Nd - Number, Decimal Digit
 *     Nl - Number, Letter
 *     No - Number, Other
 */
int utf8str_isdigit(const char *str);

/** Retuns non-zero value if the first character of str is whitespace
 * Whitespace is any character from UTF8 category Zs (Separator, Space) or
 *   character with code between 0x09 and 0x0D
 */
int utf8str_isspace(const char *str);

/** Retuns non-zero value if the first character of str is of punctiation type
 * Punctuation is any character from UTF8 categories:
 *     Pc - Punctuation, Connector
 *     Pd - Punctuation, Dash
 *     Pe - Punctuation, Close
 *     Pf - Punctuation, Final quote
 *     Pi - Punctuation, Initial quote
 *     Po - Punctuation, Other
 *     Ps - Punctuation, Open
 */
int utf8str_ispunct(const char *str);

/** Retuns non-zero value if the first character of str is lowcase letter
 * Lowcase letter is any character from UTF8 categories:
 *     Ll - Letter, Lowcase
 */
int utf8str_islower(const char *str);

/** Retuns non-zero value if the first character of str is uppercase letter
 * Lowcase letter is any character from UTF8 categories:
 *     Lu - Letter, Uppercase
 *     Lt - Letter, Titlecase
 */
int utf8str_isupper(const char *str);

/** Retuns non-zero value if the first character of str is control charcter
 * Control character is any character from UTF8 categories:
 *     Cc - Other, Control
 */
int utf8str_iscntrl(const char *str);

/** Retuns non-zero value if the first character of str is a letter
 * Letter is any character from UTF8 categories:
 *     Lu - Letter, Uppercase
 *     Lt - Letter, Titlecase
 *     Ll - Letter, Lowercase
 *     Lo - Letter, Other
 */
int utf8str_isalpha(const char *str);

/** Retuns non-zero value if the first character of str is printable
 * Printable is any character that is not modifier, control, mark, or spacing
 *    character
 */
int utf8str_isprint(const char *str);

/** Returns the total width in monospace characters of the first len UTF8
 *   characters of the string
 */
size_t utf8str_width(const char *str, size_t len);

/** Copies len UTF8 characters from str to dest starting from UTF8 character start.
 *    \param[in] start - UTF8 index of the first character to copy. If start is
 *        greater than string length then UTF8_INVALID_ARG is returned
 *    \param[in] len - number of UTF8 characters to copy. The function copies as
 *        many characters as possible. So, if len exceeds string length then
 *        the whole string is copied and UTF8_OK is returned
 *    \param[in] dest - pointer to buffer to copy the substring. If it is NULL then
 *       the function just checks if source string is valid and whether dest_sz
 *       is large enough to keep the substring
 *    \param[in,out] dest_sz - size of destination buffer in bytes. If it is NULL
 *       or equal 0 no checks for destination size is done. If it is not 0 then
 *       the function may return UTF8_BUFFER_SMALL in case of the buffer too
 *       short to keep the substring. If extracting substring is successful
 *       and dest_sz is not NULL then the function assigns the number of really
 *       used bytes for substring to dest_sz(not including the trailing '\0'
 *       character. So, setting dest to NULL and *dest_sz to 0 you can get how
 *       big destination should be to store the substring.
 * Returns UTF8_OK if substring is exctracted without any issue, UTF8_INVALID_UTF
 *    if the source is not valid UTF8 sequence, or UTF8_BUFFER_SMALL if destination
 *    size is not big enough to store the substring
 */
enum utf8_result utf8str_substr(const char *str, ssize_t start, ssize_t len, char *dest, size_t *dest_sz);

/** Moves pointer to the next UTF8 character in string if it is possible(the
 *    pointer is not NULL and it does not point to '\0' character). No UTF8
 *    validation checks are done
 */
const char* utf8str_char_next(const char *str);

/** Moves pointer to the next UTF8 character in string if pointer is not NULL.
 *   No validation checks are done, so the function is dangerous, e.g, in case
 *   of you call it for the first character
 */
const char* utf8str_char_back(const char *str);

/** Moves pointer to the next UTF8 character in string if pointer is not NULL
 *   and it is not equal stopper pointer.
 *   Safer version of utf8str_char_back because it allows to set string beginning
 *   as a stopper, so you never pass string boundaries while going back
 */
const char* utf8str_char_back_safe(const char *str, const char *stopper);

/* extra functions */

/** Reverses a string
 * Retuns:
 *    UTF8_OK - if the operation completed successfully
 *    UTF8_INVALID_UTF - if the str is not a valid UTF8 sequence
 *    UTF8_OUT_OF_MEMORY - the function allocates a temporary buffer for the
 *      reversed string and if it fails it returns out-of-memory error
 */
enum utf8_result utf8str_reverse(char *str);

/** Make the first characher of every word in the string to uppercase
 * Retuns:
 *    UTF8_OK - if the operation completed successfully
 *    UTF8_INVALID_UTF - if the str is not a valid UTF8 sequence
 */
enum utf8_result utf8str_titlecase(char *str);


/** Make the string a valid UTF8 sequence. If any character is not valid it is
 *   replaced with 'replace'.
 *  \param[in] replace - ASCII character to replace every invalid byte. If
 *    'replace' is '\0' then invalid characters are removed from original string
 *    instead of being replaced
 * Retuns:
 *    UTF8_OK - if the operation completed successfully
 *    UTF8_INVALID_ARG - if 'replace' is not valid ASCII character (>= 0x80)
 */
enum utf8_result utf8str_scrub(char *str, char replace);

/** Returns the number of word in the string or -1 if the string is not a
 *    valid UTF8 sequence
 *  \param[in] sep - if sep is NULL or empty string then the separator is
 *    space. Otherwise a new word start after any character from the sep.
 *    A few separators in a row are considered as one separator.
 *  Examples:
 *     utf8str_word_count("abc def hig", NULL) == 3
 *     utf8str_word_count("abc  def hig", NULL) == 3 (two first spaces are
 *        calculated as one separator)
 *     utf8str_word_count("abc,;def;hig", ",;") == 3 (first ',;' is treated as
 *        one separator)
 */
int utf8str_word_count(const char *str, const char *sep);

/** Replace characters from 'what' with corresponding character from 'with'.
 *     If 'with' is shorter than 'what' then the characters that do not have
 *     corresponding 'with' characters are just removed from the string. So,
 *     you can easily eliminate any characters from the string just by setting
 *     'what' and NULL 'with'.
 *  \param[in] dst - destination for the transformed string. It can be NULL - in
 *     this case dst_sz must not be NULL because in this case the function just
 *     calculates the final size of transformed string and assigns it to dst_sz
 *  \param[in,out] dst_sz - size in bytes of the destination. If it is NULL or 0
 *     then no checks for destination size is done. If it is not NULL then after
 *     the string is transformed a size in bytes of final string(not including
 *     the trailing '\0') is assigned to dst_sz
 *  \param[in] what - set of characters to replace. It must not be NULL
 *  \param[in] with - set of characters to replace with. It can be NULL - in this
 *     case all characters in 'what' will be deleted from the result string
 *  Example:
 *     utf8str_translate("1920", d, NULL, "01", "20") - replaces all '0's to '2's and
 *         '1's to '0's at the same time. The result string is "0922".
 *     utf8str_translate("1920", d, NULL, "01", "2") - replaces all '0's to '2's and
 *         removes all '1's. The result string is "922".
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_ARG - dst is NULL and dst_sz is NULL at the same time, or
 *        'with' or 'what' is not valid UTF8 sequence
 *     UTF8_OUT_OF_MEMORY - the function allocates memory for internal structures,
 *        so it may fail and return this error
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *     UTF8_BUFFER_SMALL - dst_sz is not 0 and it is too small to keep all the
 *        result string
 */
enum utf8_result utf8str_translate(const char *src, char *dst, size_t *dst_sz, const char *what, const char *with);

/** Replaces all TABs to tab_sz number of spaces and put the result to dst
 *  \param[in] src - a source UTF8 string with TABs
 *  \param[out] dst - a result UTF8 string with spaces. If dst is NULL and
 *      dst_sz is not NULL then the function just calculates how much space
 *      a new string requires and sets dst_sz with required size
 *  \param[in,out] dst_sz - a buffer size, set it to NULL if you want to get the
 *      size of result string
 *  \param[in] tab_sz - a size of a TAB is space characters
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_ARG - dst is NULL and dst_sz is NULL at the same time, or
 *        tab_sz == 0 or tab_sz > 128
 *        so it may fail and return this error
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *     UTF8_BUFFER_SMALL - dst_sz is not 0 and it is too small to keep all the
 *        result string
 */
enum utf8_result utf8str_expand_tabs(const char *str, char *dst, size_t *dst_sz, size_t tab_sz);

/** Deletes adjacent repeated characters. If what is empty or NULL then it deletes
 *      all repeated characters, otherwise it deletes only repeated characters
 *      that are in what string
 *  \param[in,out] str - a UTF8 string that will be replaced in the end
 *  \param[in] what - a set of characters to replace
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *  Examples:
 *      utf8str_squeeze("exxa  mple", NULL) == "exa mple"
 *      utf8str_squeeze("exxxammmpppple", "xp") == "exammmple"
 */
enum utf8_result utf8str_squeeze(char *str, const char *what);

/** Trims a string from both ends. At first it tries to trim from right and
 *      if it was successful it trims from left
 *  \param[in,out] str - a UTF8 string that will be replaced in the end
 *  \param[in] what - a set of characters to trim. If it is empty or NULL then
 *      the function trims all UTF8 space characters (space, CR, LF, TAB etc)
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *  Examples:
 *      utf8str_strip("\rexample \0x09 \n", NULL) == "example"
 *      utf8str_strip("examples", "osle") == "examp"
 */
enum utf8_result utf8str_strip(char *str, const char *what);
/** See utf8str_strip */
enum utf8_result utf8str_rstrip(char *str, const char *what);
/** See utf8str_strip */
enum utf8_result utf8str_lstrip(char *str, const char *what);

/** Adds a characters to the end of a string until its length reaches sz
 *  \param[in,out] str - a UTF8 string that will be replaced in the end
 *  \param[in] with - a filler. If it is empty or NULL then space character
 *      is used to fill the string
 *  \param[in] sz - a required string length
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_ARG - src is NULL
 *     UTF8_TOO_LONG - a string is already longer than sz (not an error, just
 *      an informative warning)
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *  Examples:
 *      utf8str_rjustify("example", NULL, 9) == "example  "
 *      utf8str_rjustify("example", " *>", 12) == "example *> *"
 */
enum utf8_result utf8str_rjustify(char *str, const char *with, size_t sz);

/** Adds a characters to the beginning of a string until its length reaches sz
 *  \param[in,out] str - a UTF8 string that will be replaced in the end
 *  \param[in] with - a filler. If it is empty or NULL then space character
 *      is used to fill the string
 *  \param[in] sz - a required string length
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_ARG - src is NULL
 *     UTF8_TOO_LONG - a string is already longer than sz (not an error, just
 *      an informative warning)
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *     UTF8_OUT_OF_MEMORY - the function allocates memory for internal structures,
 *        so it may fail and return this error
 *  Examples:
 *      utf8str_ljustify("example", NULL, 9) == "  example"
 *      utf8str_ljustify("example", " *>", 12) == " *> *example"
 */
enum utf8_result utf8str_ljustify(char *str, const char *with, size_t sz);

/** Adds a characters from both ends of a string until its length reaches sz.
 *      If the number of characters to add is not even then more is added to
 *      the end of the string
 *  \param[in,out] str - a UTF8 string that will be replaced in the end
 *  \param[in] with - a filler. If it is empty or NULL then space character
 *      is used to fill the string
 *  \param[in] sz - a required string length
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_ARG - src is NULL
 *     UTF8_TOO_LONG - a string is already longer than sz (not an error, just
 *      an informative warning)
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *     UTF8_OUT_OF_MEMORY - the function allocates memory for internal structures,
 *        so it may fail and return this error
 *  Examples:
 *      utf8str_justify("example", NULL, 10) == "  example "
 *      utf8str_justify("example", " *>", 12) == " *example *>"
 */
enum utf8_result utf8str_justify(char *str, const char *with, size_t sz);

/** Adds a spaces between words until its length reaches sz size.
 *  \param[in,out] str - a UTF8 string that will be replaced in the end
 *  \param[in] sz - a required string length
 *  Returns:
 *     UTF8_OK - transformation is successful
 *     UTF8_INVALID_ARG - src is NULL
 *     UTF8_TOO_LONG - a string is already longer than sz (not an error, just
 *      an informative warning)
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *     UTF8_NO_WORDS - the string does not have space character inside it
 *  Examples:
 *      utf8str_justify("example", 10) == "example" and returns UTF8_NO_WORDS
 *      utf8str_justify("exam exam exam", 16) == "exam  exam  exam"
 */
enum utf8_result utf8str_mjustify(char *str, size_t sz);

/*
------------ word iterator functions ----------------------------
They all should be re-enterant
*/
/** Initializes a word iterator. Right after calling initialization the
 *  iterator is in invalid state and cannot be used to get the current word,
 *  you have to call utf8str_iter_next first.
 *  \param[in] str - a UTF8 string that will be iterated
 *  \param[in] include - a set of characters that make a word, all other
 *      characters are treated as word separators
 *  \param[in] exclude - a set of characters that are word separators
 *  Note: if both include and exclude are set then only exclude works. In other
 *      words, in this case the function works as if include is NULL. If both
 *      include and exclude are NULL then the function skip all whitespaces
 *      between words
 *  Note: initialization makes a copy of include and exclude sets, so it is
 *      safe to delete them right after an iterator intialization. From the
 *      other hand, the original string is not copied - be sure to keep the
 *      original string until you finish iterating
 *  Returns:
 *      NULL in case of error (out of memory or original string is NULL)
 *      or poiner to initalized iterator
 */
struct utf8str_iter_t* utf8str_iter_init(char *str, const char *include,
        const char *exclude);
/** Finds the next word in the source string. The function does not do any
 *      memory allocations or copying, it just returns the next word limits that
 *      can be read from iter fields (See description of structure
 *      utf8str_iter_t)
 *  \param[in] iter - an initialized word iterator
 *  Returns:
 *     UTF8_OK - freeing memory is successful
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *     UTF8_INVALID_ITERATOR - passed iter is NULL, or not initialized, or
 *      you tries to call next after the end of string is reached
 *     UTF8_NO_WORDS - the iterator has reached the end of the string. In this
 *      case no word is found and begin and end pointers are not valid
 */
enum utf8_result utf8str_iter_next(struct utf8str_iter_t *iter);
/** Frees a memory allocated for the iterator by initialization.
 *  \param[in] iter - an initialized word iterator. It is safe to free a
 *      pointer that is NULL
 *  Returns:
 *     UTF8_OK - freeing memory is successful
 *     UTF8_INVALID_ITERATOR - passed iter is not NULL but the iterator looks
 *      like it has been already freed some time ago, or it is broken
 */
enum utf8_result utf8str_iter_free(struct utf8str_iter_t *iter);

#ifdef __cplusplus
}
#endif

#endif /*UTF8STRING_20170130*/
