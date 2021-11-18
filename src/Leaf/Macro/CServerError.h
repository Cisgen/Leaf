#ifndef CSERVERERROR_H__
#define CSERVERERROR_H__

#define SVR_OK 0
#define SVR_NOK -1

enum NetWorkErr
{
	SVR_NET_PORT_ERR = 1, //网络监听端口错误
	SVR_NET_CREATE_EPOLL_ERR = 2, //创建epoll失败
	SVR_NET_MALLOC_ERR = 3, //申请内存失败
	SVR_NET_LISTEN_ERR = 4, //监听端口失败
	SVR_NET_BIND_PORT_ERR = 5, //绑定端口失败
	SVR_NET_MAX_SIZE_ERR = 6, //超出最大值上限
	SVR_NET_EPOLL_ADD_ERR = 7, //Epoll添加异常
};

#endif