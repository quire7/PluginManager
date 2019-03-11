#include "NDTaskThread.h"

#include "NDStaticFunctionGlobal.h"
#include "CriticalSection.h"
#include "CJsonHelper.h"
#include "CFunctionImpl.h"
#include "NDNetService.h"
#include "NDDataProcess.h"
#include "NDPluginManagerGlobal.h"


NDTaskThread::NDTaskThread() : NDThread(), m_bExit(true), m_nTaskThreadID( 0 ), m_nSleepMSTime( 0 )
{
		
}

NDTaskThread::~NDTaskThread()
{
	release();
}

bool NDTaskThread::init( int nTaskThreadID, int nSleepMSTime )
{
	m_nTaskThreadID	= nTaskThreadID;
	m_nSleepMSTime	= nSleepMSTime;
	m_DataLock		= new CriticalSection();

	return true;
}

void NDTaskThread::release()
{
	if ( NULL != m_DataLock ) {
		delete m_DataLock;
		m_DataLock = NULL;
	}
}

void NDTaskThread::run()
{
	m_bExit = false;

	while ( true ) {		
		//处理数据;
		disposeOperateData();

		//Thread的退出;
		if ( isQuitThread() ) {
			break;
		}

		// 线程睡眠时间;
		NDStaticFunctionGlobal::sleep( m_nSleepMSTime );
	}
}

void NDTaskThread::stop()
{
	m_bExit = true;
}

void NDTaskThread::insertOperateData(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen)
{
	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();
	// 保存接受消息到处理队列;
	{
		//copy and save data;
		RecvMsgInfo remoteIdInfo;
		remoteIdInfo.length = remoteIdLen;
		remoteIdInfo.szBuf = (char*)pNDPluginManagerGlobal->malloc(remoteIdInfo.length + 1);
		memcpy(remoteIdInfo.szBuf, remoteId, remoteIdInfo.length);
		remoteIdInfo.szBuf[remoteIdInfo.length] = '\0';

		RecvMsgInfo msgDataInfo;
		msgDataInfo.length = msgLen;
		msgDataInfo.szBuf = (char*)pNDPluginManagerGlobal->malloc(msgLen + 1);
		memcpy(msgDataInfo.szBuf, msgData, msgLen);
		msgDataInfo.szBuf[msgLen] = '\0';

		GuardLock lock(m_DataLock);
		m_recvMsgList.push_back( std::make_pair(remoteIdInfo, msgDataInfo) );
	}
}

bool NDTaskThread::isOperateDataEmpty()
{
	GuardLock autolock(m_DataLock);
	return m_recvMsgList.empty();
}

bool NDTaskThread::isQuitThread()
{
	return m_bExit;
}

bool NDTaskThread::disposeOperateData()
{
	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();
	static NDDataProcess* pNDDataProcess	= pNDPluginManagerGlobal->getDataProcess();
	static NDNetService* pNDNetService		= pNDPluginManagerGlobal->getNetService();

	static const NDPluginManagerConfig& refPluginManagerConfig = pNDPluginManagerGlobal->getPluginManagerConfig();

	int nOperatorDataCount = 0;		//每次循环最多操作数据的数量;

	while (true) {

		//超过某个数值就返回吧,防止大量数据出现while时间过长;	
		nOperatorDataCount++;
		if ( nOperatorDataCount > 100 ) {
			break;
		}

		std::pair<RecvMsgInfo, RecvMsgInfo> msg;
		{
			GuardLock autolock(m_DataLock);
			if ( m_recvMsgList.empty() ){
				break;
			}
			msg = m_recvMsgList.front();
			m_recvMsgList.pop_front();
		}

		//MSG DISPOSE;
		char* szRet = NULL;
		PluginDataProcessBase* pBase = NULL;
		do {
			int nMethod = 0;
			{
				CJsonHelper jsonHelper;
				if ( !jsonHelper.parse(msg.second.szBuf) ) {
					szRet = (char*)refPluginManagerConfig.recv_msg_format_error.c_str();
					break;
				}
				if ( !jsonHelper.get( refPluginManagerConfig.method_name.c_str(), nMethod ) ) {
					szRet = (char*)refPluginManagerConfig.recv_msg_method_error.c_str();
					break;
				}
			}

			pBase = (PluginDataProcessBase*)pNDDataProcess->getPluginDataProcessBase(nMethod);
			if ( NULL == pBase ) {
				CPMLog_Error(" method_id:[%d] has unregistered. ", nMethod);
				szRet = (char*)refPluginManagerConfig.method_fun_not_exist.c_str();
				break;
			}

			if ( eFunctionType_Outer != pBase->type ) {
				CPMLog_Error(" method_id:[%d] type:[%d] != function_type:[%d]. ", nMethod, pBase->type, eFunctionType_Outer);
				szRet = (char*)refPluginManagerConfig.method_fun_not_outer.c_str();
				pBase = NULL;
				break;
			}
		} while (0);

		if ( NULL != pBase )
			 szRet = (char*)(pBase->pProcess)( msg.second.szBuf );
		
		int nRetLength = strlen(szRet);

		pNDNetService->sendMessage( msg.first.szBuf, msg.first.length, szRet, nRetLength );

		if ( NULL != pBase )
			(pBase->pFunFree)( szRet );

		pNDPluginManagerGlobal->free( msg.first.szBuf );
		pNDPluginManagerGlobal->free( msg.second.szBuf );
	}

	return true;
}
