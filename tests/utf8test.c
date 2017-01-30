#include <stdio.h>

int skip_failed_pkg = 1;
#include "unittest.h"

int tests_run = 0;
int tests_fail = 0;

/*
const char* test_time_t() {
    time_t point = time(nullptr);
    time_t now = time(nullptr);

    string res = relative_date(point);
    ut_assert("now", res == std::string("just now"));
    point += 40;
    res = relative_date(point);
    ut_assert("40 seconds ahead", res == std::string("40 seconds from now"));
    point -= 80;
    res = relative_date(point);
    ut_assert("40 seconds back", res == std::string("40 seconds ago"));
    point -= 60*60*11;
    res = relative_date(point);
    ut_assert("11 hours back", res == std::string("11 hours ago"));

    return 0;
}
*/

const char * run_all_test() {
    /* ut_run_test("Translation test", test_translation); */
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

