/********************************************************************
created:	2014/10/01
filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDTaskThreadManager.h
file base:	NDTaskThreadManager
purpose:	task thread manager class;
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_TASK_THREAD_MANAGER_H__
#define __SHARE_BASE_ND_TASK_THREAD_MANAGER_H__


#include <vector>
using std::vector;

class NDTaskThread;
class NDTaskThreadManager
{
private:
	typedef vector<NDTaskThread*>				NDTaskThreadPtrVec;
	typedef NDTaskThreadPtrVec::iterator		NDTaskThreadPtrVecIter;

	NDTaskThreadPtrVec							m_NDTaskThreadPtrVec;

public:
	NDTaskThreadManager();
	~NDTaskThreadManager();

	bool				init( int nTaskThreadNum, int nSleepMSTime );
	void				release();

	void				stopAllTaskThread();

	void				insertOperateData(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen);	//向list中插入要同步的数据;
};

#endif
