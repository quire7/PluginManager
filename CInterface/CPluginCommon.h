/********************************************************************
created:	2014/10/01
filename: 	CPluginCommon.h
file base:	PluginDataProcessBase
purpose:	provide plugin data process base struct;
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __C_INTERFACE_COMMON_H__
#define __C_INTERFACE_COMMON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

	/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 3 define options:

	CPLUGIN_HIDE_SYMBOLS	- Define this in the case where you don't want to ever dllexport symbols
	CPLUGIN_EXPORT_SYMBOLS	- Define this on library build when you want to dllexport symbols (default)
	CPLUGIN_IMPORT_SYMBOLS	- Define this if you want to dllimport symbol

	For *nix builds that support visibility attribute, you can define similar behavior by

	setting default visibility to hidden by adding
	-fvisibility=hidden (for gcc)
	or
	-xldscope=hidden (for sun cc)
	to CFLAGS

	then using the CPLUGIN_API_VISIBILITY flag to "export" the same symbols the way CPLUGIN_EXPORT_SYMBOLS does
	*/

#define CPLUGIN_CDECL	__cdecl
#define CPLUGIN_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(CPLUGIN_HIDE_SYMBOLS) && !defined(CPLUGIN_IMPORT_SYMBOLS) && !defined(CPLUGIN_EXPORT_SYMBOLS)
#define CPLUGIN_EXPORT_SYMBOLS
#endif

#if defined(CPLUGIN_HIDE_SYMBOLS)
#define CPLUGIN_PUBLIC(type)   type CPLUGIN_STDCALL
#elif defined(CPLUGIN_EXPORT_SYMBOLS)
#define CPLUGIN_PUBLIC(type)   __declspec(dllexport) type CPLUGIN_STDCALL
#elif defined(CPLUGIN_IMPORT_SYMBOLS)
#define CPLUGIN_PUBLIC(type)   __declspec(dllimport) type CPLUGIN_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CPLUGIN_CDECL
#define CPLUGIN_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CPLUGIN_API_VISIBILITY)
#define CPLUGIN_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CPLUGIN_PUBLIC(type) type
#endif
#endif



// plugin provide to plugin manager info start;

#ifndef CPLUGIN_FUN_NAME_MAX
#define CPLUGIN_FUN_NAME_MAX	(256)
#endif // !CPLUGIN_FUN_NAME_MAX

// plugin init info;
typedef const char* (*pInit)( const char* szJson, void* pHandle );
typedef bool		(*pUnInit)( const char* szBuf );
typedef bool		(*pExecute)();


// plugin data process base info;
enum eFunctionType
{
	eFunctionType_Invalid,				//invalid type;
	eFunctionType_Inner,				//inner type;
	eFunctionType_Outer,				//outer type;
	eFunctionType_All					//inner and outer type;
};
typedef struct _PluginDataProcessBase
{
	int			type;												//inner or outer function type;
	int			method_id;											//method id;
	const char* (*pProcess)( const char* szJson );					//process function of method id;
	bool		(*pFunFree)( const char* szBuf );					//free callback pProcess function pointer;
}PluginDataProcessBase;

// plugin provide to plugin manager info end;


// plugin manager provide to plugin info start;

//log max string macro;
#ifndef CPLUGIN_MAX_LOG
#define CPLUGIN_MAX_LOG		(4096)
#endif

#ifndef CPLUGIN_MARK
#define CPLUGIN_MARK		__FILE__,__LINE__
#endif // !CPLUGIN_MARK


enum eLogType
{
	eLogType_Trace,
	eLogType_Debug,
	eLogType_Info,
	eLogType_Warn,
	eLogType_Error,
	eLogType_Fatal
};
typedef struct _PluginInterFaceCommon
{
	void*		(*pMalloc)( unsigned int bytes );
	void		(*pFree)( void* ptr );

	bool		(*pWriteLog)( int type, const char* szModule, const char*szFile, int nLine, char* szFormat, ... );
	bool		(*pRegDataProcess)( const PluginDataProcessBase* pBase );	//process register function;
	bool		(*pUnRegDataProcess)( int method_id );						//process unregister function;
	
	const PluginDataProcessBase* (*pGetPluginDataProcessBase)( int method_id ); //getPluginDataProcessBase function;
}PluginInterFaceCommon;

typedef bool	(*pParseJsonInfo)( const char* szJson, PluginInterFaceCommon* pData );

// plugin manager provide to plugin info end;

#ifdef __cplusplus
}
#endif

#endif