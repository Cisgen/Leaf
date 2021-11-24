// Server.cpp : �������̨Ӧ�ó������ڵ㡣
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
	// ��ʼ������������
    g_ServerFrame.InitServerConfig();

	// ������������Ϊ�ػ�����
	bool bDaemonize = true;
	if (bDaemonize) 
	{
		 g_ServerFrame.Daemonize();
	}

	//��ֹ֮ͣǰ�Ľ���
	g_ServerFrame.KillProcess();

	// ��������ʼ�����������ݽṹ
	if(0 != g_ServerFrame.InitServerData())
	{
		g_ServerFrame.DeleteServerRes();
		return -1;
	}

	// ������������ػ����̣���ô���� PID �ļ�
	if (bDaemonize) 
	{
		g_ServerFrame.CreatePidFile();
	}

	// Ϊ������������������
	g_ServerFrame.SetProcTitle(argv[0]);

	// �����¼���������һֱ���������ر�Ϊֹ
	g_ServerFrame.SetBeforeSleepProc();

	g_ServerFrame.MainLoop();

	// �������رգ�ֹͣ�¼�ѭ��
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

