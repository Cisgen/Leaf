#ifndef CSERVERFRAME_H__
#define CSERVERFRAME_H__

#include "CMacro.h"
#include "CServerMeta.h"

class CServerFrame
{
public:

	CServerFrame();
	~CServerFrame();

	//��ʼ������
	void InitServerConfig();

	// ������������Ϊ�ػ�����
	void Daemonize();

	//����PID�ļ�
	void CreatePidFile();

	//ֹ֮ͣǰ�Ľ���
	int KillProcess();

	//��������ʼ�����������ݽṹ
	int InitServerData();

	// Ϊ������������������
	void SetProcTitle(char *title);

	void SetBeforeSleepProc();

	void DeleteServerRes();
	void MainLoop();
	CServerData* GetServerData() { return &m_ServerData; }

private:
	CServerData m_ServerData;
};

#endif

