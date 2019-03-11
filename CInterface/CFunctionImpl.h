/********************************************************************
created:	2014/10/01
filename: 	CFunctionImpl.h
file base:	PluginDataProcessBase
purpose:	plugin manager provide to plugin function;
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __C_FUNCTION_IMPL_H__
#define __C_FUNCTION_IMPL_H__

#include "CPluginCommon.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void*	gMalloc( unsigned int bytes );
	void	gFree( void* ptr );

	bool	writelog( int type, const char* szModule, const char*szFile, int nLine, char* szFormat, ... );
	bool	regDataProcess( const PluginDataProcessBase* pBase );
	bool	unRegDataProcess( int method_id );

	bool	parseJsonInfoToPluginInterFaceCommon( const char* szJson, PluginInterFaceCommon* pData );

	const PluginDataProcessBase* getPluginDataProcessBase( int method_id );

#ifndef CPMLog_Trace
#define CPMLog_Trace( format, ... ) \
		writelog( eLogType_Trace, "PluginManager", CPLUGIN_MARK, format, ##__VA_ARGS__ );
#endif // !CPMLog_Trace

#ifndef CPMLog_Debug
#define CPMLog_Debug( format, ... ) \
		writelog( eLogType_Debug, "PluginManager", CPLUGIN_MARK, format, ##__VA_ARGS__ );
#endif // !CPMLog_Debug

#ifndef CPMLog_Info
#define CPMLog_Info( format, ... ) \
		writelog( eLogType_Info, "PluginManager", CPLUGIN_MARK, format, ##__VA_ARGS__ );
#endif // !CPMLog_Info

#ifndef CPMLog_Warn
#define CPMLog_Warn( format, ... ) \
		writelog( eLogType_Warn, "PluginManager", CPLUGIN_MARK, format, ##__VA_ARGS__ );
#endif // !CPMLog_Warn

#ifndef CPMLog_Error
#define CPMLog_Error( format, ... ) \
		writelog( eLogType_Error, "PluginManager", CPLUGIN_MARK, format, ##__VA_ARGS__ );
#endif // !CPMLog_Error

#ifndef CPMLog_Fatal
#define CPMLog_Fatal( format, ... ) \
		writelog( eLogType_Fatal, "PluginManager", CPLUGIN_MARK, format, ##__VA_ARGS__ );
#endif // !CPMLog_Fatal



#ifdef __cplusplus
}
#endif

#endif