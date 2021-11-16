/*
**  
*  简单文件日志服务器
*/

#include "CLeafLog.h"
#include "CServerFrame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>

extern CServerFrame g_ServerFrame;
const char* syslogLevelMap[LOG_MAX] = { "DEBUG", "INFO", "NOTICE", "WARN" };
void LOG_SVR(int level, const char *func, int line, const char *fmt, ...) 
{
    va_list ap;
    char msg[LEAF_MAX_LOGMSG_LEN];

    if ((level&0xff) < g_ServerFrame.GetServerData()->verbosity) return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    LOG_SVR_RAW(level, func, line, msg);
}

void LOG_SVR_RAW(int level, const char *func, int line, const char *msg) 
{
	FILE *fp;
	char buf[64];
	int rawmode = (level & LOG_LOG_RAW);
	int log_to_stdout = g_ServerFrame.GetServerData()->logfile[0] == '\0';

	level &= 0xff; /* clear flags */
	if (level < g_ServerFrame.GetServerData()->verbosity) return;

	fp = log_to_stdout ? stdout : fopen(g_ServerFrame.GetServerData()->logfile, "a");
	if (!fp) return;

	if (rawmode) 
	{
		fprintf(fp, "%s", msg);
	} 
	else 
	{
		struct timeval tv;
		gettimeofday(&tv,NULL);
		strftime(buf, sizeof(buf),"%d %b %H:%M:%S", localtime(&tv.tv_sec));
		//snprintf(buf+off, sizeof(buf)-off, "%03d", (int)tv.tv_usec/1000);

		fprintf(fp,"[%d %s][%s] [(%s:%d)] %s\n", (int)getpid(), buf, syslogLevelMap[level], func, line, msg);
	}
	fflush(fp);

	if (!log_to_stdout) fclose(fp);
	//if (g_ServerFrame.GetServerData()->syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}