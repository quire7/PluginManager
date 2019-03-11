#include "NDPluginManagerGlobal.h"

#include "CFunctionMacro.h"
#include "CFunctionImpl.h"
#include "CriticalSection.h"
#include "NDStaticFunctionGlobal.h"
#include "NDBufferPool.h"
#include "NDNetService.h"
#include "NDDataProcess.h"
#include "NDPluginLibraryLoad.h"
#include "NDTaskThreadManager.h"

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

NDPluginManagerGlobal* NDPluginManagerGlobal::s_pNDPluginManagerGlobal = NULL;

NDPluginManagerGlobal::NDPluginManagerGlobal() :
m_pLogCriticalSection(NULL),
m_pNDNetService(NULL),
m_pNDBufferPool(NULL),
m_pProcess(NULL),
m_pNDTaskThreadManager(NULL)
{
}

NDPluginManagerGlobal::~NDPluginManagerGlobal()
{
	if ( NULL != m_pNDNetService ) {
		delete m_pNDNetService;
		m_pNDNetService = NULL;
	}
	if ( NULL != m_pNDBufferPool ) {
		delete m_pNDBufferPool;
		m_pNDBufferPool = NULL;
	}
	if ( NULL != m_pProcess ) {
		delete m_pProcess;
		m_pProcess = NULL;
	}
	if ( NULL != m_pNDTaskThreadManager ) {
		delete m_pNDTaskThreadManager;
		m_pNDTaskThreadManager = NULL;
	}
	for ( NDPluginLibraryLoadMapIter iter = m_NDPluginLibraryLoadMap.begin();
									iter != m_NDPluginLibraryLoadMap.end();
									++iter ) {
		if ( NULL != iter->second ) {
			delete iter->second;
			iter->second = NULL;
		}
	}

	if ( NULL != m_pLogCriticalSection ) {
		delete m_pLogCriticalSection;
		m_pLogCriticalSection = NULL;
	}
}

NDPluginManagerGlobal* NDPluginManagerGlobal::getInstance()
{
	if ( NULL == s_pNDPluginManagerGlobal )
		s_pNDPluginManagerGlobal = new NDPluginManagerGlobal();

	return s_pNDPluginManagerGlobal;
}

void NDPluginManagerGlobal::releaseInstance()
{
	if (NULL != s_pNDPluginManagerGlobal)
		delete s_pNDPluginManagerGlobal;
	s_pNDPluginManagerGlobal = NULL;
}

bool NDPluginManagerGlobal::init()
{
	//首先初始化日志系统;
	if ( !initLog() ) {
		printf(" NDPluginManagerGlobal::init initLog error. ");
		return false;
	}

	m_pLogCriticalSection = new CriticalSection();
	if ( NULL == m_pLogCriticalSection ) {
		printf(" m_pLogCriticalSection error.");
		return false;
	}

	m_pNDBufferPool = new NDBufferPool();
	if ( NULL == m_pNDBufferPool ) {
		CPMLog_Error(" new NDBufferPool error.");
		return false;
	}

	m_pProcess = new NDDataProcess();
	if ( NULL == m_pProcess ) {
		CPMLog_Error(" new NDDataProcess error.");
		return false;
	}

	//create must infor for plugin library;
	createInfoForPluginLibrary();

	//load config;
	vector<string> filesSortVec;
	
	if ( !loadConfig( &filesSortVec ) ) {
		CPMLog_Error(" load config error.");
		return false;
	}

	//get plugin library file by directory;
	vector<string> allFilesVec;

	if ( !NDStaticFunctionGlobal::getFilesByDirectory( m_strNDPluginManagerConfig.plugin_path.c_str(), &allFilesVec ) ) {
		CPMLog_Error(" Directory[%s] getFilesByDirectory error.", m_strNDPluginManagerConfig.plugin_path.c_str() );
		return false;
	}

	//sort plugin library;
	if ( !sortPlugin( &filesSortVec, allFilesVec ) ) {
		CPMLog_Error(" sortPlugin error.");
		return false;
	}

	//dynamic load plugin library;
	int nLibraryFileSize = filesSortVec.size();
	for ( int i = 0; i < nLibraryFileSize; ++i ) {
		string& refStrLibraryName = filesSortVec[i];
		NDPluginLibraryLoad * pNDPluginLibraryLoad = new NDPluginLibraryLoad();
		if ( !pNDPluginLibraryLoad->init( m_strNDPluginManagerConfig.plugin_path.c_str(), refStrLibraryName.c_str()) ) {
			delete pNDPluginLibraryLoad;
			pNDPluginLibraryLoad = NULL;
			CPMLog_Error( " library name[%s] init error.", refStrLibraryName.c_str() );
			return false;
		}

		m_NDPluginLibraryLoadMap.insert( std::make_pair( pNDPluginLibraryLoad->getLibNameNoSuffix(), pNDPluginLibraryLoad ) );
	}

	//start task thread manager;
	m_pNDTaskThreadManager = new NDTaskThreadManager();
	if ( NULL == m_pNDTaskThreadManager ) {
		CPMLog_Error(" new NDTaskThreadManager failed.");
		return false;
	}
	if ( !m_pNDTaskThreadManager->init( m_strNDPluginManagerConfig.task_thread_num, 100 ) ) {
		CPMLog_Error(" m_pNDTaskThreadManager init failed.");
		return false;
	}

	//start net service;
	m_pNDNetService = new NDNetService();
	if ( NULL == m_pNDNetService ) {
		CPMLog_Error(" m_pNDNetService new NDNetService failed.");
		return false;
	}

	int nRet = m_pNDNetService->start( m_strNDPluginManagerConfig.zmq_address.c_str() );
	if ( nRet != 0 ) {
		CPMLog_Error( " m_pNDNetService start zmq_address[%s] failed.", m_strNDPluginManagerConfig.zmq_address.c_str() );
		return false;
	}

	printf( "Plugin Manager service start success." );
	CPMLog_Info(" Plugin Manager service start success.");
	return true;
}

void NDPluginManagerGlobal::release()
{
	m_pNDNetService->stop();
	m_pNDTaskThreadManager->stopAllTaskThread();
}

void NDPluginManagerGlobal::loop()
{
	do {
		NDStaticFunctionGlobal::sleep(10000);
	} while (true);
}

int NDPluginManagerGlobal::getLibVersion(const char* szLibNameNoSuffix)
{
	NDPluginLibraryLoad* pNDPluginLibraryLoad = getNDPluginLibraryLoad( szLibNameNoSuffix);
	if ( NULL == pNDPluginLibraryLoad )
		return 0;
	
	return pNDPluginLibraryLoad->getLibVersion();
}

NDPluginLibraryLoad* NDPluginManagerGlobal::getNDPluginLibraryLoad( const char* szLibNameNoSuffix)
{
	NDPluginLibraryLoadMapIter iterFind = m_NDPluginLibraryLoadMap.find( szLibNameNoSuffix );
	if ( iterFind == m_NDPluginLibraryLoadMap.end() )
		return NULL;

	return iterFind->second;
}

bool NDPluginManagerGlobal::createInfoForPluginLibrary()
{
	CJsonHelper jsonHelper;

	char szBuf[64] = { 0 };
	SNPRINTF(szBuf, sizeof(szBuf) - 1, "%p", gMalloc);
	jsonHelper.set("pMalloc", szBuf);

	memset(szBuf, 0, sizeof(szBuf));
	SNPRINTF(szBuf, sizeof(szBuf) - 1, "%p", gFree);
	jsonHelper.set("pFree", szBuf);

	memset(szBuf, 0, sizeof(szBuf));
	SNPRINTF(szBuf, sizeof(szBuf) - 1, "%p", writelog);
	jsonHelper.set("pWriteLog", szBuf);

	memset(szBuf, 0, sizeof(szBuf));
	SNPRINTF(szBuf, sizeof(szBuf) - 1, "%p", regDataProcess);
	jsonHelper.set("pRegDataProcess", szBuf);

	memset(szBuf, 0, sizeof(szBuf));
	SNPRINTF(szBuf, sizeof(szBuf) - 1, "%p", unRegDataProcess);
	jsonHelper.set("pUnRegDataProcess", szBuf);

	memset(szBuf, 0, sizeof(szBuf));
	SNPRINTF(szBuf, sizeof(szBuf) - 1, "%p", unRegDataProcess);
	jsonHelper.set("pGetPluginDataProcessBase", szBuf);

	m_strInfoForPluginLibrary = jsonHelper.toString();

	return true;
}

bool NDPluginManagerGlobal::initLog() 
{
	Logger root = Logger::getRoot();
	try
	{
		PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("./log4cplus.properties"));
		LOG4CPLUS_INFO(root, "Start Log");
	}
	catch (...) {
		LOG4CPLUS_FATAL(root, "Exception occured...");
		return false;
	}

	return true;
}

bool NDPluginManagerGlobal::loadConfig( vector<string>* pFilesSortVec )
{
	if ( NULL == pFilesSortVec ) {
		CPMLog_Error( " NDPluginManagerGlobal::loadConfig param is NULL. " );
		return false;
	}

	CJsonHelper jsonHelper;

	do {
		FILE* pFile = fopen("./PluginManager.cfg", "r");
		if (NULL == pFile) {
			CPMLog_Error(" NDPluginManagerGlobal::loadConfig read file[./PluginManager.cfg] error.");
			return false;
		}
		fseek(pFile, 0, SEEK_END);
		int nFileSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		char* szBuf = (char*)malloc(nFileSize + 1);
		if (NULL == szBuf) {
			CPMLog_Error(" NDPluginManagerGlobal::loadConfig malloc is failed.");
			return false;
		}
		fread(szBuf, 1, nFileSize, pFile);
		szBuf[nFileSize] = '\0';


		if ( !jsonHelper.parse(szBuf) ) {
			free(szBuf);
			fclose(pFile);
			CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] json format is error.");
			return false;
		}
		free(szBuf);
		fclose(pFile);
	} while (0);


	if (!jsonHelper.get( "zmq_address", m_strNDPluginManagerConfig.zmq_address) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [zmq_address] section.");
		return false;
	}

	if ( !jsonHelper.get( "plugin_path", m_strNDPluginManagerConfig.plugin_path) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [plugin_path] section.");
		return false;
	}

	CJsonHelper* pOrderJsonHelper = jsonHelper["plugin_order"];
	int nOrderSize = pOrderJsonHelper->getArraySize();
	for (int i = 0; i < nOrderSize; i++)
	{
		string strPluginName;
		if ( !pOrderJsonHelper->get( i, strPluginName ) ) {
			CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg]  [plugin_order] json array [%d] error.", i);
			return false;
		}
		pFilesSortVec->push_back( strPluginName );
	}

	if ( !jsonHelper.get("initfun_must_return_section", m_strNDPluginManagerConfig.initfun_must_return_section) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [initfun_must_return_section] section.");
		return false;
	}

	if ( !jsonHelper.get("task_thread_num", m_strNDPluginManagerConfig.task_thread_num) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [task_thread_num] section.");
		return false;
	}

	if ( !jsonHelper.get("method_name", m_strNDPluginManagerConfig.method_name) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [method_name] section.");
		return false;
	}

	if ( !jsonHelper.get("recv_msg_format_error", m_strNDPluginManagerConfig.recv_msg_format_error) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [recv_msg_format_error] section.");
		return false;
	}

	if ( !jsonHelper.get("recv_msg_method_error", m_strNDPluginManagerConfig.recv_msg_method_error) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [recv_msg_method_error] section.");
		return false;
	}

	if ( !jsonHelper.get("method_fun_not_exist", m_strNDPluginManagerConfig.method_fun_not_exist) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [method_fun_not_exist] section.");
		return false;
	}

	if ( !jsonHelper.get("method_fun_not_outer", m_strNDPluginManagerConfig.method_fun_not_outer) ) {
		CPMLog_Error(" NDPluginManagerGlobal::loadConfig file[./PluginManager.cfg] not exist [method_fun_not_outer] section.");
		return false;
	}

	return true;
}

bool NDPluginManagerGlobal::sortPlugin(vector<string>* pFilesSortVec, vector<string> allFilesVec)
{
	if ( NULL == pFilesSortVec ) {
		return false;
	}
	if ( pFilesSortVec->empty() ) {
		pFilesSortVec->assign( allFilesVec.begin(), allFilesVec.end() );
		return true;
	}

	//first check pFilesSortVec string is exist;
	int nOriginSortSize = pFilesSortVec->size();
	for (int i = 0; i < nOriginSortSize; i++)
	{
		string& refStrPluginName = (*pFilesSortVec)[i];
		vector<string>::iterator iter = find( allFilesVec.begin(), allFilesVec.end(), refStrPluginName );
		if ( iter == allFilesVec.end() ) {
			CPMLog_Error( " NDPluginManagerGlobal::sortPlugin [plugin_order] json array [%s] not exist.", refStrPluginName.c_str() );
			return false;
		}
		allFilesVec.erase(iter);
	}

	pFilesSortVec->insert( pFilesSortVec->end(), allFilesVec.begin(), allFilesVec.end() );

	return true;
}

void* NDPluginManagerGlobal::malloc(unsigned int bytes)
{
	return m_pNDBufferPool->malloc(bytes);
}

void NDPluginManagerGlobal::free(void* ptr)
{
	m_pNDBufferPool->free(ptr);
}
