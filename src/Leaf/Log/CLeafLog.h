#ifndef CLEAFLOG_H__
#define CLEAFLOG_H__

void LOG_SVR(int level, const char *func, int line, const char *fmt, ...);
void LOG_SVR_RAW(int level, const char *func, int line, const char *msg);
char* stringcatfmt(char* s, char const *fmt, ...);

#define LOG_MSG(level, fmt, args...) \
do \
{\
	LOG_SVR(level, __FILE__, __LINE__, fmt, ##args);\
} while (0);

#endif