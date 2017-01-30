#ifndef LOGGER_H_20160118
#define LOGGER_H_20160118

int create_logfile(char *logfile, char *heading);
int log_message(char *logfile, char *message, ...);
int log_error(char *logfile, int out_to_stderr, char *message, ...);

#endif //LOGGER_H_20160118
