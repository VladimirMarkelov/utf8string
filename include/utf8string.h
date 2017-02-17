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

/* Checks if UTF8 strings starts with the same characters(ignoring case).
 * \param[in] len - maximum number of characters to compare. If len is 0 then
 *    the function compares the whole strings(as utf8str_equal_no_case does)
 * Returns:
 *   UTF8_EQUAL - strings are the same
 *   UTF8_NEQUAL - strings differ
 *   UTF8_INVALID_UTF - one string(or both strings) has invalid UTF8 sequence
 */
enum utf8_result utf8str_nequal_no_case(const char *orig, const char *cmp, size_t len);
enum utf8_result utf8str_starts_with(const char *orig, const char *cmp);
enum utf8_result utf8str_ends_with(const char *orig, const char *cmp);
int utf8str_isdigit(const char *str);
int utf8str_isspace(const char *str);
int utf8str_ispunct(const char *str);
int utf8str_islower(const char *str);
int utf8str_isupper(const char *str);
int utf8str_iscntrl(const char *str);
int utf8str_isalpha(const char *str);
int utf8str_isprint(const char *str);
size_t utf8str_width(const char *str, size_t len);
int utf8str_substr(const char *str, ssize_t start, ssize_t len, char *dest, size_t *dest_sz);
const char* utf8str_char_next(const char *str);
const char* utf8str_char_back(const char *str);
const char* utf8str_char_back_safe(const char *str, const char *stopper);

/* extra functions */
enum utf8_result utf8str_reverse(char *str);
enum utf8_result utf8str_titlecase(char *str);
enum utf8_result utf8str_scrub(char *str, char replace);
int utf8str_word_count(const char *str, const char *sep);
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
