# UTF8String library ![](https://travis-ci.org/VladimirMarkelov/utf8string.svg?branch=master)
A C library to manipulate UTF-8 strings. It uses the external library [utf8proc](https://github.com/JuliaLang/utf8proc).

## Compatibility
The library was developed on Windows(Windows 7) and complied using MinGW toolkit.
It works without any issues on Linux (tested on Ubuntu 16.04 LTS).

## How to use
Either build the library with CMake and add the static library libutf8string.a and the header utf8string.h to your project. Or just add to your project files utf8proc.h, utf8string.h, utf8proc.c, utf8proc_data.c, and utf8string.c.

## Basic functions

* utf8str_count - Returns number of UTF8 characters in the string or (size_t)-1 if str points to invalid UTF8 sequence
* utf8str_char_length - Returns number of bytes occupied by the first UTF8 character in the string or 0 if the str is NULL or points to empty string
* utf8str_is_valid - Checks if a string is a valid UTF8 sequence
* utf8str_at_index - Returns pointer to a bytes that starts UTF8 character at position index
* utf8str_upcase - Transforms the string to uppercase letters
* utf8str_lowcase - Transforms the string to lowercase letters
* utf8str_upcase_inplace - Transforms the string to uppercase one inplace
* utf8str_lowcase_inplace - Transforms the string to lowercase one inplace
* utf8str_equal_no_case - Checks if two UTF8 strings equal(ignoring case)
* utf8str_nequal_no_case - Checks if both UTF8 strings starts with the same characters(ignoring case)
* utf8str_starts_with - Checks if UTF8 string starts with prefix(casesensitive)
* utf8str_ends_with - Checks if UTF8 string ends with suffix(casesensitive)
* utf8str_width - Retuns width of the string in monospace characters
* utf8str_substr - copy substring from src to dest
* utf8str_char_next - Moves pointer to the next UTF8 character in the string
* utf8str_char_back - Moves pointer to the previous UTF8 character in the string (no checks are done so it is possible to move out of the real string beginning)
* utf8str_char_back_safe - Moves pointer to the previous UTF8 character in the string only if str is not equal stopper. A safer version of the previous function
* utf8str_reverse - Reverses string
* utf8str_titlecase - Makes first charcters of all words uppercase
* utf8str_scrub - Validate byte sequence and replace or remove invalid characters to make the sequence valid UTF8 string
* utf8str_word_count - Calculates number of words in the string
* utf8str_translate - Replaces characters in the string
* utf8str_expand_tabs - Replace all tab characters with certain number of spaces
* utf8str_squeeze - Removes duplicated characters from the string
* utf8str_strip - Removes characters (spaces by default) from both string ends
* utf8str_rstrip - Removes characters (spaces by default) from right end
* utf8str_lstrip - Removes characters (spaces by default) from left end
* utf8str_rjustify - Adds characters from the right until the string reaches certain length
* utf8str_ljustify - Adds characters from the left until the string reaches certain length
* utf8str_justify - Centers text by adding characters from both ends of the string
* utf8str_mjustify - Adds spaces in the middle of the string between words evenly until the string reaches certain length
* utf8str_iter_init - Initializes iterator to extract word by word from the string
* utf8str_iter_next - Gets the next word from the string
* utf8str_iter_free - Frees all memory allocated to the iterator

## A set of function that similar to C ones but the following works with UTF8 characters.
All these functions works always with the first UTF8 character in the string *str*

* utf8str_isdigit
* utf8str_isspace
* utf8str_ispunct
* utf8str_islower
* utf8str_isupper
* utf8str_iscntrl
* utf8str_isalpha
* utf8str_isprint
