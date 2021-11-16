/*
**  
*  服务器网路通讯
*/

#include "CNetwork.h"
#include "CServerError.h"
#include "CMacro.h"
#include <netdb.h>
#include "CLeafLog.h"
#include <netinet/tcp.h>
#include "CQueryFund.h"

CServerNet::CServerNet()
{
	m_EpollData.Epevents = NULL;
}

CServerNet::~CServerNet()
{
	if (m_EpollData.Epevents)
	{
		free(m_EpollData.Epevents);
		m_EpollData.Epevents = NULL;
	}
}

int CServerNet::CreateEventLoop(int iSetsize)
{
	// 设置数组大小
	m_EpollData.Setsize = iSetsize;

	// 初始化执行最近一次执行时间
	m_EpollData.lastTime = time(NULL);

	// 初始化时间事件结构
	m_EpollData.timeEventNextId = 0;

	m_EpollData.Stop = 0;
	m_EpollData.Maxfd = -1;

	// 创建 epoll 实例
	m_EpollData.EpollFd = epoll_create(1024);
	if (m_EpollData.EpollFd == -1) 
	{
		return SVR_NET_CREATE_EPOLL_ERR;
	}

	//申请事件的结构
	m_EpollData.Epevents = (struct epoll_event*)malloc(sizeof(struct epoll_event)*iSetsize);
	if (!m_EpollData.Epevents)
	{
		return SVR_NET_MALLOC_ERR;
	}

	// 初始化监听事件
	for (int i = 0; i < iSetsize; i++)
	{
		m_EpollData.Epevents[i].data.fd = 0;
		m_EpollData.Epevents[i].events = 0;
	}

	// 返回事件循环
	return SVR_OK;
}

int CServerNet::ListenToPort(int iPort, int* pFds, int* iCount, int iBacklog)
{
	char _port[6];
	struct addrinfo hints, *servinfo, *p;

	snprintf(_port, sizeof(_port), "%d", iPort);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;    /* No effect if bindaddr != NULL */

	if (getaddrinfo(NULL, _port, &hints, &servinfo) != 0) 
	{
		return SVR_NET_LISTEN_ERR;
	}

	//初始化socket
	int iNewSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (iNewSocket <= 0)
	{
		return SVR_NET_LISTEN_ERR;
	}

	//设置端口可复用
	int iRet = SetReuseAddr(iNewSocket);
	if (iRet != 0)
	{
		close(iNewSocket);
		return iRet;
	}

	//设置非阻塞
	iRet = SetNonBlock(iNewSocket);
	if (iRet != 0)
	{
		close(iNewSocket);
		return iRet;
	}

	struct sockaddr_in Serveraddr;
	memset(&Serveraddr, 0, sizeof(Serveraddr));
	Serveraddr.sin_family = AF_INET;
	//inet_aton("127.0.0.1", &(Serveraddr.sin_addr));
	Serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	Serveraddr.sin_port = htons(iPort);

	//绑定并且监听
	iRet = ListenAddr(iNewSocket, (sockaddr *)&Serveraddr, sizeof(sockaddr_in), iBacklog);
	if (iRet != 0)
	{
		close(iNewSocket);
		return iRet;
	}

	pFds[*iCount] = iNewSocket;
	(*iCount)++;
	m_EpollData.ListenFd = iNewSocket;
	return SVR_OK;
}

void CServerNet::DeleteEventLoop()
{
	if (m_EpollData.EpollFd > 0)
	{
		close(m_EpollData.EpollFd);
		if (m_EpollData.Epevents)
		{
			free(m_EpollData.Epevents);
		}
	}
}

void CServerNet::ProcWriteEvent(int iEventFd)
{
	LOG_MSG(LOG_DEBUG, "Write Event!!!");
}

void CServerNet::ProcReadEvent(int iEventFd)
{
	if (iEventFd == m_EpollData.ListenFd)
	{
		CreateTcpEvent(iEventFd, EPOLLIN|EPOLLET, true);
	}
	else
	{
		//connet
		//LOG_MSG(LOG_DEBUG, "Revive Connect!!! %d %d", iEventFd, m_EpollData.ListenFd);

		// 读入长度（默认为 16 MB）
		int iReadlen = LEAF_IOBUF_LEN;
		char querybuf[LEAF_IOBUF_LEN] = {0};
		int iRecvSize = read(iEventFd, querybuf, iReadlen);

		// 读入出错
		if (iRecvSize == -1) 
		{
			if (errno == EAGAIN) 
			{
				iRecvSize = 0;
			} else 
			{
				LOG_MSG(LOG_DEBUG, "Reading from client: %s", strerror(errno));
				return;
			}
			// 遇到 EOF
		} 
		else if (iRecvSize == 0) 
		{
			LOG_MSG(LOG_DEBUG, "Client closed connection");
			return;
		}

		if (iRecvSize) 
		{
			LOG_MSG(LOG_DEBUG, "fd: %d read:%s", iEventFd, querybuf);

			//发送一个回包
			char sendbuff[LEAF_IOBUF_LEN] = "++OK\n";
			int iSendSize = send(iEventFd, sendbuff, sizeof(sendbuff), 0);
			FundTest(iEventFd);
		} 
		else 
		{
			return;
		}
	}
}

void CServerNet::ProcErrorEvent(int iEventFd)
{
	LOG_MSG(LOG_DEBUG, "Error Event!!!");
}

/*
 * 关联给定事件到 fd
 */
int CServerNet::ApiAddEvent(int iFd, int iMask) 
{
    struct epoll_event ee;

    // 注册事件到 epoll
    ee.events = iMask;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = iFd;

	LOG_MSG(LOG_DEBUG, "Create Event Sucess!!! %d", iFd);
    return epoll_ctl(m_EpollData.EpollFd, EPOLL_CTL_ADD, iFd, &ee);
}

/*
 * 从 fd 中删除给定事件
 */
void CServerNet::ApiDelEvent(int iFd, int iDelmask)
{
    struct epoll_event ee;
    //int mask = eventLoop->events[fd].mask & (~delmask);

    ee.events = 0;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = iFd;
    if (iDelmask != 0) 
	{
        epoll_ctl(m_EpollData.EpollFd, EPOLL_CTL_MOD, iFd, &ee);
    } 
	else 
	{
        epoll_ctl(m_EpollData.EpollFd, EPOLL_CTL_DEL, iFd, &ee);
    }
}

/*
 * 获取可执行事件
 */
int CServerNet::ApiPollEvent(struct timeval *tvp) 
{
    int iPorcessCount = 0;

	// 调用epoll组件
    int iCount = epoll_wait(m_EpollData.EpollFd, m_EpollData.Epevents, m_EpollData.Setsize, 
		tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);

	// 为已就绪事件设置相应的模式
	// 并加入到 eventLoop 的 fired 数组中
	for (int i = 0; i < iCount; i++)
	{
		int rfired = 0; 
		struct epoll_event* e = &m_EpollData.Epevents[i];
		//ProcEvent* pProc = &pEpollData->ProcEvents[e->fd];
		if (e->events & EPOLLIN)
		{
			// 读事件
			// rfired 确保读/写事件只能执行其中一个
			rfired = 1;
			ProcReadEvent(e->data.fd);
		}
		else if (e->events & EPOLLOUT || e->events & EPOLLHUP)
		{
			// 写事件
			ProcWriteEvent(e->data.fd);
		}
		else if (e->events & EPOLLERR)
		{
			ProcErrorEvent(e->data.fd);
		}

		iPorcessCount++;
	}

    // 返回已就绪事件个数
    return iPorcessCount;
}

int CServerNet::SetNonBlock(int iFd)
{
	int iFlag;
    if ((iFlag = fcntl(iFd, F_GETFL)) < 0) 
	{
        fprintf(stderr, "fcntl(F_GETFL): %s", strerror(errno));
        return SVR_NOK;
    }

    if (fcntl(iFd, F_SETFL, iFlag | O_NONBLOCK) < 0) 
	{
        fprintf(stderr, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return SVR_NOK;
	}

	return SVR_OK;
}

// 设置地址为可重用
int CServerNet::SetReuseAddr(int iFd) 
{
    int yes = 1;
    /* Make sure connection-intensive things like the redis benckmark
     * will be able to close/open sockets a zillion of times */
    if (setsockopt(iFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
	{
        fprintf(stderr, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return SVR_NOK;
    }

    return SVR_OK;
}


int CServerNet::SetEnableTcpNoDelay(int iFd)
{
	int ival;
	if (setsockopt(iFd, IPPROTO_TCP, TCP_NODELAY, &ival, sizeof(ival)) == -1)
	{
		return SVR_NOK;
	}
	return SVR_OK;
}

int CServerNet::ListenAddr(int iSocket, struct sockaddr *sa, socklen_t len, int iBacklog)
{
	if (bind(iSocket, sa, len) < 0) 
	{
		close(iSocket);
		return SVR_NET_BIND_PORT_ERR;;
	}

	if (listen(iSocket, iBacklog) < 0) 
	{
		close(iSocket);
		return SVR_NET_LISTEN_ERR;
	}

	return SVR_OK;
}

int CServerNet::CreateTcpEvent(int iFd, int iMask, bool bAccept)
{
	if ((m_EpollData.Maxfd > 0 && iFd > m_EpollData.Maxfd) || iFd >= m_EpollData.Setsize) 
	{
		LOG_MSG(LOG_DEBUG, "Fd is too much! %d %d %d", iFd, m_EpollData.Maxfd, m_EpollData.Setsize);
		return SVR_NET_MAX_SIZE_ERR;
	}

	//增加tcp 客户端连接
	if (bAccept)
	{
		int iClientPort, iClientFd, iMax = 126;
		char cip[LEAF_IP_STR_LEN];

		while(iMax--) 
		{
			// accept 客户端连接
			iClientFd = ProcTcpAccept(iFd, cip, sizeof(cip), &iClientPort);
			if (iClientFd <= 0)
			{
				if (errno != EWOULDBLOCK)
				{
					LOG_MSG(LOG_WARNING,"Accepting client connection Failed");
				}

				break;
			}
			else
			{
				// 监听指定 fd 的指定事件
				iFd = iClientFd;
				LOG_MSG(LOG_DEBUG, "Fd:%d  Client [%s] Port [%d]", iClientFd, cip, iClientPort);
				if (ApiAddEvent(iClientFd, iMask) == -1)
				{
					return SVR_NET_EPOLL_ADD_ERR;
				}
			}
		}
	}
	else
	{
		// 监听指定 fd 的指定事件
		if (ApiAddEvent(iFd, iMask) == -1)
		{
			return SVR_NET_EPOLL_ADD_ERR;
		}
	}
	
	// 如果有需要，更新事件处理器的最大 fd
	if (iFd > m_EpollData.Maxfd)
	{
		m_EpollData.Maxfd = iFd;
	}
		
	return SVR_OK;
}

/*
 * TCP 连接 accept 函数
 */
int CServerNet::ProcTcpAccept(int iFd, char* pClientip, size_t ip_len, int *pClientPort) 
{
    int iClientFd = 0;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

	while(iClientFd <= 0) 
	{
		iClientFd = accept(m_EpollData.ListenFd, (struct sockaddr*)&sa, &salen);
		if (iClientFd == -1) 
		{
			if (errno == EINTR)
				continue;
			else 
			{
				LOG_MSG(LOG_WARNING, "accept: %s %d %d", strerror(errno), m_EpollData.ListenFd, errno);
				return SVR_NOK;
			}
		}
	}

	// 非阻塞
	SetNonBlock(iClientFd);

	// 禁用 Nagle 算法
	SetEnableTcpNoDelay(iClientFd);

	//// 设置 keep alive
	//if (server.tcpkeepalive)
	//	anetKeepAlive(NULL,fd,server.tcpkeepalive);

	struct sockaddr_in *s = (struct sockaddr_in *)&sa;
	if (pClientip)
	{
		inet_ntop(AF_INET, (void*)&(s->sin_addr), pClientip, ip_len);
	}

	if (pClientPort)
	{
		*pClientPort = ntohs(s->sin_port);
	}

    return iClientFd;
}

void CServerNet::FunBeforeSleepProc()
{

}

int CServerNet::ProcessEvents(int iFlag)
{
	int iProcessed = 0;

    if (!(iFlag & TIME_EVENTS) && !(iFlag & FILE_EVENTS)) return 0;

    /* Note that we want call select() even if there are no
     * file events to process as long as we want to process time
     * events, in order to sleep until the next time event is ready
     * to fire. */
    if (m_EpollData.Maxfd != -1 || ((iFlag & TIME_EVENTS) && !(iFlag & DONT_WAIT))) 
	{
        // 处理文件事件，阻塞时间由 tvp 决定
        iProcessed = ApiPollEvent();
    }

    /* Check time events */
    // 执行时间事件
	/* if (flags & AE_TIME_EVENTS)
	iProcessed += processTimeEvents(eventLoop);*/

    return iProcessed; /* return the number of processed file/time events */
}
