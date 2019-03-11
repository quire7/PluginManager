#include "CFunctionImpl.h"

#include <stdio.h>
#include <stdarg.h>

#include "CFunctionMacro.h"
#include "CJsonHelper.h"
#include "CriticalSection.h"
#include "NDDataProcess.h"
#include "NDPluginManagerGlobal.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

bool	writelog(int type, const char* szModule, const char*szFile, int nLine, char* szFormat, ...)
{
	static CriticalSection* s_logCriticalSection = NDPluginManagerGlobal::getInstance()->getLogCriticalSection();
	static Logger root = Logger::getRoot();

	if ((type < eLogType_Trace || (type > eLogType_Fatal)) ||
		(NULL == szModule || '\0' == szModule[0]) ||
		(NULL == szFile || '\0' == szFile[0])) {
		LOG4CPLUS_ERROR_FMT(root, " [%s] [%s:%d] %s call PluginManger writelog error.", szModule, szFile, nLine, szModule);
		return false;
	}

	char szBuf[CPLUGIN_MAX_LOG] = {0};
	int nLen = SNPRINTF( szBuf, CPLUGIN_MAX_LOG - 1, " [%s] [%s:%d]:", szModule, szFile, nLine);

	va_list vArgList;
	va_start(vArgList, szFormat);
	VSNPRINTF( szBuf + nLen, (CPLUGIN_MAX_LOG - nLen - 1), szFormat, vArgList );
	va_end(vArgList);

	GuardLock lock(s_logCriticalSection);
	switch (type)
	{
	case eLogType_Trace:
		LOG4CPLUS_TRACE_FMT( root, szBuf );
		break;
	case eLogType_Debug:
		LOG4CPLUS_DEBUG_FMT(root, szBuf);
		break;
	case eLogType_Info:
		LOG4CPLUS_INFO_FMT(root, szBuf);
		break;
	case eLogType_Warn:
		LOG4CPLUS_WARN_FMT(root, szBuf);
		break;
	case eLogType_Error:
		LOG4CPLUS_ERROR_FMT( root, szBuf );
		break;
	case eLogType_Fatal:
		LOG4CPLUS_FATAL_FMT(root, szBuf);
		break;
	}

	return true;
}

void*	gMalloc(unsigned int bytes)
{
	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();
	return pNDPluginManagerGlobal->malloc( bytes );
}

void	gFree(void* ptr)
{
	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();
	return pNDPluginManagerGlobal->free( ptr );
}

bool	regDataProcess(const PluginDataProcessBase* pBase)
{
	static NDDataProcess* pDataProcess = NDPluginManagerGlobal::getInstance()->getDataProcess();
	return pDataProcess->registerCallBack(pBase);
}

bool	unRegDataProcess( int method_id )
{
	static NDDataProcess* pDataProcess = NDPluginManagerGlobal::getInstance()->getDataProcess();
	return pDataProcess->unregisterCallBack(method_id);
}

const PluginDataProcessBase* getPluginDataProcessBase(int method_id)
{
	static NDDataProcess* pDataProcess = NDPluginManagerGlobal::getInstance()->getDataProcess();
	return pDataProcess->getPluginDataProcessBase(method_id);
}

bool	parseJsonInfoToPluginInterFaceCommon(const char* szJson, PluginInterFaceCommon* pData)
{
	if ( NULL == szJson || ( '\0' == szJson[0] ) || ( NULL == pData ) ) {
		return false;
	}

	CJsonHelper jsonHelper;
	if ( !jsonHelper.parse(szJson) ) {
		return false;
	}

	string strBuf;
	if (!jsonHelper.get("pMalloc", strBuf)) {
		return false;
	}
	sscanf(strBuf.c_str(), "%p", &pData->pMalloc);

	if (!jsonHelper.get("pFree", strBuf)) {
		return false;
	}
	sscanf(strBuf.c_str(), "%p", &pData->pFree);

	if (!jsonHelper.get("pWriteLog", strBuf)) {
		return false;
	}
	sscanf(strBuf.c_str(), "%p", &pData->pWriteLog);

	if ( !jsonHelper.get( "pRegDataProcess", strBuf ) ) {
		return false;
	}
	sscanf(strBuf.c_str(), "%p", &pData->pRegDataProcess);

	if ( !jsonHelper.get( "pUnRegDataProcess", strBuf ) ) {
		return false;
	}
	sscanf( strBuf.c_str(), "%p", &pData->pUnRegDataProcess );

	if (!jsonHelper.get("pGetPluginDataProcessBase", strBuf)) {
		return false;
	}
	sscanf(strBuf.c_str(), "%p", &pData->pGetPluginDataProcessBase);

	return true;
}