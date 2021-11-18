/*
**  
*  后台创建子线程
*/
#include "CBackThread.h"
#include "CLeafLog.h"

#define BACK_THREAD_NUM  2
#define THREAD_STACK_SIZE (1024*1024*4)

void CreatePthreadInit()
{
	pthread_attr_t attr;
    pthread_t thread;
    size_t stacksize;

    /* Initialization of state vars and objects 
     *
     * 初始化 job 队列，以及线程状态
     */
    for (int j = 0; j < BACK_THREAD_NUM; j++) 
	{
        pthread_mutex_init(&bio_mutex[j],NULL);
        pthread_cond_init(&bio_condvar[j],NULL);
        bio_jobs[j] = listCreate();
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
        if (pthread_create(&thread,&attr, bioProcessBackgroundJobs,arg) != 0) 
		{
            LOG_MSG(LOG_WARNING, "Fatal: Can't initialize Background Jobs.");
            exit(1);
        }

        bio_threads[j] = thread;
    }
}

