/*
**  
*  后台创建子线程
*/
#include "CBackThread.h"
#include "CLeafLog.h"
#include "CMacro.h"
#include <stdlib.h>
#include <unistd.h>

#define BACK_THREAD_NUM  2
#define THREAD_STACK_SIZE (1024*1024*4)

// 工作线程，斥互和条件变量
static pthread_t bio_threads[BACK_THREAD_NUM];
static pthread_mutex_t bio_mutex[BACK_THREAD_NUM];
static pthread_cond_t bio_condvar[BACK_THREAD_NUM];


/* The following array is used to hold the number of pending jobs for every
 * OP type. This allows us to export the bioPendingJobsOfType() API that is
 * useful when the main thread wants to perform some operation that may involve
 * objects shared with the background thread. The main thread will just wait
 * that there are no longer jobs of this type to be executed before performing
 * the sensible operation. This data is also useful for reporting. */
// 记录每种类型 job 队列里有多少 job 等待执行
static unsigned long long bio_pending[BACK_THREAD_NUM];

/* This structure represents a background Job. It is only used locally to this
 * file as the API deos not expose the internals at all.
 *
 * 表示后台任务的数据结构
 *
 * 这个结构只由 API 使用，不会被暴露给外部。
 */

struct bio_job 
{
    // 任务创建时的时间
    time_t time; /* Time at which the job was created. */

    /* Job specific arguments pointers. If we need to pass more than three
     * arguments we can just pass a pointer to a structure or alike. 
     *
     * 任务的参数。参数多于三个时，可以传递数组或者结构
     */
    void *arg1, *arg2, *arg3;
};

void* ProcessBackgroundJobs(void *arg)
{
	LOG_MSG(LOG_DEBUG, "Create BackJob Sucess!!! %d %d", (int)getpid(), (int)pthread_self());
	while (1)
	{
		usleep(100);
	}

	return NULL;
};

void CreatePthreadInit()
{
	LOG_MSG(LOG_WARNING, "initialize Background Jobs.");
	pthread_attr_t attr;
    pthread_t thread;
    size_t stacksize;

    /* Initialization of state vars and objects 
     *
     * 初始化 job 队列，以及线程状态
     */
    for (int j = 0; j < BACK_THREAD_NUM; j++) 
	{
        pthread_mutex_init(&bio_mutex[j], NULL);
        pthread_cond_init(&bio_condvar[j], NULL);
        bio_pending[j] = 0;
    }

    /* Set the stack size as by default it may be small in some system 
     *
     * 设置栈大小
     */
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr,&stacksize);
    if (!stacksize) stacksize = 1; /* The world is full of Solaris Fixes */
    while (stacksize < THREAD_STACK_SIZE) stacksize *= 2;
    pthread_attr_setstacksize(&attr, stacksize);

    /* Ready to spawn our threads. We use the single argument the thread
     * function accepts in order to pass the job ID the thread is
     * responsible of. 
     *
     * 创建线程
     */
    for (int j = 0; j < BACK_THREAD_NUM; j++) 
	{
        void *arg = (void*)(unsigned long) j;
        if (pthread_create(&thread,&attr, ProcessBackgroundJobs, arg) != 0) 
		{
            LOG_MSG(LOG_WARNING, "Fatal: Can't initialize Background Jobs.");
            exit(1);
        }

        bio_threads[j] = thread;
    }
}

