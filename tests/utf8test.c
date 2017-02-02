#include <stdio.h>
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
    ut_assert("Short string at", utf8str_at_index(ascii, 10) == NULL);
    ut_assert("Short string at [negative]", utf8str_at_index(ascii, -10) == NULL);
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

const char * run_all_test() {
    printf("=== Basic operations ===\n");
    ut_run_test("String Length", test_strlen);
    ut_run_test("Valid UTF", test_utf_valid);
    ut_run_test("Char at", test_utf_at);
    ut_run_test("Lowcase and Upcase", test_utf_cases);
    ut_run_test("Compare no case", test_utf_iequal);
    ut_run_test("Starts and ends with", test_utf_starts);
    ut_run_test("Categories", test_utf_categories);
    ut_run_test("Width", test_utf_width);
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

