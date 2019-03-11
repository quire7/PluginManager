#include "NDDataProcess.h"

#include "CFunctionImpl.h"


NDDataProcess::NDDataProcess(void)
{
	m_CallBackMap.clear();
}

NDDataProcess::~NDDataProcess(void)
{
	for (CallBackMapIter iter = m_CallBackMap.begin();
						iter != m_CallBackMap.end();
						++iter) {
		if (NULL != iter->second)
			free((void*)iter->second);
	}
	m_CallBackMap.clear();
}

bool NDDataProcess::isValidPluginDataProcessBase(const PluginDataProcessBase* pBase)
{
	return ( ( NULL != pBase ) && ( NULL != pBase->pProcess ) && ( NULL != pBase->pFunFree ) );
}

const PluginDataProcessBase* NDDataProcess::getPluginDataProcessBase(int method_id) 
{
	CallBackMapIter iterFind = m_CallBackMap.find( method_id );
	if (iterFind == m_CallBackMap.end())
		return NULL;
	return iterFind->second;
}

bool NDDataProcess::isExistCallBack(int method_id) 
{
	return ( NULL != getPluginDataProcessBase(method_id) );
}

int NDDataProcess::getCallBackFunctionType(int method_id)
{
	const PluginDataProcessBase* pBase = getPluginDataProcessBase( method_id );
	if ( NULL == pBase )
		return eFunctionType_Invalid;
	return pBase->type;
}

bool NDDataProcess::registerCallBack( const PluginDataProcessBase* pBase )
{
	if ( !isValidPluginDataProcessBase( pBase ) ) {
		CPMLog_Error( " isValidPluginDataProcessBase " );
		return false;
	}

	if ( isExistCallBack( pBase->method_id ) ) {
		CPMLog_Error( " method_id:[%d] has exist.", pBase->method_id );
		return false;
	}

	static int nPluginDataProcessBaseSize = sizeof(PluginDataProcessBase);
	PluginDataProcessBase* pNewBase = (PluginDataProcessBase*)malloc(nPluginDataProcessBaseSize);
	if ( NULL == pNewBase ) {
		CPMLog_Error(" malloc PluginDataProcessBase error. " );
		return false;
	}
	memcpy( pNewBase, pBase, nPluginDataProcessBaseSize );

	m_CallBackMap.insert( std::make_pair( pNewBase->method_id, pNewBase ) );
	
	return true;
}

bool NDDataProcess::unregisterCallBack( int method_id )
{
	if ( m_CallBackMap.empty() )
		return true;
	CallBackMapIter iterFind = m_CallBackMap.find(method_id);
	if (iterFind != m_CallBackMap.end()) {
		if (NULL != iterFind->second)
			free((void*)iterFind->second);
		m_CallBackMap.erase(iterFind);
	}
	return true;
}

CJsonHelper NDDataProcess::process( const char* szJson, NDProtocolHeader& refHeader )
{
	CJsonHelper ret;
	if ( 0 == szJson || ('\0' == szJson[0]) ) {
		CPMLog_Error(" [NDDataProcess::process] param is error. ");
		return ret;
	}

	//int nProtocolID = 0;
	//{
	//	CJsonHelper jsonHelper;
	//	if ( !jsonHelper.parse(szJson) )
	//	{
	//		return ret;
	//	}
	//	if (!jsonHelper.get("method", nProtocolID))
	//	{
	//		return ret;
	//	}
	//}

	const PluginDataProcessBase* pBase = getPluginDataProcessBase( refHeader.method_id );
	if ( NULL == pBase ) {
		printf( " method_id:[%d] has unregistered. ", refHeader.method_id );
		CPMLog_Error( " method_id:[%d] has unregistered. ", refHeader.method_id );
		return ret;
	}

	if ( refHeader.function_type != pBase->type ) {
		CPMLog_Error(" method_id:[%d] type:[%d] != function_type:[%d]. ", refHeader.method_id, pBase->type, refHeader.function_type);
		return ret;
	}

	const char* pszRet = (pBase->pProcess)( szJson );
	bool bRet = ret.parse(pszRet);
	(pBase->pFunFree)(pszRet);

	if ( !bRet ) {
		CPMLog_Error( " method_id:[%d] return json format error.", refHeader.method_id );
		return ret;
	}

	return ret;
}
