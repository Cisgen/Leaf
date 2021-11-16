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
	// 目前已注册的最大描述符
	int Maxfd;   /* highest file descriptor currently registered */

	// 目前已追踪的最大描述符
	int Setsize; /* max number of file descriptors tracked */

	// 用于生成时间事件 id
	long long timeEventNextId;

	// 最后一次执行时间事件的时间
	time_t lastTime;     /* Used to detect system clock skew */

	// 已注册的文件事件
	//ProcEvent* ProcEvents; /* Registered events */

	//// 已就绪的事件
	//ReadyEpollEvent* ReadyList; /* Fired events */

	// 时间事件
	//aeTimeEvent *timeEventHead;

	// 事件处理器的开关
	int Stop;
	
	//监听socket

	int ListenFd;

	// epoll_event 实例描述符
	int EpollFd;

	// 事件槽
	struct epoll_event* Epevents;
};

class CServerNet
{
public:
	CServerNet();
	~CServerNet();

	//创建epoll 实例资源
	int CreateEventLoop(int iSetsize);

	//回收epoll 资源
	void DeleteEventLoop();

	int ProcessEvents(int iFlag);

	//对epoll操作
	int ApiPollEvent(struct timeval *tvp = NULL);
	void ApiDelEvent(int iFd, int iDelmask);
	int ApiAddEvent(int iFd, int iMask);

	//监听端口
	int ListenToPort(int iPort, int* pFds, int* iCount, int iBacklog);

	//设置为非阻塞
	static int SetNonBlock(int iFd);
	static int SetReuseAddr(int iFd);
	static int SetEnableTcpNoDelay(int iFd);
	static int ListenAddr(int iSocket, struct sockaddr *sa, socklen_t len, int iBacklog);

	//创建TCP监听
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