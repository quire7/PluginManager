#include "NDTaskThreadManager.h"



#include "NDStaticFunctionGlobal.h"
#include "CFunctionImpl.h"
#include "CFunctionMacro.h"
#include "NDPluginManagerGlobal.h"

#include "NDTaskThread.h"


NDTaskThreadManager::NDTaskThreadManager()
{
		
}

NDTaskThreadManager::~NDTaskThreadManager()
{
	release();
}

bool NDTaskThreadManager::init( int nTaskThreadNum, int nSleepMSTime )
{
	char szBuf[64] = { 0 };

	for ( int i = 0; i < nTaskThreadNum; ++i ) {
		NDTaskThread* pNDTaskThread = new NDTaskThread();
		if ( NULL == pNDTaskThread ) {
			CPMLog_Error( " new NDTaskThread failed.");
			return false;
		}

		if ( !pNDTaskThread->init(i, 100) ) {
			delete pNDTaskThread;
			CPMLog_Error(" pNDTaskThread init failed.");
			return false;
		}

		memset(szBuf, 0, sizeof(szBuf));
		SNPRINTF(szBuf, sizeof(szBuf) - 1, "NDTaskThread_%u", i);
		pNDTaskThread->setName(szBuf);
		pNDTaskThread->start();

		m_NDTaskThreadPtrVec.push_back( pNDTaskThread );

		NDStaticFunctionGlobal::sleep(100);
	}

	return true;
}

void NDTaskThreadManager::release()
{
	NDTaskThreadPtrVecIter iter		= m_NDTaskThreadPtrVec.begin();
	NDTaskThreadPtrVecIter iterEnd	= m_NDTaskThreadPtrVec.end();
	for (; iter != iterEnd; ++iter)
	{
		if ( NULL != *iter ) {
			delete *iter;
			*iter = NULL;
		}
	}
}

void NDTaskThreadManager::stopAllTaskThread()
{
	NDTaskThreadPtrVecIter iter = m_NDTaskThreadPtrVec.begin();
	NDTaskThreadPtrVecIter iterEnd = m_NDTaskThreadPtrVec.end();
	for (; iter != iterEnd; ++iter)
	{
		(*iter)->stop();
	}
}

void NDTaskThreadManager::insertOperateData(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen)
{
	static int nVecSize = m_NDTaskThreadPtrVec.size();

	int nHash	= NDStaticFunctionGlobal::bkdr_hash( remoteId );
	int nSelect = nHash % nVecSize;

	NDTaskThread* pNDTaskThread = m_NDTaskThreadPtrVec[nSelect];
	pNDTaskThread->insertOperateData( remoteId, remoteIdLen, msgData, msgLen );
}

