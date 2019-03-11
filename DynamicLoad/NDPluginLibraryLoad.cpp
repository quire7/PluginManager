#include "NDPluginLibraryLoad.h"

#include "CFunctionMacro.h"
#include "CFunctionImpl.h"
#include "CJsonHelper.h"
#include "NDStaticFunctionGlobal.h"
#include "NDPluginManagerGlobal.h"


NDPluginLibraryLoad::NDPluginLibraryLoad() : 
m_pInitFun(NULL),
m_pUnInitFun(NULL),
m_pExecuteFun(NULL),
m_nLibVersion(0),
m_pszLibRetJson(NULL)
{

}

NDPluginLibraryLoad::~NDPluginLibraryLoad()
{
	if ( NULL != m_pUnInitFun )
		m_pUnInitFun(m_pszLibRetJson);
}

bool NDPluginLibraryLoad::init(const char* szLibPath, const char* szLibraryName)
{
	if ( NULL == szLibPath || ( '\0' == szLibPath[0] ) )	return false;

	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();

	if (!NDStaticFunctionGlobal::getLibNameNoSuffix(szLibraryName, &m_strLibNameNoSuffix)) {
		CPMLog_Error(" library name[%s]: getLibNameNoSuffix error.", szLibraryName);
		return false;
	}

	do {
		string strLibraryNamePath = string(szLibPath) + "/" + string(szLibraryName);
		//load library;
		if (!initLibrary(strLibraryNamePath.c_str())) {
			CPMLog_Error( " load library[%s]:%s.", strLibraryNamePath.c_str(), getError() );
			return false;
		}
	} while (0);



	//call library init function return json infor;
	m_pszLibRetJson = (char*)m_pInitFun( pNDPluginManagerGlobal->getInfoForPluginLibrary(), (void*)parseJsonInfoToPluginInterFaceCommon );

	//parse json infor;
	CJsonHelper jsonHelper;
	if ( !jsonHelper.parse(m_pszLibRetJson) ) {
		CPMLog_Error(" library name[%s]: pInitFun return json format error.", szLibraryName);
		return false;
	}

	int nInitReturn = 0;
	const NDPluginManagerConfig& refConfig = pNDPluginManagerGlobal->getPluginManagerConfig();
	if ( !jsonHelper.get( refConfig.initfun_must_return_section.c_str(), nInitReturn ) ) {
		CPMLog_Error( " library name[%s]: pInitFun return json no [%s] section.", szLibraryName, refConfig.initfun_must_return_section.c_str() );
		return false;
	}

	if ( nInitReturn == 0 ) {
		CPMLog_Error(" library name[%s]: pInitFun return json data show init failed.", szLibraryName );
		return false;
	}

	//execute function;
	bool bRet = m_pExecuteFun();
	if ( !bRet ) {
		CPMLog_Error(" library name[%s]: pExecuteFun return false.", szLibraryName);
		return false;
	}

	return true;
}

bool NDPluginLibraryLoad::load()
{
	char szFunName[CPLUGIN_FUN_NAME_MAX] = { 0 };
	int  nFunNameSize = sizeof(szFunName);
	SNPRINTF( szFunName, nFunNameSize - 1, "init_%s", m_strLibNameNoSuffix.c_str() );
	m_pInitFun = (pInit)getProcAddress( szFunName );
	if ( NULL == m_pInitFun ) {
		CPMLog_Error(" library name[%s]: no [init_%s] function.", m_strLibNameNoSuffix.c_str(), m_strLibNameNoSuffix.c_str());
		return false;
	}

	memset( szFunName, 0, nFunNameSize );
	SNPRINTF( szFunName, nFunNameSize - 1, "uninit_%s", m_strLibNameNoSuffix.c_str() );
	m_pUnInitFun = (pUnInit)getProcAddress( szFunName );
	if ( NULL == m_pUnInitFun ) {
		CPMLog_Error(" library name[%s]: no [uninit_%s] function.", m_strLibNameNoSuffix.c_str(), m_strLibNameNoSuffix.c_str());
		return false;
	}

	memset( szFunName, 0, nFunNameSize );
	SNPRINTF( szFunName, nFunNameSize - 1, "execute_%s", m_strLibNameNoSuffix.c_str() );
	m_pExecuteFun = (pExecute)getProcAddress(szFunName);
	if ( NULL == m_pExecuteFun ) {
		CPMLog_Error(" library name[%s]: no [execute_%s] function.", m_strLibNameNoSuffix.c_str(), m_strLibNameNoSuffix.c_str());
		return false;
	}

	return true;
}
