/********************************************************************
created:	2014/10/01
filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDTaskThread.h
file base:	NDTaskThread
purpose:	task thread class;
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_TASK_THREAD_H__
#define __SHARE_BASE_ND_TASK_THREAD_H__


#include <list>
using std::list;

#include "NDThread.h"

class CriticalSection;

class NDTaskThread : public NDThread
{
private:
	bool				m_bExit;				// exit flag;
	int					m_nTaskThreadID;		//TaskThread�±�;
	int					m_nSleepMSTime;			//�߳�SLEEP������;

	CriticalSection*	m_DataLock;				//������;

	/// ���������Ϣ�������( pair<clientId, reqContent> );
	struct RecvMsgInfo {
		int		length;
		char*	szBuf;
	};
	list<std::pair<RecvMsgInfo, RecvMsgInfo> > m_recvMsgList;

public:
	NDTaskThread();
	~NDTaskThread();

	void				run();
	void				stop();

	bool				init( int nTaskThreadID, int nSleepMSTime );
	void				release();

	void				insertOperateData(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);	//��list�в���Ҫͬ��������;

	
	bool				isOperateDataEmpty();					//����Ƿ�����Ҫ���������;
	bool				isQuitThread();							//�Ƿ��˳�Thread;
	bool				disposeOperateData();					//����Task����;
};


#endif

