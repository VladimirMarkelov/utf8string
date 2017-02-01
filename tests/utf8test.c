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
    ut_assert("No size case", utf8str_upcase(str, buf, NULL) == UTF8_OK && strcmp(buf, "EXAMPLE ПРИМЕР") == 0);
    sz = 10;
    ut_assert("No dest case", utf8str_upcase(easy, buf, &sz) == UTF8_OK && strcmp(buf, "EX") == 0 && sz == 8);
    sz = 48;
    ut_assert("All case", utf8str_lowcase(str, buf, NULL) == UTF8_OK && strcmp(buf, "example пример") == 0);
    sz = 48;
    ut_assert("Invalid string case", utf8str_lowcase(invstr, buf, NULL) == UTF8_INVALID_UTF);
    sz = 10;
    ut_assert("Small destination case", utf8str_lowcase(str, buf, &sz) == UTF8_BUFFER_SMALL);
    sz = 10;
    ut_assert("Empty dest case", utf8str_upcase(easy, NULL, &sz) == UTF8_OK && sz == 8);

    return 0;
}

const char* test_utf_iequal() {
    ut_assert("NULL strings equal", utf8str_iequal(NULL, NULL) == UTF8_EQUAL);
    ut_assert("NULL string and non-NULL equal 1", utf8str_iequal("a", NULL) == UTF8_NEQUAL);
    ut_assert("NULL string and non-NULL equal 2", utf8str_iequal(NULL, "b") == UTF8_NEQUAL);
    ut_assert("ASCII string equal", utf8str_iequal("aBc", "Abc") == UTF8_EQUAL);
    ut_assert("ASCII string nequal", utf8str_iequal("aBd", "Abc") == UTF8_NEQUAL);
    ut_assert("ASCII string different length 1", utf8str_iequal("aBcd", "Abc") == UTF8_NEQUAL);
    ut_assert("ASCII string different length 2", utf8str_iequal("aBcd", "AbcDe") == UTF8_NEQUAL);
    ut_assert("UTF string equal", utf8str_iequal("приМер", "прИмЕр") == UTF8_EQUAL);
    ut_assert("UTF string nequal", utf8str_iequal("приМэр", "прИмЕр") == UTF8_NEQUAL);
    ut_assert("UTF string different length", utf8str_iequal("приМер", "прИмЕры") == UTF8_NEQUAL);
    return 0;
}

const char * run_all_test() {
    printf("=== Basic operations ===\n");
    ut_run_test("String Length", test_strlen);
    ut_run_test("Valid UTF", test_utf_valid);
    ut_run_test("Char at", test_utf_at);
    ut_run_test("Lowcase and Upcase", test_utf_cases);
    ut_run_test("Compare no case", test_utf_iequal);
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

