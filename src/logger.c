#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "logger.h"

int create_logfile(char *logfile, char *heading) {
    FILE *f = fopen(logfile, "w");

    if (!f) {
        fprintf(stderr, "ERROR: Could not open logfile: %s\n", logfile);
        return 1;
    }

    time_t now = time(NULL);
    char *date = ctime(&now);

    fprintf(f, "%s\t%s\n", logfile, date);

    if (heading) {
        fprintf(f, "%s\n", heading);
    }

    fclose(f);

    return 0;
}

int log_message(char *logfile, char *message, ...) {
    va_list args;
    FILE *f = fopen(logfile, "a");

    if (!f) {
        fprintf(stderr, "ERROR: Could not open logfile: %s\n", logfile);
        return 1;
    }

    va_start(args, message);
    vfprintf(f, message, args);
    va_end(args);

    fclose(f);

    return 0;
}

int log_error(char *logfile, int out_to_stderr, char *message, ...) {
    va_list  args;
    FILE *f = fopen(logfile, "a");

    if (!f) {
        fprintf(stderr, "ERROR: Could not open logfile: %s\n", logfile);
        return 1;
    }

    va_start(args, message);
    fprintf(f, "ERROR: ");
    vfprintf(f, message, args);
    va_end(args);

    if (out_to_stderr) {
        va_start(args, message);
        fprintf(stderr, "ERROR: ");
        vfprintf(stderr, message, args);
        va_end(args);
    }

    fclose(f);

    return 0;
}
