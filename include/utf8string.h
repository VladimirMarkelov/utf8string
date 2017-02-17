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
 *     UTF8_OUT_OF_MEMORY - the function allocated memory for internal structures,
 *        so it may fail and return this error
 *     UTF8_INVALID_UTF - src is not a vaild UTF8 sequence
 *     UTF8_BUFFER_SMALL - dst_sz is not 0 and it is too small to keep all the
 *        result string
 */
enum utf8_result utf8str_translate(const char *src, char *dst, size_t *dst_sz, const char *what, const char *with);

enum utf8_result utf8str_expand_tabs(const char *str, char *dst, size_t *dst_sz, size_t tab_sz);
enum utf8_result utf8str_squeeze(char *str, const char *what);
enum utf8_result utf8str_strip(char *str, const char *what);
enum utf8_result utf8str_rstrip(char *str, const char *what);
enum utf8_result utf8str_lstrip(char *str, const char *what);
enum utf8_result utf8str_rjustify(char *str, const char *with, size_t sz);

/*
3. Maybe
justify - add spaces between words to make the string *width* if the string is longer than *limit*
center - add spaces from start and to end until *width* is reached
right_justify - add spaces to end until *width* is reached
left_justify - add spaces from start until *width* is reached
word_iterator
*/

#ifdef __cplusplus
}
#endif

#endif /*UTF8STRING_20170130*/
