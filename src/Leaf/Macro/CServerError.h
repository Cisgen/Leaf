#ifndef CSERVERERROR_H__
#define CSERVERERROR_H__

#define SVR_OK 0
#define SVR_NOK -1

enum NetWorkErr
{
	SVR_NET_PORT_ERR = 1, //��������˿ڴ���
	SVR_NET_CREATE_EPOLL_ERR = 2, //����epollʧ��
	SVR_NET_MALLOC_ERR = 3, //�����ڴ�ʧ��
	SVR_NET_LISTEN_ERR = 4, //�����˿�ʧ��
	SVR_NET_BIND_PORT_ERR = 5, //�󶨶˿�ʧ��
	SVR_NET_MAX_SIZE_ERR = 6, //�������ֵ����
	SVR_NET_EPOLL_ADD_ERR = 7, //Epoll����쳣
};

#endif