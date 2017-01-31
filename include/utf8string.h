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
size_t utf8str_len(const char *str);
size_t utf8str_char_length(const char *str);
enum utf8_error utf8str_is_valid(const char *str, size_t len);

/*
str_cat - char & char*
is_valid - check if it contains valid utf
count - number of utf8 symbols
len - number of bytes used
delete - char, substring, and (from, to)
insert - substring and char
append - substring and char
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

#ifdef __cplusplus
}
#endif

#endif /*UTF8STRING_20170130*/
