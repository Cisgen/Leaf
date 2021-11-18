#ifndef CSERVERFRAME_H__
#define CSERVERFRAME_H__

#include "CMacro.h"
#include "CServerMeta.h"

class CServerFrame
{
public:

	CServerFrame();
	~CServerFrame();

	//初始化配置
	void InitServerConfig();

	// 将服务器设置为守护进程
	void Daemonize();

	//创建PID文件
	void CreatePidFile();

	//停止之前的进程
	int KillProcess();

	//创建并初始化服务器数据结构
	int InitServerData();

	// 为服务器进程设置名字
	void SetProcTitle(char *title);

	void SetBeforeSleepProc();

	void DeleteServerRes();
	void MainLoop();
	CServerData* GetServerData() { return &m_ServerData; }

private:
	CServerData m_ServerData;
};

#endif

