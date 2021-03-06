﻿#include <stdio.h>
#include <string.h>
#include "utf8string.h"

int skip_failed_pkg = 1;
#include "unittest.h"

int tests_run = 0;
int tests_fail = 0;

const char* test_strlen() {
    char *ascii = "example";
    char *utfstr = "пример";

    ut_assert("NULL string length", utf8str_count(NULL) == 0);
    ut_assert("Empty string length", utf8str_count("") == 0);
    ut_assert("ASCII string length", utf8str_count("example") == strlen("example"));
    ut_assert("UTF8 string length", utf8str_count("-пример-") == 8);
    ut_assert("Invalid string length", utf8str_count("\xf8\xe3") == -1);

    return 0;
}

const char* test_utf_valid() {
    char *ascii = "example";
    char *utfstr = "примерпример";
    char *invalid ="example\xf8\xe3";

    ut_assert("NULL string valid", utf8str_is_valid(NULL, 10) == UTF8_INVALID_ARG);
    ut_assert("Zero length string valid", utf8str_is_valid(ascii, 0) == UTF8_OK);
    ut_assert("ASCII valid", utf8str_is_valid(ascii, 0) == UTF8_OK);
    ut_assert("ASCII part valid", utf8str_is_valid(ascii, 4) == UTF8_OK);
    ut_assert("ASCII part valid - too short", utf8str_is_valid(ascii, 40) == UTF8_TOO_SHORT);
    ut_assert("UTF8 valid", utf8str_is_valid(utfstr, 0) == UTF8_OK);
    ut_assert("UTF8 part valid", utf8str_is_valid(utfstr, 8) == UTF8_OK);
    ut_assert("UTF8 part valid - unfinished", utf8str_is_valid(utfstr, 9) == UTF8_UNFINISHED);
    ut_assert("UTF8 invalid", utf8str_is_valid(invalid, 0) == UTF8_INVALID_UTF);
    ut_assert("UTF8 invalid - good start", utf8str_is_valid(invalid, 7) == UTF8_OK);

    return 0;
}

const char* test_utf_at() {
    char *ascii = "example";
    char *utfstr = "пример";

    ut_assert("NULL string at", utf8str_at_index(NULL, 2) == NULL);
    const char *last = utf8str_at_index(ascii, 10);
    ut_assert("Short string at", last != NULL && *last == '\0');
    ut_assert("Short string at [negative]", utf8str_at_index(ascii, -10) == ascii);
    ut_assert("ASCII string at", utf8str_at_index(ascii, 5) != NULL && (*utf8str_at_index(ascii, 5) == 'l'));
    ut_assert("ASCII string at [negative]", utf8str_at_index(ascii, -3) != NULL && (*utf8str_at_index(ascii, -3) == 'p'));
    ut_assert("UTF string at", utf8str_at_index(utfstr, 2) != NULL && (*utf8str_at_index(utfstr, 2) == utfstr[4]));
    ut_assert("UTF string at [negative]", utf8str_at_index(utfstr, -2) != NULL && (*utf8str_at_index(utfstr, -2) == utfstr[8]));

    return 0;
}

const char* test_utf_cases() {
    char *str = "exAmpLe пРимЕр";
    char *invstr = "exAmpLe\x89\xe6 пРимЕр";
    char buf[48];
    char *easy = "ex";
    size_t sz = 20;

    ut_assert("NULL string case", utf8str_upcase(NULL, NULL, &sz) == UTF8_INVALID_ARG);
    int r = utf8str_upcase(str, NULL, &sz);
    ut_assert("Detect size case", utf8str_upcase(str, NULL, &sz) == UTF8_OK && sz == 20);
    ut_assert("No size case", utf8str_upcase(str, buf, NULL) == UTF8_OK && strcmp(buf, "EXAMPLE ПРИМЕР") == 0);
    sz = 10;
    ut_assert("No dest case", utf8str_upcase(easy, buf, &sz) == UTF8_OK && strcmp(buf, "EX") == 0 && sz == 2);
    sz = 48;
    ut_assert("All case", utf8str_lowcase(str, buf, NULL) == UTF8_OK && strcmp(buf, "example пример") == 0);
    sz = 48;
    ut_assert("Invalid string case", utf8str_lowcase(invstr, buf, NULL) == UTF8_INVALID_UTF);
    sz = 10;
    ut_assert("Small destination case", utf8str_lowcase(str, buf, &sz) == UTF8_BUFFER_SMALL);
    sz = 10;
    ut_assert("Empty dest case", utf8str_upcase(easy, NULL, &sz) == UTF8_OK && sz == 2);

    return 0;
}

const char* test_utf_cases_inplace() {
    char *str = "exAmpLe пРимЕр";
    char buf[48];

    ut_assert("NULL string case", utf8str_upcase_inplace(NULL, 0) == UTF8_INVALID_ARG);
    strcpy(buf, str);
    int r = utf8str_upcase_inplace(buf, 0);
    ut_assert("No size case", r == UTF8_OK && strcmp(buf, "EXAMPLE ПРИМЕР") == 0);
    strcpy(buf, str);
    r = utf8str_upcase_inplace(buf, 4);
    ut_assert("Only 4 first", r == UTF8_OK && strcmp(buf, "EXAMpLe пРимЕр") == 0);

    return 0;
}

const char* test_utf_iequal() {
    ut_assert("NULL strings equal", utf8str_equal_no_case(NULL, NULL) == UTF8_EQUAL);
    ut_assert("NULL string and non-NULL equal 1", utf8str_equal_no_case("a", NULL) == UTF8_NEQUAL);
    ut_assert("NULL string and non-NULL equal 2", utf8str_equal_no_case(NULL, "b") == UTF8_NEQUAL);
    ut_assert("ASCII string equal", utf8str_equal_no_case("aBc", "Abc") == UTF8_EQUAL);
    ut_assert("ASCII string not equal", utf8str_equal_no_case("aBd", "Abc") == UTF8_NEQUAL);
    ut_assert("ASCII string different length 1", utf8str_equal_no_case("aBcd", "Abc") == UTF8_NEQUAL);
    ut_assert("ASCII string different length 2", utf8str_equal_no_case("aBcd", "AbcDe") == UTF8_NEQUAL);
    ut_assert("UTF string equal", utf8str_equal_no_case("приМер", "прИмЕр") == UTF8_EQUAL);
    ut_assert("UTF string not equal", utf8str_equal_no_case("приМэр", "прИмЕр") == UTF8_NEQUAL);
    ut_assert("UTF string different length", utf8str_equal_no_case("приМер", "прИмЕры") == UTF8_NEQUAL);

    ut_assert("ASCII string equal partially", utf8str_nequal_no_case("aBce", "Abcd", 3) == UTF8_EQUAL);
    ut_assert("UTF string equal partially", utf8str_nequal_no_case("приМэр", "прИмЕр", 4) == UTF8_EQUAL);
    ut_assert("ASCII string nequal short strings", utf8str_nequal_no_case("aBc", "Abc", 6) == UTF8_EQUAL);
    ut_assert("ASCII string not nequal short strings", utf8str_nequal_no_case("aBcd", "Abce", 6) == UTF8_NEQUAL);
    ut_assert("ASCII string not nequal different lengths", utf8str_nequal_no_case("aBcd", "Abcde", 6) == UTF8_NEQUAL);
    return 0;
}

const char* test_utf_starts() {
    ut_assert("NULL strings starts", utf8str_starts_with(NULL, NULL) == UTF8_EQUAL);
    ut_assert("Equal strings starts", utf8str_starts_with("abc", "abc") == UTF8_EQUAL);
    ut_assert("Longer orig strings starts", utf8str_starts_with("abcd", "abc") == UTF8_EQUAL);
    ut_assert("Longer cmp strings starts", utf8str_starts_with("abc", "abcd") == UTF8_NEQUAL);
    ut_assert("Unequal strings starts", utf8str_starts_with("abcdef", "abchgj") == UTF8_NEQUAL);

    ut_assert("NULL strings ends", utf8str_ends_with(NULL, NULL) == UTF8_EQUAL);
    ut_assert("Equal strings ends", utf8str_ends_with("abc", "abc") == UTF8_EQUAL);
    ut_assert("Longer orig strings ends", utf8str_ends_with("dabc", "abc") == UTF8_EQUAL);
    ut_assert("Longer cmp strings ends", utf8str_ends_with("abc", "abcd") == UTF8_NEQUAL);
    ut_assert("Unequal strings ends", utf8str_ends_with("abcdef", "abchgj") == UTF8_NEQUAL);
    ut_assert("UTF Equal strings ends", utf8str_ends_with("пример", "мер") == UTF8_EQUAL);
    ut_assert("UTF unequal strings ends", utf8str_ends_with("пример", "прима") == UTF8_NEQUAL);
    return 0;
}

const char* test_utf_categories() {
    ut_assert("NULL is not digit", !utf8str_isdigit(NULL));
    ut_assert("Empty string is not digit", !utf8str_isdigit(""));
    ut_assert("1a is digit", utf8str_isdigit("1a"));
    ut_assert("a1 is not digit", !utf8str_isdigit("a1"));
    ut_assert("ASCII isupper", utf8str_isupper("A"));
    ut_assert("ASCII not isupper", !utf8str_isupper("a"));
    ut_assert("ASCII islower", utf8str_islower("z"));
    ut_assert("ASCII not islower", !utf8str_islower("Z"));
    ut_assert("ASCII isalpha", utf8str_isalpha("y"));
    ut_assert("ASCII iscntrl", utf8str_iscntrl("\x8"));
    ut_assert("ASCII isspace space", utf8str_isspace(" "));
    ut_assert("ASCII isspace tab", utf8str_isspace("\x9 "));
    ut_assert("ASCII ispunct -", utf8str_ispunct("-"));
    ut_assert("ASCII ispunct .", utf8str_ispunct("."));
    ut_assert("UTF isupper", utf8str_isupper("Ф"));
    ut_assert("UTF not isupper", !utf8str_isupper("ф"));
    ut_assert("UTF islower", utf8str_islower("á"));
    ut_assert("UTF not islower", !utf8str_islower("Á"));
    ut_assert("UTF isalpha", utf8str_isalpha("ŋ"));

    return 0;
}

const char* test_utf_width() {
    ut_assert("NULL width", utf8str_width(NULL, 10) == 0);
    ut_assert("Empty string width", utf8str_width("", 10) == 0);
    ut_assert("ASCII string width", utf8str_width("abcd", 10) == 4);
    ut_assert("ASCII char width", utf8str_width("abcd", 1) == 1);
    ut_assert("Non-printable width", utf8str_width("\x08\x04\x11u\x12", 0) == 1);
    ut_assert("UTF string width", utf8str_width("пример", 0) == 6);
    ut_assert("UTF char width", utf8str_width("пример", 1) == 1);
    return 0;
}

const char* test_utf_substring() {
    const char *ascii = "example";
    const char *utf = "пример";
    char buf[64];
    size_t sz;

    ut_assert("NULL substring", utf8str_substr(NULL, 1, 1, NULL, NULL) == UTF8_INVALID_ARG);
    ut_assert("NULL buffer substring", utf8str_substr(ascii, 1, 1, NULL, NULL) == UTF8_OK);
    sz = 64;
    ut_assert("NULL buffer detect size substring", utf8str_substr(ascii, 1, 3, NULL, &sz) == UTF8_OK && sz == 3);
    sz = 3;
    ut_assert("NULL buffer small substring", utf8str_substr(ascii, 1, 3, buf, &sz) == UTF8_BUFFER_SMALL);
    sz = 64;
    ut_assert("ASCII substring - full", utf8str_substr(ascii, 1, 3, buf, &sz) == UTF8_OK && strcmp(buf, "xam") == 0 && sz == 3);
    sz = 64;
    int res = utf8str_substr(ascii, 2, 10, buf, &sz);
    ut_assert("ASCII substring - short", res == UTF8_OK && strcmp(buf, "ample") == 0 && sz == 5);
    sz = 64;
    ut_assert("ASCII substring negative = short", utf8str_substr(ascii, -1, 3, buf, &sz) == UTF8_OK && strcmp(buf, "e") == 0 && sz == 1);
    sz = 64;
    ut_assert("ASCII substring negative = full", utf8str_substr(ascii, -4, 3, buf, &sz) == UTF8_OK && strcmp(buf, "mpl") == 0 && sz == 3);
    sz = 64;
    res = utf8str_substr(utf, 1, 3, buf, &sz);
    ut_assert("UTF substring - full", res == UTF8_OK && strcmp(buf, "рим") == 0 && sz == 6);
    sz = 64;
    ut_assert("UTF substring - short", utf8str_substr(utf, 2, 10, buf, &sz) == UTF8_OK && strcmp(buf, "имер") == 0 && sz == 8);
    sz = 64;
    ut_assert("UTF substring negative = short", utf8str_substr(utf, -1, 3, buf, &sz) == UTF8_OK && strcmp(buf, "р") == 0 && sz == 2);
    sz = 64;
    ut_assert("UTF substring negative = full", utf8str_substr(utf, -4, 3, buf, &sz) == UTF8_OK && strcmp(buf, "име") == 0 && sz == 6);

    return 0;
}

const char* test_utf_moving() {
    const char *ascii = "example";
    const char *utf = "пример";

    const char *p = utf8str_at_index(ascii, 2);
    p = utf8str_char_next(p);
    ut_assert("ASCII next", p != NULL && *p == 'm');
    p = utf8str_at_index(ascii, 6);
    const char *p2 = utf8str_char_next(p);
    p = utf8str_at_index(ascii, 7);
    ut_assert("Char next at end", p2 == p && *p == '\0');
    p = utf8str_at_index(ascii, 2);
    p = utf8str_char_back(p);
    ut_assert("ASCII prev", p != NULL && *p == 'x');
    p = utf8str_char_back_safe(p, ascii);
    ut_assert("ASCII next prev safe", p != NULL && *p == 'e');
    p = utf8str_char_back_safe(p, ascii);
    ut_assert("ASCII next prev safe at beginning", p != NULL && p == ascii);

    p = utf8str_at_index(utf, 2);
    p2 = utf8str_at_index(utf, 3);
    p = utf8str_char_next(p);
    ut_assert("UTF next", p == p2);
    ++p;
    p = utf8str_char_next(p);
    p2 = utf8str_char_next(p2);
    ut_assert("UTF next from middle", p == p2);
    p2 = utf8str_at_index(utf, 3);
    p = utf8str_char_back(p);
    ut_assert("UTF prev", p == p2);
    p2 = utf8str_char_back(p2);
    --p;
    p = utf8str_char_back(p);
    ut_assert("UTF prev from middle", p == p2);

    return 0;
}

const char* test_utf_reverse() {
    char ascii[] = "example";
    char utf[] = "пример";
    char invstr[] = "exAmpLe\x89\xe6 пРимЕр";

    int r = utf8str_reverse(NULL);
    ut_assert("NULL string", r == UTF8_OK);
    r = utf8str_reverse("");
    ut_assert("Empty string", r == UTF8_OK);
    r = utf8str_reverse(invstr);
    ut_assert("Invalid string reverse", r == UTF8_INVALID_UTF && strcmp(invstr, "exAmpLe\x89\xe6 пРимЕр") == 0);
    r = utf8str_reverse(ascii);
    ut_assert("ASCII reverse", r == UTF8_OK && strcmp(ascii, "elpmaxe") == 0);
    r = utf8str_reverse(utf);
    ut_assert("UTF reverse", r == UTF8_OK && strcmp(utf, "ремирп") == 0);

    return 0;
}

const char* test_utf_title_case() {
    char ascii[] = "example Example example";
    char utf[] = "пример Пример\x09пример";
    char invstr[] = "exAmpLe тест\x89\xe6 пРимЕр";

    int r = utf8str_titlecase(NULL);
    ut_assert("NULL string", r == UTF8_OK);
    r = utf8str_titlecase("");
    ut_assert("Empty string", r == UTF8_OK);
    r = utf8str_titlecase(invstr);
    ut_assert("Invalid string title case", r == UTF8_INVALID_UTF && strcmp(invstr, "ExAmpLe Тест\x89\xe6 пРимЕр") == 0);
    r = utf8str_titlecase(ascii);
    ut_assert("ASCII title case", r == UTF8_OK && strcmp(ascii, "Example Example Example") == 0);
    r = utf8str_titlecase(utf);
    ut_assert("UTF title case", r == UTF8_OK && strcmp(utf, "Пример Пример\x09Пример") == 0);

    return 0;
}

const char* test_utf_scrub() {
    char ascii[] = "example Example example";
    char utf[] = "пример Пример\x09пример";
    char invstr[] = "exAmpLe тест\x89\xe6 пРимЕр";
    char invstr2[] = "exAmpLe тест\x89\xe6 пРимЕр";

    int r = utf8str_scrub(NULL, ' ');
    ut_assert("NULL string", r == UTF8_OK);
    r = utf8str_scrub("", ' ');
    ut_assert("Empty string", r == UTF8_OK);
    r = utf8str_scrub(ascii, '\x89');
    ut_assert("ASCII scrub - invalid replacemnt", r == UTF8_INVALID_ARG && strcmp(ascii, "example Example example") == 0);
    r = utf8str_scrub(ascii, ' ');
    ut_assert("ASCII scrub", r == UTF8_OK && strcmp(ascii, "example Example example") == 0);
    r = utf8str_scrub(utf, ' ');
    ut_assert("UTF scrub", r == UTF8_OK && strcmp(utf, "пример Пример\x09пример") == 0);
    r = utf8str_scrub(invstr, ' ');
    ut_assert("Invalid string scrub", r == UTF8_OK && strcmp(invstr, "exAmpLe тест   пРимЕр") == 0);
    r = utf8str_scrub(invstr2, '\0');
    ut_assert("Invalid string scrub compress", r == UTF8_OK && strcmp(invstr2, "exAmpLe тест пРимЕр") == 0);

    return 0;
}

const char* test_utf_word_count() {
    char ascii[] = "example Example  example";
    char ascii_ex[] = "   example Example  example   ";
    char utf[] = "пример\x0DПример\x0D \x09\x0Aпример";
    char invstr[] = "exAmpLe тест\x89\xe6 пРимЕр";

    int r = utf8str_word_count(NULL, NULL);
    ut_assert("NULL string", r == 0);
    r = utf8str_word_count("", NULL);
    ut_assert("Empty string", r == 0);
    r = utf8str_word_count(invstr, NULL);
    ut_assert("Invalid word count - default", r == -1);
    r = utf8str_word_count(ascii, NULL);
    ut_assert("ASCII word count - default", r == 3);
    r = utf8str_word_count(ascii, " ");
    ut_assert("ASCII word count - spaces", r == 3);
    r = utf8str_word_count(utf, NULL);
    ut_assert("UTF word count - default", r == 3);
    r = utf8str_word_count(utf, "\x0D\x0A");
    ut_assert("UTF word count - line end", r == 4);
    r = utf8str_word_count(ascii_ex, NULL);
    ut_assert("ASCII word count - ext ascii", r == 3);
    r = utf8str_word_count("one_word", NULL);
    ut_assert("ASCII word count - one word", r == 1);

    return 0;
}

const char* test_utf_translate() {
    char text[] = "пример\x0D example\x0D \x09\x0Aпример";
    char dst[64] = "tst";
    size_t sz = 64;

    int r = utf8str_translate(NULL, dst, NULL, "a", "b");
    ut_assert("NULL string", r == UTF8_OK && *dst == '\0');
    r = utf8str_translate(text, NULL, NULL, "a", "b");
    ut_assert("NULL dest", r == UTF8_INVALID_ARG);
    r = utf8str_translate(text, NULL, &sz, "\x89", "b");
    ut_assert("Invalid what", r == UTF8_INVALID_ARG);
    r = utf8str_translate(text, NULL, &sz, "e", "\x95");
    ut_assert("Invalid with", r == UTF8_INVALID_ARG);
    r = utf8str_translate("abcd\x89gef", dst, &sz, "b", "f");
    ut_assert("Invalid source UTF", r == UTF8_INVALID_UTF);

    r = utf8str_translate(text, dst, NULL, "e", "b");
    ut_assert("No dest size", r == UTF8_OK && strcmp(dst, "пример\x0D bxamplb\x0D \x09\x0Aпример") == 0);
    sz = 0;
    r = utf8str_translate(text, dst, &sz, "e", "b");
    ut_assert("Calculate size", r == UTF8_OK && sz == 37);
    sz = 10;
    r = utf8str_translate(text, dst, &sz, "a", "b");
    ut_assert("Dest small", r == UTF8_BUFFER_SMALL && strcmp(dst, "прим") == 0);
    sz = 64;
    r = utf8str_translate(text, dst, &sz, "\x0D\x0A", "");
    ut_assert("Remove end-of-lines", r == UTF8_OK && strcmp(dst, "пример example \x09пример") == 0);
    r = utf8str_translate(text, dst, NULL, "x", "á");
    ut_assert("Bigger replace", r == UTF8_OK && strcmp(dst, "пример\x0D eáample\x0D \x09\x0Aпример") == 0);
    r = utf8str_translate(text, dst, NULL, "и", "I");
    ut_assert("Smaller replace", r == UTF8_OK && strcmp(dst, "прIмер\x0D example\x0D \x09\x0AпрIмер") == 0);

    return 0;
}

const char* test_utf_tab_replace() {
    char text[] = "пример\x09 example \x09пример";
    char dst[64] = "";
    size_t sz;

    int r = utf8str_expand_tabs(NULL, dst, &sz, 4);
    ut_assert("NULL string", r == UTF8_OK && *dst == '\0' && sz == 0);
    sz = 0;
    r = utf8str_expand_tabs("a\x09", NULL, &sz, 4);
    ut_assert("Calculate", r == UTF8_OK && sz == 5);
    r = utf8str_expand_tabs("a\x09", NULL, NULL, 4);
    ut_assert("No dest", r == UTF8_INVALID_ARG);
    r = utf8str_expand_tabs("a\x09", dst, NULL, 0);
    ut_assert("Invalid tab size", r == UTF8_INVALID_ARG);
    sz = 5;
    r = utf8str_expand_tabs("a\x09", dst, &sz, 4);
    ut_assert("Small buffer", r == UTF8_BUFFER_SMALL);
    sz = 64;
    size_t orig = strlen(text);
    r = utf8str_expand_tabs(text, dst, &sz, 4);
    ut_assert("Expand normal", r == UTF8_OK && sz == orig+2*4-2 && strcmp(dst, "пример     example     пример") == 0);

    return 0;
}

const char* test_utf_squeeze() {
    char ascii[] = "exxample  exammmmmpppple==";
    char ascii2[] = "exxample  exammmmmpppple==";
    char utf[] = "  приммерр \x09 exaample  примеееерррр";
    char utf2[] = "  приммерр \x09 exaample  примеееерррр";

    int r = utf8str_squeeze(NULL, NULL);
    ut_assert("NULL string", r == UTF8_OK);
    r = utf8str_squeeze(ascii, NULL);
    ut_assert("NULL char list", r == UTF8_OK && strcmp(ascii, "example example=") == 0);
    r = utf8str_squeeze(ascii2, "xp");
    ut_assert("XP in char list", r == UTF8_OK && strcmp(ascii2, "example  exammmmmple==") == 0);
    r = utf8str_squeeze(utf, NULL);
    ut_assert("UTF NULL char list", r == UTF8_OK && strcmp(utf, " пример \x09 example пример") == 0);
    r = utf8str_squeeze(utf2, "рa");
    ut_assert("UTF UTF&NON-UTF in char list", r == UTF8_OK && strcmp(utf2, "  приммер \x09 example  примеееер") == 0);

    return 0;
}

const char* test_utf_strip() {
    char s0[] = "exámple  akña";
    char s1[] = "   exámple  akña   ";
    char s2[] = "   exámple  akña   ";
    char s3[] = "   exámple  akña   ";
    char s10[] = "+é-  exámple  akña  -è+";

    int r = utf8str_strip(NULL, NULL);
    ut_assert("NULL string", r == UTF8_OK);
    r = utf8str_strip(s1, NULL);
    ut_assert("Default strip charset", r == UTF8_OK && strcmp(s1, "exámple  akña") == 0);
    r = utf8str_lstrip(s2, NULL);
    ut_assert("Strip spaces from left", r == UTF8_OK && strcmp(s2, "exámple  akña   ") == 0);
    r = utf8str_rstrip(s3, NULL);
    ut_assert("Strip spaces from right", r == UTF8_OK && strcmp(s3, "   exámple  akña") == 0);
    r = utf8str_strip(s10, "+-éè");
    ut_assert("Strip custom charset", r == UTF8_OK && strcmp(s10, "  exámple  akña  ") == 0);

    return 0;
}

const char* test_utf_justify() {
    char s0[128] = "exámple  akña\0";
    char s1[128] = "exámple  akña\0";
    char s2[128] = "exámple  akña\0";
    char s3[128] = "exámple  akña  some  text  два\0";
    char s4[128] = "exámple  akña  some  text  два\0";
    char short_str[16] = "example";
    char short_str2[16] = " exam ";
    char short_str3[16] = " example ";
    size_t clen = utf8str_count(s3);

    int r = utf8str_rjustify(NULL, NULL, 16);
    ut_assert("NULL string", r == UTF8_INVALID_ARG);
    r = utf8str_rjustify(s0, NULL, 9);
    ut_assert("RJustify too long", r == UTF8_TOO_LONG && strcmp(s0, "exámple  akña") == 0);
    r = utf8str_rjustify(s0, NULL, 14);
    ut_assert("RJustify default filler", r == UTF8_OK && strcmp(s0, "exámple  akña ") == 0);
    r = utf8str_rjustify(s0, "filler", 22);
    ut_assert("RJustify custom filler", r == UTF8_OK && strcmp(s0, "exámple  akña fillerfi") == 0);

    r = utf8str_ljustify(NULL, NULL, 16);
    ut_assert("NULL string", r == UTF8_INVALID_ARG);
    r = utf8str_ljustify(s1, NULL, 9);
    ut_assert("LJustify too long", r == UTF8_TOO_LONG && strcmp(s1, "exámple  akña") == 0);
    r = utf8str_ljustify(s1, NULL, 15);
    ut_assert("LJustify default filler", r == UTF8_OK && strcmp(s1, "  exámple  akña") == 0);
    r = utf8str_ljustify(s1, "filler", 23);
    ut_assert("LJustify custom filler", r == UTF8_OK && strcmp(s1, "fillerfi  exámple  akña") == 0);
    r = utf8str_ljustify(s1, "filler", 29);
    ut_assert("LJustify custom filler", r == UTF8_OK && strcmp(s1, "fillerfillerfi  exámple  akña") == 0);

    r = utf8str_justify(NULL, NULL, 16);
    ut_assert("NULL string", r == UTF8_INVALID_ARG);
    r = utf8str_justify(s2, NULL, 9);
    ut_assert("Justify too long", r == UTF8_TOO_LONG && strcmp(s2, "exámple  akña") == 0);
    r = utf8str_justify(s2, NULL, 16);
    ut_assert("Justify default filler", r == UTF8_OK && strcmp(s2, " exámple  akña  ") == 0);
    r = utf8str_justify(s2, "filler", 23);
    ut_assert("Justify custom filler", r == UTF8_OK && strcmp(s2, "fil exámple  akña  fill") == 0);
    r = utf8str_justify(s2, "filler", 35);
    ut_assert("Justify custom filler", r == UTF8_OK && strcmp(s2, "fillerfil exámple  akña  fillfiller") == 0);

    r = utf8str_mjustify(NULL, 16);
    ut_assert("NULL string", r == UTF8_INVALID_ARG);
    r = utf8str_mjustify(short_str,  9);
    ut_assert("MJustify no words", r == UTF8_NO_WORDS && strcmp(short_str, "example") == 0);
    r = utf8str_mjustify(short_str2,  9);
    ut_assert("MJustify no words - 2", r == UTF8_NO_WORDS && strcmp(short_str2, " exam ") == 0);
    r = utf8str_mjustify(short_str3,  9);
    ut_assert("MJustify no words - 3", r == UTF8_OK && strcmp(short_str3, " example ") == 0);
    r = utf8str_mjustify(s3,  9);
    ut_assert("MJustify too long", r == UTF8_TOO_LONG && strcmp(s3, "exámple  akña  some  text  два") == 0);
    r = utf8str_mjustify(s3, clen + 3);
    ut_assert("MJustify less than word count", r == UTF8_OK && strcmp(s3, "exámple  akña   some   text   два") == 0);
    strcpy(s3, s4);
    r = utf8str_mjustify(s3, clen + 5);
    ut_assert("MJustify greater than word count by 1", r == UTF8_OK && strcmp(s3, "exámple   akña   some   text    два") == 0);
    strcpy(s3, s4);
    r = utf8str_mjustify(s3, clen + 4);
    ut_assert("MJustify equal fill", r == UTF8_OK && strcmp(s3, "exámple   akña   some   text   два") == 0);
    strcpy(s3, s4);
    r = utf8str_mjustify(s3, clen + 10);
    ut_assert("MJustify greater than word count by some", r == UTF8_OK && strcmp(s3, "exámple    akña     some    text     два") == 0);

    return 0;
}

const char* test_utf_word_iterator() {
    char s1[32] = "one\0";
    char s2[32] = "one twoo\0";
    char s3[32] = "\t one\r two     three \t\0";
    char s4[32] = "1234 a4512b192жд0786нг\0";
    char s5[64] = "The test: a-good-one test, that long\0";

    struct utf8str_iter_t* iter;
    int r;
    iter = utf8str_iter_init(NULL, NULL, NULL);
    ut_assert("NULL string", iter == NULL);
    r = utf8str_iter_next(iter);
    ut_assert("NULL iterator", r == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_free(iter);
    ut_assert("NULL iterator free", r == UTF8_OK);

    iter = utf8str_iter_init(s1, NULL, NULL);
    ut_assert("One word init", iter != NULL && iter->result == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_next(iter);
    ut_assert("One word: first", r == UTF8_OK
            && iter->begin != NULL && strcmp(s1, iter->begin) == 0
            && iter->char_count == 3 && *iter->end == '\0');
    r = utf8str_iter_next(iter);
    ut_assert("One word: no more", r == UTF8_NO_WORDS);
    r = utf8str_iter_next(iter);
    ut_assert("One word: no more invalid iter", r == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_free(iter);
    ut_assert("One word iterator free", r == UTF8_OK);

    iter = utf8str_iter_init(s2, NULL, NULL);
    ut_assert("Two words init", iter != NULL && iter->result == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_next(iter);
    ut_assert("Two words: first", r == UTF8_OK
            && iter->begin != NULL && strcmp(s2, iter->begin) == 0
            && iter->char_count == 3 && *iter->end == ' '
            && iter->result == UTF8_OK);
    r = utf8str_iter_next(iter);
    ut_assert("Two words: second", r == UTF8_OK
            && iter->begin != NULL && strcmp(iter->begin, "twoo") == 0
            && iter->char_count == 4 && *iter->end == '\0'
            && iter->result == UTF8_OK);
    r = utf8str_iter_next(iter);
    ut_assert("Two words: no more", r == UTF8_NO_WORDS && iter->result == UTF8_NO_WORDS);
    r = utf8str_iter_next(iter);
    ut_assert("Two words: no more invalid iter", r == UTF8_INVALID_ITERATOR && iter->result == UTF8_NO_WORDS);
    r = utf8str_iter_free(iter);
    ut_assert("Two words iterator free", r == UTF8_OK);

    iter = utf8str_iter_init(s3, NULL, NULL);
    ut_assert("Three words init", iter != NULL && iter->result == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_next(iter);
    ut_assert("Three words: first", r == UTF8_OK
            && iter->begin != NULL && strcmp("one\r two     three \t\0", iter->begin) == 0
            && iter->char_count == 3 && *iter->end == '\r'
            && iter->end - iter->begin == 3);
    r = utf8str_iter_next(iter);
    ut_assert("Three words: second", r == UTF8_OK
            && iter->begin != NULL && strcmp(iter->begin, "two     three \t\0") == 0
            && iter->char_count == 3 && *iter->end == ' '
            && iter->end - iter->begin == 3);
    r = utf8str_iter_next(iter);
    ut_assert("Three words: third", r == UTF8_OK
            && iter->begin != NULL && strcmp(iter->begin, "three \t\0") == 0
            && iter->char_count == 5 && *iter->end == ' '
            && iter->end - iter->begin == 5);
    r = utf8str_iter_next(iter);
    ut_assert("Three words: no more", r == UTF8_NO_WORDS);
    r = utf8str_iter_next(iter);
    ut_assert("Three words: no more invalid iter", r == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_free(iter);
    ut_assert("Three words iterator free", r == UTF8_OK);

    iter = utf8str_iter_init(s4, "1234567890", NULL);
    ut_assert("Include init", iter != NULL && iter->result == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_next(iter);
    ut_assert("Include: first", r == UTF8_OK
            && iter->begin != NULL && strcmp(s4, iter->begin) == 0
            && iter->char_count == 4 && *iter->end == ' '
            && iter->end - iter->begin == 4);
    r = utf8str_iter_next(iter);
    ut_assert("Include: second", r == UTF8_OK
            && iter->begin != NULL && strcmp("4512b192жд0786нг", iter->begin) == 0
            && iter->char_count == 4 && *iter->end == 'b'
            && iter->end - iter->begin == 4);
    r = utf8str_iter_next(iter);
    ut_assert("Include: third", r == UTF8_OK
            && iter->begin != NULL && strcmp("192жд0786нг", iter->begin) == 0
            && iter->char_count == 3 && strcmp(iter->end, "жд0786нг") == 0
            && iter->end - iter->begin == 3);
    r = utf8str_iter_next(iter);
    ut_assert("Include: forth", r == UTF8_OK
            && iter->begin != NULL && strcmp("0786нг", iter->begin) == 0
            && iter->char_count == 4 && strcmp(iter->end, "нг") == 0
            && iter->end - iter->begin == 4);
    r = utf8str_iter_next(iter);
    ut_assert("Include: no more", r == UTF8_NO_WORDS);
    r = utf8str_iter_next(iter);
    ut_assert("Include: no more invalid iter", r == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_free(iter);
    ut_assert("Include iterator free", r == UTF8_OK);

    iter = utf8str_iter_init(s5, NULL, " :,");
    ut_assert("Exclude init", iter != NULL && iter->result == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: first", r == UTF8_OK
            && iter->begin != NULL && strcmp(s5, iter->begin) == 0
            && iter->char_count == 3 && *iter->end == ' '
            && iter->end - iter->begin == 3);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: second", r == UTF8_OK
            && iter->begin != NULL && strcmp("test: a-good-one test, that long", iter->begin) == 0
            && iter->char_count == 4 && *iter->end == ':'
            && iter->end - iter->begin == 4);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: third", r == UTF8_OK
            && iter->begin != NULL && strcmp("a-good-one test, that long", iter->begin) == 0
            && iter->char_count == 10 && *iter->end == ' '
            && iter->end - iter->begin == 10);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: forth", r == UTF8_OK);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: fifth", r == UTF8_OK);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: sixth", r == UTF8_OK
            && iter->begin != NULL && strcmp("long", iter->begin) == 0
            && iter->char_count == 4 && *iter->end == '\0'
            && iter->end - iter->begin == 4);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: no more", r == UTF8_NO_WORDS);
    r = utf8str_iter_next(iter);
    ut_assert("Exclude: no more invalid iter", r == UTF8_INVALID_ITERATOR);
    r = utf8str_iter_free(iter);
    ut_assert("Exclude iterator free", r == UTF8_OK);

    return 0;
}

const char * run_all_test() {
    printf("=== Basic operations ===\n");
    ut_run_test("String Length", test_strlen);
    ut_run_test("Valid UTF", test_utf_valid);
    ut_run_test("Char at", test_utf_at);
    ut_run_test("Lowcase and Upcase", test_utf_cases);
    ut_run_test("Lowcase and Upcase inplace", test_utf_cases_inplace);
    ut_run_test("Compare no case", test_utf_iequal);
    ut_run_test("Starts and ends with", test_utf_starts);
    ut_run_test("Categories", test_utf_categories);
    ut_run_test("Width", test_utf_width);
    ut_run_test("Substring", test_utf_substring);
    ut_run_test("Char next and back", test_utf_moving);
    ut_run_test("Reverse", test_utf_reverse);

    printf("\n=== Extra functions ===\n");
    ut_run_test("Title case", test_utf_title_case);
    ut_run_test("Scrub", test_utf_scrub);
    ut_run_test("Word count", test_utf_word_count);
    ut_run_test("Translate", test_utf_translate);
    ut_run_test("Replace tabs", test_utf_tab_replace);
    ut_run_test("Sqeeze", test_utf_squeeze);
    ut_run_test("Strip", test_utf_strip);
    ut_run_test("Justify", test_utf_justify);
    ut_run_test("Word iteration", test_utf_word_iterator);
    return 0;
}

int main() {
    const char* res = run_all_test();
    if (res && tests_fail == 0) {
        printf("%s\n", res);
    } else {
        printf("Tests run: %d\nSuccess: %d\nFail: %d\n", tests_run, tests_run - tests_fail, tests_fail);
    }
    return 0;
}

