// Server.cpp : 定义控制台应用程序的入口点。
//

#include "CServerFrame.h"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "CLeafLog.h"

#define BT_BUF_SIZE 100
int nptrs;
void *buffer[BT_BUF_SIZE];

extern "C"
{
	void* __real_malloc(size_t size);
	void* __wrap_malloc(size_t size)
	{

		char **strings;
		int nptrs = backtrace(buffer, BT_BUF_SIZE);
		strings = backtrace_symbols(buffer, nptrs);
		if (strings) 
		{
			for (int j = 0; j < nptrs; j++)
				LOG_MSG(LOG_DEBUG, "__wrap_malloc %s", strings[j]);
		}
		free(strings);
		void* p = __real_malloc(size);
		LOG_MSG(LOG_DEBUG, "Size  %d Addr %p", size, p);
		return p;
	}
};

CServerFrame g_ServerFrame;
int main(int argc, char **argv)
{
	// 初始化服务器配置
    g_ServerFrame.InitServerConfig();

	// 将服务器设置为守护进程
	bool bDaemonize = true;
	if (bDaemonize) 
	{
		 g_ServerFrame.Daemonize();
	}

	//先停止之前的进程
	g_ServerFrame.KillProcess();

	// 创建并初始化服务器数据结构
	if(0 != g_ServerFrame.InitServerData())
	{
		g_ServerFrame.DeleteServerRes();
		return -1;
	}

	// 如果服务器是守护进程，那么创建 PID 文件
	if (bDaemonize) 
	{
		g_ServerFrame.CreatePidFile();
	}

	// 为服务器进程设置名字
	g_ServerFrame.SetProcTitle(argv[0]);

	// 运行事件处理器，一直到服务器关闭为止
	g_ServerFrame.SetBeforeSleepProc();

	g_ServerFrame.MainLoop();

	// 服务器关闭，停止事件循环
	g_ServerFrame.DeleteServerRes();

	///* process start/stop/refresh */
	//for( opt=optind; opt<pstCtx->argc; opt++ )
	//{
	//	if( 0==stricmp(pstCtx->argv[opt], "stop") )
	//	{
	//		iRet = tos_send_signal( pstCtx->pszPidFile, pszApp, SIGUSR1, NULL );
	//		exit( iRet );
	//	}
	//	else if( 0==stricmp(pstCtx->argv[opt], "reload") )
	//	{
	//		iRet	=	tos_send_signal( pstCtx->pszPidFile, pszApp, SIGUSR2, NULL );
	//		exit( iRet );
	//	}
	//	else if( 0==stricmp(pstCtx->argv[opt], "kill") )
	//	{
	//		iRet	=	tos_kill_prev( pstCtx->pszPidFile, pszApp, pstCtx->iWait);
	//		exit( iRet );
	//	}
	//	else if( 0==stricmp(pstCtx->argv[opt], "start") )
	//	{
	//		iIsStart	=	1;
	//		break;
	//	}
	//}

	return 0;
}

