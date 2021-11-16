// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "CServerFrame.h"
CServerFrame g_ServerFrame;
int main(int argc, char **argv)
{
	// ��ʼ������������
    g_ServerFrame.InitServerConfig();

	// ������������Ϊ�ػ�����
	bool bDaemonize = false;
	if (bDaemonize) 
	{
		 g_ServerFrame.Daemonize();
	}

	// ��������ʼ�����������ݽṹ
	if(0 != g_ServerFrame.InitServerData())
	{
		g_ServerFrame.DeleteServerRes();
		return -1;
	}

	// Ϊ������������������
	g_ServerFrame.SetProcTitle(argv[0]);

	// �����¼���������һֱ���������ر�Ϊֹ
	g_ServerFrame.SetBeforeSleepProc();

	g_ServerFrame.MainLoop();

	// �������رգ�ֹͣ�¼�ѭ��
	g_ServerFrame.DeleteServerRes();

	return 0;
}

