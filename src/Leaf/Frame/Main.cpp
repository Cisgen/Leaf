// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "CServerFrame.h"
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

