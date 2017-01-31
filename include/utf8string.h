#ifndef UTF8STRING_20170130
#define UTF8STRING_20170130

#ifdef __cplusplus
extern "C"
{
#endif

enum utf8_error {
    UTF8_OK,
    UTF8_INVALID_ARG,
    UTF8_INVALID_UTF,
    UTF8_UNFINISHED,
    UTF8_TOO_SHORT,
};

/* Basic operations */
size_t utf8str_count(const char *str);
size_t utf8str_char_length(const char *str);
enum utf8_error utf8str_is_valid(const char *str, size_t len);
const char* utf8_at_index(const char *str, ssize_t index);

/*
at - utf8 symbol at position
upcase
lowcase
compare
compare_no_case
utf8width - for the object, char, char*
utf8width_first - first char of char*
substring
str_srch - cp, char & char*
starts_with
end_with
replace
trim
trim_right
trim_left
back - find the first byte of the symbol under pointer
last - go to last symbol
is_digit
is_whitespace
is_letter
is_letter_upcase
is_letter_lowcase
is_punct
*/

/*
2. Nice to have
str_char_iterator
str_word_iterator
reverse
upcase_first
lowcase_first
title_case
split/split_next(?)
scrub - replace invalid utf characters with placeholder
translate - tr/abc/def/
word_count
str_srch_right
repeat
chomp - remove the suffix

3. Maybe
justify - add spaces between words to make the string *width* if the string is longer than *limit*
center - add spaces from start and to end until *width* is reached
right_justify - add spaces to end until *width* is reached
left_justify - add spaces from start until *width* is reached
expand_tabs - convert tabs to spaces
squeeze - remove duplicate spaces
substr_count - number of occurances substr in string
 */

#ifdef __cplusplus
}
#endif

#endif /*UTF8STRING_20170130*/
