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
	int					m_nTaskThreadID;		//TaskThread下标;
	int					m_nSleepMSTime;			//线程SLEEP毫秒数;

	CriticalSection*	m_DataLock;				//数据锁;

	/// 待处理的消息缓存队列( pair<clientId, reqContent> );
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

	void				insertOperateData(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);	//向list中插入要同步的数据;

	
	bool				isOperateDataEmpty();					//检查是否还有需要处理的数据;
	bool				isQuitThread();							//是否退出Thread;
	bool				disposeOperateData();					//处理Task数据;
};


#endif

