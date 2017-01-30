#define ut_assert(name, test) do {\
                printf("    Checking %s .. ", name); \
                    if (!(test)) { printf("FAIL\n"); return name;} \
                    else printf("OK\n"); \
                } while (0)
#define ut_run_test(name, test) do { printf("Running %s...\n", name); \
                                const char *message = test(); tests_run++; \
                                if (message != NULL) { if (skip_failed_pkg) \
                                tests_fail++; else  return message; } } while (0)
