#ifndef CSERVERMETA_H__
#define CSERVERMETA_H__

#include <time.h>
#include <string.h>
typedef void FunBeforeSleepProc(struct ServerLoop *eventLoop);
typedef void aeFileProc(struct ServerEpollData* eventLoop, int fd, void* clientData, int mask);

typedef char list;
typedef long off_t;
class CServerNet;
struct CServerData 
{
    // �����ļ��ľ���·��
    char* Configfile;           /* Absolute config file path, or NULL */

    // serverCron() ÿ����õĴ���
    int hz;                     /* serverCron() calls frequency in hertz */

    // �¼�״̬
    CServerNet* EpollData;

    // �رշ������ı�ʶ
    int Shutdown;          /* SHUTDOWN needed ASAP */

    // ��ִ�� serverCron() ʱ���н���ʽ rehash
    int activerehashing;        /* Incremental rehash in serverCron() */

    // �Ƿ�����������
    char *requirepass;          /* Pass for AUTH command, or NULL */

    // PID �ļ�
    const char* pidfile;              /* PID file path */

    // �ܹ�����
    int arch_bits;              /* 32 or 64 depending on sizeof(long) */

    // serverCron() ���������д���������
    int cronloops;              /* Number of times the cron function run */

    // ���������� RUN ID
    char runid[LEAF_RUN_ID_SIZE+1];  /* ID always different at every exec. */

    // �������Ƿ������� SENTINEL ģʽ
    int sentinel_mode;          /* True if this instance is a Sentinel. */


    /* Networking */

    // TCP �����˿�
    int port;                   /* TCP listening port */

    int tcp_backlog;            /* TCP listen() backlog */

    // ��ַ
    char *bindaddr[LEAF_BINDADDR_MAX]; /* Addresses we should bind to */
    // ��ַ����
    int bindaddr_count;         /* Number of addresses in server.bindaddr[] */

    // UNIX �׽���
    char *unixsocket;           /* UNIX socket path */
    int unixsocketperm;      /* UNIX socket permission */

    // ������
    int ipfd[LEAF_BINDADDR_MAX]; /* TCP socket file descriptors */
    // ����������
    int ipfd_count;             /* Used slots in ipfd[] */

    // UNIX �׽����ļ�������
    int sofd;                   /* Unix socket file descriptor */

    int cfd[LEAF_BINDADDR_MAX];/* Cluster bus listening socket */
    int cfd_count;              /* Used slots in cfd[] */

    // һ���������������пͻ���״̬�ṹ
    list *clients;              /* List of active clients */
    // �������������д��رյĿͻ���
    list *clients_to_close;     /* Clients to close asynchronously */

    // �������������дӷ��������Լ����м�����
    list *slaves, *monitors;    /* List of slaves and MONITORs */

    // �������ĵ�ǰ�ͻ��ˣ������ڱ�������
   // redisClient *current_client; /* Current client, only used on crash report */

    int clients_paused;         /* True if clients are currently paused */
    //mstime_t clients_pause_end_time; /* Time when we undo clients_paused */

    // �������
    //char neterr[ANET_ERR_LEN];   /* Error buffer for anet.c */

    // MIGRATE ����
   // dict *migrate_cached_sockets;/* MIGRATE cached sockets */


    /* RDB / AOF loading information */

    // ���ֵΪ��ʱ����ʾ���������ڽ�������
    int loading;                /* We are loading data from disk if true */

    // ������������ݵĴ�С
    int loading_total_bytes;

    // ���������ݵĴ�С
    int loading_loaded_bytes;

    // ��ʼ���������ʱ��
    time_t loading_start_time;
    int loading_process_events_interval_bytes;

    /* Fast pointers to often looked up command */
    // ��������Ŀ������
    struct redisCommand *delCommand, *multiCommand, *lpushCommand, *lpopCommand,
                        *rpopCommand;


    /* Fields used only for stats */

    // ����������ʱ��
    time_t stat_starttime;          /* Server start time */

    // �Ѵ������������
    long long stat_numcommands;     /* Number of processed commands */

    // �������ӵ���������������
    long long stat_numconnections;  /* Number of connections received */

    // �ѹ��ڵļ�����
    long long stat_expiredkeys;     /* Number of expired keys */

    // ��Ϊ�����ڴ�����ͷŵĹ��ڼ�������
    long long stat_evictedkeys;     /* Number of evicted keys (maxmemory) */

    // �ɹ����Ҽ��Ĵ���
    long long stat_keyspace_hits;   /* Number of successful lookups of keys */

    // ���Ҽ�ʧ�ܵĴ���
    long long stat_keyspace_misses; /* Number of failed lookups of keys */

    // ��ʹ���ڴ��ֵ
    size_t stat_peak_memory;        /* Max used memory record */

    // ���һ��ִ�� fork() ʱ���ĵ�ʱ��
    long long stat_fork_time;       /* Time needed to perform latest fork() */

    // ��������Ϊ�ͻ�������������ܾ��ͻ������ӵĴ���
    long long stat_rejected_conn;   /* Clients rejected because of maxclients */

    // ִ�� full sync �Ĵ���
    long long stat_sync_full;       /* Number of full resyncs with slaves. */

    // PSYNC �ɹ�ִ�еĴ���
    long long stat_sync_partial_ok; /* Number of accepted PSYNC requests. */

    // PSYNC ִ��ʧ�ܵĴ���
    long long stat_sync_partial_err;/* Number of unaccepted PSYNC requests. */


    /* slowlog */

    // ��������������ѯ��־������
    list *slowlog;                  /* SLOWLOG list of commands */

    // ��һ������ѯ��־�� ID
    long long slowlog_entry_id;     /* SLOWLOG current entry ID */

    // ���������� slowlog-log-slower-than ѡ���ֵ
    long long slowlog_log_slower_than; /* SLOWLOG time limit (to get logged) */

    // ���������� slowlog-max-len ѡ���ֵ
    unsigned long slowlog_max_len;     /* SLOWLOG max number of items logged */
    size_t resident_set_size;       /* RSS sampled in serverCron(). */
    /* The following two are used to track instantaneous "load" in terms
     * of operations per second. */
    // ���һ�ν��г�����ʱ��
    long long ops_sec_last_sample_time; /* Timestamp of last sample (in ms) */
    // ���һ�γ���ʱ����������ִ�����������
    long long ops_sec_last_sample_ops;  /* numcommands in last sample */
    // �������
    long long ops_sec_samples[LEAF_OPS_SEC_SAMPLES];
    // �������������ڱ�����������������Ҫʱ���Ƶ� 0
    int ops_sec_idx;


    /* Configuration */

    // ��־�ɼ���
    int verbosity;                  /* Loglevel in redis.conf */

    // �ͻ�������תʱ��
    int maxidletime;                /* Client timeout in seconds */

    // �Ƿ��� SO_KEEPALIVE ѡ��
    int tcpkeepalive;               /* Set SO_KEEPALIVE if non-zero. */
    int active_expire_enabled;      /* Can be disabled for testing purposes. */
    size_t client_max_querybuf_len; /* Limit for client query buffer length */
    int dbnum;                      /* Total number of configured DBs */
    int daemonize;                  /* True if running as a daemon */
    // �ͻ��������������С����
    // �����Ԫ���� LEAF_CLIENT_LIMIT_NUM_CLASSES ��
    // ÿ������һ��ͻ��ˣ���ͨ���ӷ�������pubsub���������
    //clientBufferLimitsConfig client_obuf_limits[LEAF_CLIENT_LIMIT_NUM_CLASSES];


    /* AOF persistence */

    // AOF ״̬������/�ر�/��д��
    int aof_state;                  /* LEAF_AOF_(ON|OFF|WAIT_REWRITE) */

    // ��ʹ�õ� fsync ���ԣ�ÿ��д��/ÿ��/�Ӳ���
    int aof_fsync;                  /* Kind of fsync() policy */
  
    int aof_no_fsync_on_rewrite;    /* Don't fsync if a rewrite is in prog. */
    int aof_rewrite_perc;           /* Rewrite AOF if % growth is > M and... */
    off_t aof_rewrite_min_size;     /* the AOF file is at least N bytes. */

    // ���һ��ִ�� BGREWRITEAOF ʱ�� AOF �ļ��Ĵ�С
    off_t aof_rewrite_base_size;    /* AOF size on latest startup or rewrite. */

    // AOF �ļ��ĵ�ǰ�ֽڴ�С
    off_t aof_current_size;         /* AOF current size. */
    int aof_rewrite_scheduled;      /* Rewrite once BGSAVE terminates. */

    // ������� AOF ��д���ӽ��� ID
    //pid_t aof_child_pid;            /* PID if rewriting process */

    // AOF ��д�������������Ŷ�������
    list *aof_rewrite_buf_blocks;   /* Hold changes during an AOF rewrite. */

    // AOF ������
    //sds aof_buf;      /* AOF buffer, written before entering the event loop */

    // AOF �ļ���������
    int aof_fd;       /* File descriptor of currently selected AOF file */

    // AOF �ĵ�ǰĿ�����ݿ�
    int aof_selected_db; /* Currently selected DB in AOF */

    // �Ƴ� write ������ʱ��
    time_t aof_flush_postponed_start; /* UNIX time of postponed AOF flush */

    // ���һֱִ�� fsync ��ʱ��
    time_t aof_last_fsync;            /* UNIX time of last fsync() */
    time_t aof_rewrite_time_last;   /* Time used by last AOF rewrite run. */

    // AOF ��д�Ŀ�ʼʱ��
    time_t aof_rewrite_time_start;  /* Current AOF rewrite start time. */

    // ���һ��ִ�� BGREWRITEAOF �Ľ��
    int aof_lastbgrewrite_status;   /* LEAF_OK or LEAF_ERR */

    // ��¼ AOF �� write �������Ƴ��˶��ٴ�
    unsigned long aof_delayed_fsync;  /* delayed AOF fsync() counter */

    // ָʾ�Ƿ���Ҫÿд��һ���������ݣ�������ִ��һ�� fsync()
    int aof_rewrite_incremental_fsync;/* fsync incrementally while rewriting? */
    int aof_last_write_status;      /* LEAF_OK or LEAF_ERR */
    int aof_last_write_errno;       /* Valid if aof_last_write_status is ERR */
    /* RDB persistence */

    // �Դ��ϴ� SAVE ִ�����������ݿⱻ�޸ĵĴ���
    long long dirty;                /* Changes to DB from the last save */

    // BGSAVE ִ��ǰ�����ݿⱻ�޸Ĵ���
    long long dirty_before_bgsave;  /* Used to restore dirty on failed BGSAVE */

    // ����ִ�� BGSAVE ���ӽ��̵� ID
    // û��ִ�� BGSAVE ʱ����Ϊ -1
    pid_t rdb_child_pid;            /* PID of RDB saving child */
    struct saveparam *saveparams;   /* Save points array for RDB */
    int saveparamslen;              /* Number of saving points */
 
    int rdb_compression;            /* Use compression in RDB? */
    int rdb_checksum;               /* Use RDB checksum? */

    // ���һ����� SAVE ��ʱ��
    time_t lastsave;                /* Unix time of last successful save */

    // ���һ�γ���ִ�� BGSAVE ��ʱ��
    time_t lastbgsave_try;          /* Unix time of last attempted bgsave */

    // ���һ�� BGSAVE ִ�кķѵ�ʱ��
    time_t rdb_save_time_last;      /* Time used by last RDB save run. */

    // ���ݿ����һ�ο�ʼִ�� BGSAVE ��ʱ��
    time_t rdb_save_time_start;     /* Current RDB save start time. */

    // ���һ��ִ�� SAVE ��״̬
    int lastbgsave_status;          /* LEAF_OK or LEAF_ERR */
    int stop_writes_on_bgsave_err;  /* Don't allow writes if can't BGSAVE */


    /* Propagation of commands in AOF / replication */
    //redisOpArray also_propagate;    /* Additional command to propagate. */


    /* Logging */
    const char *logfile;                  /* Path of log file */
    int syslog_enabled;             /* Is syslog enabled? */
    const char *syslog_ident;             /* Syslog ident */
    int syslog_facility;            /* Syslog facility */


    /* Replication (master) */
    int slaveseldb;                 /* Last SELECTed DB in replication output */
    // ȫ�ָ���ƫ������һ���ۼ�ֵ��
    long long master_repl_offset;   /* Global replication offset */
    // ������������ PING ��Ƶ��
    int repl_ping_slave_period;     /* Master pings the slave every N seconds */

    // backlog ����
    char *repl_backlog;             /* Replication backlog for partial syncs */
    // backlog �ĳ���
    long long repl_backlog_size;    /* Backlog circular buffer size */
    // backlog �����ݵĳ���
    long long repl_backlog_histlen; /* Backlog actual data length */
    // backlog �ĵ�ǰ����
    long long repl_backlog_idx;     /* Backlog circular buffer current offset */
    // backlog �п��Ա���ԭ�ĵ�һ���ֽڵ�ƫ����
    long long repl_backlog_off;     /* Replication offset of first byte in the
                                       backlog buffer. */
    // backlog �Ĺ���ʱ��
    time_t repl_backlog_time_limit; /* Time without slaves after the backlog
                                       gets released. */

    // ������һ���дӷ�������ʱ��
    time_t repl_no_slaves_since;    /* We have no slaves since that time.
                                       Only valid if server.slaves len is 0. */

    // �Ƿ�����С�����ӷ�����д�빦��
    int repl_min_slaves_to_write;   /* Min number of slaves to write. */
    // ������С�����ӷ�����������ӳ�ֵ
    int repl_min_slaves_max_lag;    /* Max lag of <count> slaves to write. */
    // �ӳ����õĴӷ�����������
    int repl_good_slaves_count;     /* Number of slaves with lag <= max_lag. */


    /* Replication (slave) */
    // ������������֤����
    char *masterauth;               /* AUTH with this password with master */
    // ���������ĵ�ַ
    char *masterhost;               /* Hostname of master */
    // ���������Ķ˿�
    int masterport;                 /* Port of master */
    // ��ʱʱ��
    int repl_timeout;               /* Timeout after N seconds of master idle */
    // ������������Ӧ�Ŀͻ���
    //redisClient *master;     /* Client that is master for this slave */
    // �����������������PSYNC ʱʹ��
    //redisClient *cached_master; /* Cached master to be reused for PSYNC. */
    int repl_syncio_timeout; /* Timeout for synchronous I/O calls */
    // ���Ƶ�״̬���������Ǵӷ�����ʱʹ�ã�
    int repl_state;          /* Replication status if the instance is a slave */
    // RDB �ļ��Ĵ�С
    off_t repl_transfer_size; /* Size of RDB to read from master during sync. */
    // �Ѷ� RDB �ļ����ݵ��ֽ���
    off_t repl_transfer_read; /* Amount of RDB read from master during sync. */
    // ���һ��ִ�� fsync ʱ��ƫ����
    // ���� sync_file_range ����
    off_t repl_transfer_last_fsync_off; /* Offset when we fsync-ed last time. */
    // �����������׽���
    int repl_transfer_s;     /* Slave -> Master SYNC socket */
    // ���� RDB �ļ�����ʱ�ļ���������
    int repl_transfer_fd;    /* Slave -> Master SYNC temp file descriptor */
    // ���� RDB �ļ�����ʱ�ļ�����
    char *repl_transfer_tmpfile; /* Slave-> master SYNC temp file name */
    // ���һ�ζ��� RDB ���ݵ�ʱ��
    time_t repl_transfer_lastio; /* Unix time of the latest read, for timeout */
    int repl_serve_stale_data; /* Serve stale data when link is down? */
    // �Ƿ�ֻ���ӷ�������
    int repl_slave_ro;          /* Slave is read only? */
    // ���ӶϿ���ʱ��
    time_t repl_down_since; /* Unix time at which link with master went down */
    // �Ƿ�Ҫ�� SYNC ֮��ر� NODELAY ��
    int repl_disable_tcp_nodelay;   /* Disable TCP_NODELAY after SYNC? */
    // �ӷ��������ȼ�
    int slave_priority;             /* Reported in INFO and used by Sentinel. */
    // �����������ӷ���������ǰ���������� RUN ID
    char repl_master_runid[LEAF_RUN_ID_SIZE+1];  /* Master run id for PSYNC. */
    // ��ʼ��ƫ����
    long long repl_master_initial_offset;         /* Master PSYNC offset. */


    /* Replication script cache. */
    // ���ƽű�����
    // �ֵ�
    //dict *repl_scriptcache_dict;        /* SHA1 all slaves are aware of. */
    // FIFO ����
    list *repl_scriptcache_fifo;        /* First in, first out LRU eviction. */
    // ����Ĵ�С
    int repl_scriptcache_size;          /* Max number of elements. */

    /* Synchronous replication. */
    list *clients_waiting_acks;         /* Clients waiting in WAIT command. */
    int get_ack_from_slaves;            /* If true we send REPLCONF GETACK. */
    /* Limits */
    int maxclients;                 /* Max number of simultaneous clients */
    unsigned long long maxmemory;   /* Max number of memory bytes to use */
    int maxmemory_samples;          /* Pricision of random sampling */


    /* Blocked clients */
    unsigned int bpop_blocked_clients; /* Number of clients blocked by lists */
    list *unblocked_clients; /* list of clients to unblock before next loop */
    list *ready_keys;        /* List of readyList structures for BLPOP & co */


    /* Sort parameters - qsort_r() is only available under BSD so we
     * have to take this state global, in order to pass it to sortCompare() */
    int sort_desc;
    int sort_alpha;
    int sort_bypattern;
    int sort_store;


    /* Zip structure config, see redis.conf for more information  */
    size_t hash_max_ziplist_entries;
    size_t hash_max_ziplist_value;
    size_t list_max_ziplist_entries;
    size_t list_max_ziplist_value;
    size_t set_max_intset_entries;
    size_t zset_max_ziplist_entries;
    size_t zset_max_ziplist_value;
    size_t hll_sparse_max_bytes;
    time_t unixtime;        /* Unix time sampled every cron cycle. */
    long long mstime;       /* Like 'unixtime' but with milliseconds resolution. */


    /* Pubsub */
    // �ֵ䣬��ΪƵ����ֵΪ����
    // �����б��������ж���ĳ��Ƶ���Ŀͻ���
    // �¿ͻ������Ǳ���ӵ�����ı�β
    //dict *pubsub_channels;  /* Map channels to list of subscribed clients */

    // ��������¼�˿ͻ��˶��ĵ�����ģʽ������
    list *pubsub_patterns;  /* A list of pubsub_patterns */

    int notify_keyspace_events; /* Events to propagate via Pub/Sub. This is an
                                   xor of LEAF_NOTIFY... flags. */


    /* Cluster */

    int cluster_enabled;      /* Is cluster enabled? */
    //mstime_t cluster_node_timeout; /* Cluster node timeout. */
    char *cluster_configfile; /* Cluster auto-generated config file name. */
    struct clusterState *cluster;  /* State of the cluster */

    int cluster_migration_barrier; /* Cluster replicas migration barrier. */
    /* Scripting */

    // Lua ����
    //lua_State *lua; /* The Lua interpreter. We use just one for all clients */
    
    // ����ִ�� Lua �ű��е� Redis �����α�ͻ���
    //redisClient *lua_client;   /* The "fake client" to query Redis from Lua */

    // ��ǰ����ִ�� EVAL ����Ŀͻ��ˣ����û�о��� NULL
    //redisClient *lua_caller;   /* The client running EVAL right now, or NULL */

    // һ���ֵ䣬ֵΪ Lua �ű�����Ϊ�ű��� SHA1 У���
   // dict *lua_scripts;         /* A dictionary of SHA1 -> Lua scripts */
    // Lua �ű���ִ��ʱ��
   // mstime_t lua_time_limit;  /* Script timeout in milliseconds */
    // �ű���ʼִ�е�ʱ��
   // mstime_t lua_time_start;  /* Start time of script, milliseconds time */

    // �ű��Ƿ�ִ�й�д����
    int lua_write_dirty;  /* True if a write command was called during the
                             execution of the current script. */

    // �ű��Ƿ�ִ�й�����������ʵ�����
    int lua_random_dirty; /* True if a random command was called during the
                             execution of the current script. */

    // �ű��Ƿ�ʱ
    int lua_timedout;     /* True if we reached the time limit for script
                             execution. */

    // �Ƿ�Ҫɱ���ű�
    int lua_kill;         /* Kill the script if true. */


    /* Assert & bug reporting */

    char *assert_failed;
    char *assert_file;
    int assert_line;
    int bug_report_start; /* True if bug report header was already logged. */
    int watchdog_period;  /* Software watchdog period in ms. 0 = off */
};

#endif