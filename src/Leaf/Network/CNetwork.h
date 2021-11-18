#ifndef CNETWORK_H__
#define CNETWORK_H__

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <string.h>

struct CServerEpollData 
{
	// Ŀǰ��ע������������
	int Maxfd;   /* highest file descriptor currently registered */

	// Ŀǰ��׷�ٵ����������
	int Setsize; /* max number of file descriptors tracked */

	// ��������ʱ���¼� id
	long long timeEventNextId;

	// ���һ��ִ��ʱ���¼���ʱ��
	time_t lastTime;     /* Used to detect system clock skew */

	// ��ע����ļ��¼�
	//ProcEvent* ProcEvents; /* Registered events */

	//// �Ѿ������¼�
	//ReadyEpollEvent* ReadyList; /* Fired events */

	// ʱ���¼�
	//aeTimeEvent *timeEventHead;

	// �¼��������Ŀ���
	int Stop;
	
	//����socket

	int ListenFd;

	// epoll_event ʵ��������
	int EpollFd;

	// �¼���
	struct epoll_event* Epevents;
};

class CServerNet
{
public:
	CServerNet();
	~CServerNet();

	//����epoll ʵ����Դ
	int CreateEventLoop(int iSetsize);

	//����epoll ��Դ
	void DeleteEventLoop();

	int ProcessEvents(int iFlag);

	//��epoll����
	int ApiPollEvent(struct timeval *tvp = NULL);
	void ApiDelEvent(int iFd, int iDelmask);
	int ApiAddEvent(int iFd, int iMask);

	//�����˿�
	int ListenToPort(int iPort, int* pFds, int* iCount, int iBacklog);

	//����Ϊ������
	static int SetNonBlock(int iFd);
	static int SetReuseAddr(int iFd);
	static int SetEnableTcpNoDelay(int iFd);
	static int ListenAddr(int iSocket, struct sockaddr *sa, socklen_t len, int iBacklog);

	//����TCP����
	int CreateTcpEvent(int iFd, int iMask, bool bAccept);
	int ProcTcpAccept(int iFd, char* pClientip, size_t ip_len, int *pClientPort); 

	void FunBeforeSleepProc();
	void ProcWriteEvent(int iEventFd);
	void ProcReadEvent(int iEventFd);
	void ProcErrorEvent(int iEventFd);
	CServerEpollData* GetServerData() { return &m_EpollData; }
private:
	CServerEpollData m_EpollData;
};


#endif