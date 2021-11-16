/*
**  
*  �򵥷������߼����
*/

#include "CServerFrame.h"
#include "CServerError.h"
#include "CNetwork.h"
#include "CLeafLog.h"

CServerFrame::CServerFrame()
{
	m_ServerData.EpollData = NULL;
}

CServerFrame::~CServerFrame()
{
	DeleteServerRes();
}

//��ʼ������
void CServerFrame::InitServerConfig()
{
	int j;

    // ������״̬

    // ���÷����������� ID
    //getRandomHexChars(server.runid,LEAF_RUN_ID_SIZE);
    // ����Ĭ�������ļ�·��
    m_ServerData.Configfile = NULL;
    // ����Ĭ�Ϸ�����Ƶ��
    m_ServerData.hz = LEAF_DEFAULT_HZ;
    // Ϊ���� ID ���Ͻ�β�ַ�
    m_ServerData.runid[LEAF_RUN_ID_SIZE] = '\0';
    // ���÷����������мܹ�
    m_ServerData.arch_bits = (sizeof(long) == 8) ? 64 : 32;
    // ����Ĭ�Ϸ������˿ں�
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

	/*signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);*/

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

int CServerFrame::InitServerData()
{
	// �����źŴ�����
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);

	//�����¼��Ľṹ
	CServerNet* pServerNet = (CServerNet*)malloc(sizeof(CServerNet));
	if (!pServerNet)
	{
		LOG_MSG(LOG_WARNING, "Malloc CServerNet Failed");
		return SVR_NET_MALLOC_ERR;
	}

	//��������epollʵ��
	m_ServerData.EpollData = pServerNet;
	if (pServerNet->CreateEventLoop(256) != SVR_OK)
	{
		return SVR_NET_MALLOC_ERR;
	}

	// �� TCP �����˿ڣ����ڵȴ��ͻ��˵���������
	if (pServerNet->ListenToPort(m_ServerData.port, m_ServerData.ipfd, &m_ServerData.ipfd_count, m_ServerData.tcp_backlog) != 0)
	{
		return SVR_NET_PORT_ERR;
	}

    // Ϊ TCP ���ӹ�������Ӧ��accept��������
    // ���ڽ��ܲ�Ӧ��ͻ��˵� connect() ����
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
		//�����¼�
		CServerEpollData* pData = pEpollData->GetServerData();
		if (pData)
		{
			pData->Stop = 0;
			while (!pData->Stop)
			{
				// �������Ҫ���¼�����ǰִ�еĺ�������ô������
				pEpollData->FunBeforeSleepProc();

				// ��ʼ�����¼�	
				pEpollData->ProcessEvents(ALL_EVENTS);
			}
		}
	}
}

/*
 * �ͷ�ʵ����Դ
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
