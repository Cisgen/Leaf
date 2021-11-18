/*
**  
*  简单服务器逻辑框架
*/

#include "CServerFrame.h"
#include "CServerError.h"
#include "CNetwork.h"
#include "CLeafLog.h"

extern CServerFrame g_ServerFrame;
void SigHandle(int);
CServerFrame::CServerFrame()
{
	m_ServerData.EpollData = NULL;
	m_ServerData.Shutdown = 0;
}

CServerFrame::~CServerFrame()
{
	DeleteServerRes();
}

//初始化配置
void CServerFrame::InitServerConfig()
{
	int j;

    // 服务器状态

    // 设置服务器的运行 ID
    //getRandomHexChars(server.runid,LEAF_RUN_ID_SIZE);
    // 设置默认配置文件路径
    m_ServerData.Configfile = NULL;
    // 设置默认服务器频率
    m_ServerData.hz = LEAF_DEFAULT_HZ;
    // 为运行 ID 加上结尾字符
    m_ServerData.runid[LEAF_RUN_ID_SIZE] = '\0';
    // 设置服务器的运行架构
    m_ServerData.arch_bits = (sizeof(long) == 8) ? 64 : 32;
    // 设置默认服务器端口号
    m_ServerData.port = LEAF_SERVERPORT;
    m_ServerData.tcp_backlog = LEAF_TCP_BACKLOG;
    m_ServerData.bindaddr_count = 0;
    m_ServerData.unixsocket = NULL;
    m_ServerData.unixsocketperm = LEAF_DEFAULT_UNIX_SOCKET_PERM;
    m_ServerData.ipfd_count = 0;
    m_ServerData.sofd = -1;
    m_ServerData.dbnum = LEAF_DEFAULT_DBNUM;
    m_ServerData.verbosity = LOG_DEBUG;
	m_ServerData.maxidletime = LEAF_MAXIDLETIME;
	m_ServerData.tcpkeepalive = LEAF_DEFAULT_TCP_KEEPALIVE;
	m_ServerData.active_expire_enabled = 1;
	m_ServerData.client_max_querybuf_len = LEAF_MAX_QUERYBUF_LEN;
	m_ServerData.saveparams = NULL;
	m_ServerData.loading = 0;
	m_ServerData.logfile = LEAF_DEFAULT_LOGFILE;
	m_ServerData.syslog_enabled = LEAF_DEFAULT_SYSLOG_ENABLED;
	m_ServerData.syslog_ident = LEAF_DEFAULT_SYSLOG_IDENT;
	m_ServerData.daemonize = LEAF_DEFAULT_DAEMONIZE;
	m_ServerData.pidfile = LEAF_DEFAULT_PID_FILE;

	m_ServerData.requirepass = NULL;
	m_ServerData.rdb_compression = LEAF_DEFAULT_RDB_COMPRESSION;
	m_ServerData.rdb_checksum = LEAF_DEFAULT_RDB_CHECKSUM;
	m_ServerData.stop_writes_on_bgsave_err = LEAF_DEFAULT_STOP_WRITES_ON_BGSAVE_ERROR;
	m_ServerData.activerehashing = LEAF_DEFAULT_ACTIVE_REHASHING;
	m_ServerData.notify_keyspace_events = 0;
	m_ServerData.maxclients = LEAF_MAX_CLIENTS;
	m_ServerData.bpop_blocked_clients = 0;
	m_ServerData.maxmemory = LEAF_DEFAULT_MAXMEMORY;
    m_ServerData.maxmemory_samples = LEAF_DEFAULT_MAXMEMORY_SAMPLES;
}

void CServerFrame::Daemonize()
{
	int fd;

    if (fork() != 0) exit(0); /* parent exits */
    setsid(); /* create a new session */

	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	//signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

    /* Every output goes to /dev/null. If it is daemonized but
     * the 'logfile' is set to 'stdout' in the configuration file
     * it will not log at all. */
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) 
	{
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) close(fd);
    }

	/*if( chdir(a_pszRoot) )exit(2);
	umask(0);*/
}


void CServerFrame::CreatePidFile()
{
	FILE *fp = fopen(m_ServerData.pidfile, "w");
	if (fp) 
	{
		fprintf(fp, "%d\n",(int)getpid());
		fclose(fp);
	}
}

int CServerFrame::KillProcess()
{
	LOG_MSG(LOG_DEBUG, "kill Process!!!");
	int iPid = -1;
	FILE *fp = fopen(m_ServerData.pidfile, "r");
	if (fp) 
	{
		fscanf(fp, "%d", &iPid);
		fclose(fp);
		if (iPid > 0)
		{
			if(kill(iPid, SIGTERM) > 0)
			{
				if(ESRCH == errno)
				{
					return 0;
				}
				else
				{
					LOG_MSG(LOG_DEBUG, "KillProcess Sucess!! PID[%d] [%s]", iPid, strerror(errno));
					return -1;	
				}
			}
			else
			{
				LOG_MSG(LOG_DEBUG, "KillProcess Sucess!! PID[%d]", iPid);
			}
		}
		
	}

	return 0;
}

void SigHandle(int iSigID)
{
	LOG_MSG(LOG_DEBUG, "received signal %d\n", iSigID);

	if(iSigID == SIGTERM)
	{
		//停止进程
		g_ServerFrame.GetServerData()->Shutdown = 1;
	}	
	else if(iSigID == SIGUSR2)
	{   
		//重载进程配置
		return;
	}   
}

int CServerFrame::InitServerData()
{
	// 设置信号处理函数
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	//注册一下个人信号
	if(signal(SIGUSR1, SigHandle) == SIG_ERR)
	{
		printf("can not catch SIGUSR1\n");
	}

	if(signal(SIGUSR2, SigHandle) == SIG_ERR)
	{   
		printf("can not catch SIGUSR2\n");
	}   

	if (signal(SIGTERM, SigHandle) == SIG_ERR)
	{
		printf("can not catch SIGTERM\n");
	}

	//申请事件的结构
	CServerNet* pServerNet = (CServerNet*)malloc(sizeof(CServerNet));
	if (!pServerNet)
	{
		LOG_MSG(LOG_WARNING, "Malloc CServerNet Failed");
		return SVR_NET_MALLOC_ERR;
	}

	//创建网络epoll实例
	m_ServerData.EpollData = pServerNet;
	if (pServerNet->CreateEventLoop(256) != SVR_OK)
	{
		return SVR_NET_MALLOC_ERR;
	}

	// 打开 TCP 监听端口，用于等待客户端的命令请求
	if (pServerNet->ListenToPort(m_ServerData.port, m_ServerData.ipfd, &m_ServerData.ipfd_count, m_ServerData.tcp_backlog) != 0)
	{
		return SVR_NET_PORT_ERR;
	}

    // 为 TCP 连接关联连接应答（accept）处理器
    // 用于接受并应答客户端的 connect() 调用
	for (int i = 0; i < m_ServerData.ipfd_count; i++)
	{
		if (pServerNet->CreateTcpEvent(m_ServerData.ipfd[i], EPOLLIN | EPOLLET, false) != 0)
		{
			LOG_MSG(LOG_WARNING, "CreateTcpEvent Failed!!! %d", m_ServerData.ipfd[i]);
			return SVR_NET_PORT_ERR;
		}
	}

	return 0;
}

void CServerFrame::SetProcTitle(char *title)
{
//#ifdef USE_SETPROCTITLE
//	char *server_mode = "";
//	if (server.cluster_enabled) server_mode = " [cluster]";
//	else if (server.sentinel_mode) server_mode = " [sentinel]";
//
//	setproctitle("%s %s:%d%s",
//		title,
//		server.bindaddr_count ? server.bindaddr[0] : "*",
//		server.port,
//		server_mode);
//#else
//	LEAF_NOTUSED(title);
//#endif
}

void CServerFrame::SetBeforeSleepProc()
{

}

void CServerFrame::MainLoop()
{
	CServerNet* pEpollData = m_ServerData.EpollData;
	if(pEpollData)
	{
		//处理事件
		m_ServerData.Shutdown = 0;
		while (!m_ServerData.Shutdown)
		{
			// 如果有需要在事件处理前执行的函数，那么运行它
			pEpollData->FunBeforeSleepProc();

			// 开始处理事件	
			pEpollData->ProcessEvents(ALL_EVENTS);
		}

		// 服务器进程收到 SIGTERM 信号，关闭服务器
		// 尝试关闭服务器
		// 关闭监听套接字，这样在重启的时候会快一点
		for (int j = 0; j < m_ServerData.ipfd_count; j++)
		{
			close(m_ServerData.ipfd[j]);
		}

		exit(0);
	}
}

/*
 * 释放实例资源
 */
void CServerFrame::DeleteServerRes()
{
	if (m_ServerData.EpollData)
	{
		m_ServerData.EpollData->DeleteEventLoop();
		free(m_ServerData.EpollData);
		m_ServerData.EpollData = NULL;
	}
}
