/*
**  
*  ��������·ͨѶ
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
	// ���������С
	m_EpollData.Setsize = iSetsize;

	// ��ʼ��ִ�����һ��ִ��ʱ��
	m_EpollData.lastTime = time(NULL);

	// ��ʼ��ʱ���¼��ṹ
	m_EpollData.timeEventNextId = 0;

	m_EpollData.Stop = 0;
	m_EpollData.Maxfd = -1;

	// ���� epoll ʵ��
	m_EpollData.EpollFd = epoll_create(1024);
	if (m_EpollData.EpollFd == -1) 
	{
		return SVR_NET_CREATE_EPOLL_ERR;
	}

	//�����¼��Ľṹ
	m_EpollData.Epevents = (struct epoll_event*)malloc(sizeof(struct epoll_event)*iSetsize);
	if (!m_EpollData.Epevents)
	{
		return SVR_NET_MALLOC_ERR;
	}

	// ��ʼ�������¼�
	for (int i = 0; i < iSetsize; i++)
	{
		m_EpollData.Epevents[i].data.fd = 0;
		m_EpollData.Epevents[i].events = 0;
	}

	// �����¼�ѭ��
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

	//��ʼ��socket
	int iNewSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (iNewSocket <= 0)
	{
		return SVR_NET_LISTEN_ERR;
	}

	//���ö˿ڿɸ���
	int iRet = SetReuseAddr(iNewSocket);
	if (iRet != 0)
	{
		close(iNewSocket);
		return iRet;
	}

	//���÷�����
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

	//�󶨲��Ҽ���
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

		// ���볤�ȣ�Ĭ��Ϊ 16 MB��
		int iReadlen = LEAF_IOBUF_LEN;
		char querybuf[LEAF_IOBUF_LEN] = {0};
		int iRecvSize = read(iEventFd, querybuf, iReadlen);

		// �������
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
			// ���� EOF
		} 
		else if (iRecvSize == 0) 
		{
			LOG_MSG(LOG_DEBUG, "Client closed connection");
			return;
		}

		if (iRecvSize) 
		{
			LOG_MSG(LOG_DEBUG, "fd: %d read:%s", iEventFd, querybuf);

			//����һ���ذ�
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
 * ���������¼��� fd
 */
int CServerNet::ApiAddEvent(int iFd, int iMask) 
{
    struct epoll_event ee;

    // ע���¼��� epoll
    ee.events = iMask;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = iFd;

	LOG_MSG(LOG_DEBUG, "Create Event Sucess!!! %d", iFd);
    return epoll_ctl(m_EpollData.EpollFd, EPOLL_CTL_ADD, iFd, &ee);
}

/*
 * �� fd ��ɾ�������¼�
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
 * ��ȡ��ִ���¼�
 */
int CServerNet::ApiPollEvent(struct timeval *tvp) 
{
    int iPorcessCount = 0;

	// ����epoll���
    int iCount = epoll_wait(m_EpollData.EpollFd, m_EpollData.Epevents, m_EpollData.Setsize, 
		tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);

	// Ϊ�Ѿ����¼�������Ӧ��ģʽ
	// �����뵽 eventLoop �� fired ������
	for (int i = 0; i < iCount; i++)
	{
		int rfired = 0; 
		struct epoll_event* e = &m_EpollData.Epevents[i];
		//ProcEvent* pProc = &pEpollData->ProcEvents[e->fd];
		if (e->events & EPOLLIN)
		{
			// ���¼�
			// rfired ȷ����/д�¼�ֻ��ִ������һ��
			rfired = 1;
			ProcReadEvent(e->data.fd);
		}
		else if (e->events & EPOLLOUT || e->events & EPOLLHUP)
		{
			// д�¼�
			ProcWriteEvent(e->data.fd);
		}
		else if (e->events & EPOLLERR)
		{
			ProcErrorEvent(e->data.fd);
		}

		iPorcessCount++;
	}

    // �����Ѿ����¼�����
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

// ���õ�ַΪ������
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

	//����tcp �ͻ�������
	if (bAccept)
	{
		int iClientPort, iClientFd, iMax = 126;
		char cip[LEAF_IP_STR_LEN];

		while(iMax--) 
		{
			// accept �ͻ�������
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
				// ����ָ�� fd ��ָ���¼�
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
		// ����ָ�� fd ��ָ���¼�
		if (ApiAddEvent(iFd, iMask) == -1)
		{
			return SVR_NET_EPOLL_ADD_ERR;
		}
	}
	
	// �������Ҫ�������¼������������ fd
	if (iFd > m_EpollData.Maxfd)
	{
		m_EpollData.Maxfd = iFd;
	}
		
	return SVR_OK;
}

/*
 * TCP ���� accept ����
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

	// ������
	SetNonBlock(iClientFd);

	// ���� Nagle �㷨
	SetEnableTcpNoDelay(iClientFd);

	//// ���� keep alive
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
        // �����ļ��¼�������ʱ���� tvp ����
        iProcessed = ApiPollEvent();
    }

    /* Check time events */
    // ִ��ʱ���¼�
	/* if (flags & AE_TIME_EVENTS)
	iProcessed += processTimeEvents(eventLoop);*/

    return iProcessed; /* return the number of processed file/time events */
}
