// Server.cpp : 定义控制台应用程序的入口点。
//

#include "CServerFrame.h"
CServerFrame g_ServerFrame;
int main(int argc, char **argv)
{
	// 初始化服务器配置
    g_ServerFrame.InitServerConfig();

	// 将服务器设置为守护进程
	bool bDaemonize = false;
	if (bDaemonize) 
	{
		 g_ServerFrame.Daemonize();
	}

	// 创建并初始化服务器数据结构
	if(0 != g_ServerFrame.InitServerData())
	{
		g_ServerFrame.DeleteServerRes();
		return -1;
	}

	// 为服务器进程设置名字
	g_ServerFrame.SetProcTitle(argv[0]);

	// 运行事件处理器，一直到服务器关闭为止
	g_ServerFrame.SetBeforeSleepProc();

	g_ServerFrame.MainLoop();

	// 服务器关闭，停止事件循环
	g_ServerFrame.DeleteServerRes();

	return 0;
}

