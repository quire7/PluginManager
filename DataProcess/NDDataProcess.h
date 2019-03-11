/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\process\NDDataProcess.h
	file base:	NDDataProcess
	purpose:	data process;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef	__SHARE_BASE_ND_DATA_PROCESS_H__
#define __SHARE_BASE_ND_DATA_PROCESS_H__

#include <string>
using std::string;

#include <map>
using std::map;

#include "CJsonHelper.h"
#include "CPluginCommon.h"


struct NDProtocolHeader
{
	int method_id;
	int function_type;
};

class NDDataProcess
{
public:
	NDDataProcess(void);
	~NDDataProcess(void);

	int  getCallBackFunctionType( int method_id ) ;
	bool isExistCallBack( int method_id );

	bool registerCallBack( const PluginDataProcessBase* pBase );
	bool unregisterCallBack( int method_id );

	const PluginDataProcessBase* getPluginDataProcessBase(int method_id);

	CJsonHelper process( const char* szJson, NDProtocolHeader& refHeader );

private:
	bool isValidPluginDataProcessBase( const PluginDataProcessBase* pBase );


private:
	NDDataProcess(const NDDataProcess& other);
	NDDataProcess& operator = (const NDDataProcess& other);

private:
	typedef std::map< int, const PluginDataProcessBase* >	CallBackMap;
	typedef CallBackMap::iterator							CallBackMapIter;

	CallBackMap												m_CallBackMap;
};

#ifndef NDRegisterCallBackMACRO
#define NDRegisterCallBackMACRO( pDProcess, pNDCallBack )	\
		pDProcess->registerCallBack( pNDCallBack );
#endif




#endif
