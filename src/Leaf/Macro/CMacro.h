#ifndef CMACRO_H__
#define CMACRO_H__

/*
 * 时间处理器的执行 flags
 */
// 文件事件
#define FILE_EVENTS 1
// 时间事件
#define TIME_EVENTS 2
// 所有事件
#define ALL_EVENTS (FILE_EVENTS|TIME_EVENTS)
// 不阻塞，也不进行等待
#define DONT_WAIT 4

/* Log levels */
#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_NOTICE 2
#define LOG_WARNING 3
#define LOG_MAX 4
#define LOG_LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define LOG_DEFAULT_VERBOSITY LOG_NOTICE

/* Static server configuration */
/* 默认的服务器配置值 */
#define LEAF_DEFAULT_HZ        10      /* Time interrupt calls/sec. */
#define LEAF_MIN_HZ            1
#define LEAF_MAX_HZ            500 
#define LEAF_SERVERPORT        6379    /* TCP port */
#define LEAF_TCP_BACKLOG       511     /* TCP listen backlog */
#define LEAF_MAXIDLETIME       0       /* default client timeout: infinite */
#define LEAF_DEFAULT_DBNUM     16
#define LEAF_CONFIGLINE_MAX    1024
#define LEAF_DBCRON_DBS_PER_CALL 16
#define LEAF_MAX_WRITE_PER_EVENT (1024*64)
#define LEAF_SHARED_SELECT_CMDS 10
#define LEAF_SHARED_INTEGERS 10000
#define LEAF_SHARED_BULKHDR_LEN 32
#define LEAF_MAX_LOGMSG_LEN    1024 /* Default maximum length of syslog messages */
#define LEAF_AOF_REWRITE_PERC  100
#define LEAF_AOF_REWRITE_MIN_SIZE (64*1024*1024)
#define LEAF_AOF_REWRITE_ITEMS_PER_CMD 64
#define LEAF_SLOWLOG_LOG_SLOWER_THAN 10000
#define LEAF_SLOWLOG_MAX_LEN 128
#define LEAF_MAX_CLIENTS 10000
#define LEAF_AUTHPASS_MAX_LEN 512
#define LEAF_DEFAULT_SLAVE_PRIORITY 100
#define LEAF_REPL_TIMEOUT 60
#define LEAF_REPL_PING_SLAVE_PERIOD 10
#define LEAF_RUN_ID_SIZE 40
#define LEAF_OPS_SEC_SAMPLES 16
#define LEAF_DEFAULT_REPL_BACKLOG_SIZE (1024*1024)    /* 1mb */
#define LEAF_DEFAULT_REPL_BACKLOG_TIME_LIMIT (60*60)  /* 1 hour */
#define LEAF_REPL_BACKLOG_MIN_SIZE (1024*16)          /* 16k */
#define LEAF_BGSAVE_RETRY_DELAY 5 /* Wait a few secs before trying again. */
#define LEAF_DEFAULT_PID_FILE "./svr.pid"
#define LEAF_DEFAULT_SYSLOG_IDENT "redis"
#define LEAF_DEFAULT_CLUSTER_CONFIG_FILE "nodes.conf"
#define LEAF_DEFAULT_DAEMONIZE 0
#define LEAF_DEFAULT_UNIX_SOCKET_PERM 0
#define LEAF_DEFAULT_TCP_KEEPALIVE 0
#define LEAF_DEFAULT_LOGFILE "./log"
#define LEAF_DEFAULT_SYSLOG_ENABLED 0
#define LEAF_DEFAULT_STOP_WRITES_ON_BGSAVE_ERROR 1
#define LEAF_DEFAULT_RDB_COMPRESSION 1
#define LEAF_DEFAULT_RDB_CHECKSUM 1
#define LEAF_DEFAULT_RDB_FILENAME "dump.rdb"
#define LEAF_DEFAULT_SLAVE_SERVE_STALE_DATA 1
#define LEAF_DEFAULT_SLAVE_READ_ONLY 1
#define LEAF_DEFAULT_REPL_DISABLE_TCP_NODELAY 0
#define LEAF_DEFAULT_MAXMEMORY 0
#define LEAF_DEFAULT_MAXMEMORY_SAMPLES 5
#define LEAF_DEFAULT_AOF_FILENAME "appendonly.aof"
#define LEAF_DEFAULT_AOF_NO_FSYNC_ON_REWRITE 0
#define LEAF_DEFAULT_ACTIVE_REHASHING 1
#define LEAF_DEFAULT_AOF_REWRITE_INCREMENTAL_FSYNC 1
#define LEAF_DEFAULT_MIN_SLAVES_TO_WRITE 0
#define LEAF_DEFAULT_MIN_SLAVES_MAX_LAG 10
#define LEAF_IP_STR_LEN INET6_ADDRSTRLEN
#define LEAF_PEER_ID_LEN (LEAF_IP_STR_LEN+32) /* Must be enough for ip:port */
#define LEAF_BINDADDR_MAX 16
#define LEAF_MIN_RESERVED_FDS 32

#define ACTIVE_EXPIRE_CYCLE_LOOKUPS_PER_LOOP 20 /* Loopkups per loop. */
#define ACTIVE_EXPIRE_CYCLE_FAST_DURATION 1000 /* Microseconds */
#define ACTIVE_EXPIRE_CYCLE_SLOW_TIME_PERC 25 /* CPU max % for keys collection */
#define ACTIVE_EXPIRE_CYCLE_SLOW 0
#define ACTIVE_EXPIRE_CYCLE_FAST 1

/* Protocol and I/O related defines */
#define LEAF_MAX_QUERYBUF_LEN  (1024*1024*1024) /* 1GB max query buffer. */
#define LEAF_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */
#define LEAF_REPLY_CHUNK_BYTES (16*1024) /* 16k output buffer */
#define LEAF_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define LEAF_MBULK_BIG_ARG     (1024*32)
#define LEAF_LONGSTR_SIZE      21          /* Bytes needed for long -> str */
// 指示 AOF 程序每累积这个量的写入数据
// 就执行一次显式的 fsync
#define LEAF_AOF_AUTOSYNC_BYTES (1024*1024*32) /* fdatasync every 32MB */
/* When configuring the Redis eventloop, we setup it so that the total number
 * of file descriptors we can handle are server.maxclients + RESERVED_FDS + FDSET_INCR
 * that is our safety margin. */
#define LEAF_EVENTLOOP_FDSET_INCR (LEAF_MIN_RESERVED_FDS+96)

/* Hash table parameters */
#define LEAF_HT_MINFILL        10      /* Minimal hash table fill 10% */

/* Command flags. Please check the command table defined in the redis.c file
 * for more information about the meaning of every flag. */
// 命令标志
#define LEAF_CMD_WRITE 1                   /* "w" flag */
#define LEAF_CMD_READONLY 2                /* "r" flag */
#define LEAF_CMD_DENYOOM 4                 /* "m" flag */
#define LEAF_CMD_NOT_USED_1 8              /* no longer used flag */
#define LEAF_CMD_ADMIN 16                  /* "a" flag */
#define LEAF_CMD_PUBSUB 32                 /* "p" flag */
#define LEAF_CMD_NOSCRIPT  64              /* "s" flag */
#define LEAF_CMD_RANDOM 128                /* "R" flag */
#define LEAF_CMD_SORT_FOR_SCRIPT 256       /* "S" flag */
#define LEAF_CMD_LOADING 512               /* "l" flag */
#define LEAF_CMD_STALE 1024                /* "t" flag */
#define LEAF_CMD_SKIP_MONITOR 2048         /* "M" flag */
#define LEAF_CMD_ASKING 4096               /* "k" flag */
#endif