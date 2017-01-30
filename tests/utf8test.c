#include <stdio.h>
#include <string.h>
#include "utf8string.h"

int skip_failed_pkg = 1;
#include "unittest.h"

int tests_run = 0;
int tests_fail = 0;

const char* test_basic() {
    struct utf8string *ustr = utf8str_new();
    ut_assert("New", ustr != NULL && utf8str_to_char(ustr) != NULL);

    utf8str_free(ustr);
    ut_assert("Free", 1);

    ustr = utf8str_from_char("test");
    ut_assert("From char",
            ustr != NULL &&
            utf8str_to_char(ustr) != NULL &&
            strcmp(utf8str_to_char(ustr), "test") == 0 &&
            utf8str_capacity(ustr) == UTF8_INITIAL_BUFFER &&
            utf8str_used(ustr) == 4);

    utf8str_set_capacity(ustr, 128);
    ut_assert("Resize",
            utf8str_to_char(ustr) != NULL &&
            strcmp(utf8str_to_char(ustr), "test") == 0 &&
            utf8str_capacity(ustr) == 128 &&
            utf8str_used(ustr) == 4);

    utf8str_clear(ustr);
    ut_assert("Clear",
            utf8str_to_char(ustr) != NULL &&
            strlen(utf8str_to_char(ustr)) == 0 &&
            utf8str_capacity(ustr) == 128 &&
            utf8str_used(ustr) == 0);

    utf8str_reset(ustr);
    ut_assert("Reset",
            utf8str_to_char(ustr) != NULL &&
            strlen(utf8str_to_char(ustr)) == 0 &&
            utf8str_capacity(ustr) == UTF8_INITIAL_BUFFER &&
            utf8str_used(ustr) == 0);

    return 0;
}

const char * run_all_test() {
    ut_run_test("Basic tests", test_basic);
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

