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

const char * run_all_test() {
    printf("=== Basic operations ===\n");
    ut_run_test("String Length", test_strlen);
    ut_run_test("Valid UTF", test_utf_valid);
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

