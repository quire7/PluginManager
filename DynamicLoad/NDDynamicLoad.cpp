#include "NDDynamicLoad.h"

#ifdef WIN32
#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif



NDDynamicLoad::NDDynamicLoad() :m_handle(NULL)
{

}

NDDynamicLoad::~NDDynamicLoad()
{
#ifdef WIN32
	FreeLibrary( (HMODULE)m_handle );
#else
	dlclose( m_handle );
#endif
}

void* NDDynamicLoad::getProcAddress(const char* szSymbol)
{
#ifdef WIN32
	return GetProcAddress( (HMODULE)m_handle, szSymbol );
#else
	return dlsym( m_handle, szSymbol );
#endif
}

char* NDDynamicLoad::getError()
{
#ifdef WIN32
	memset( m_szMsgBuf, 0, sizeof(m_szMsgBuf) );
	DWORD nRet = FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
								MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								(LPTSTR)&m_szMsgBuf, sizeof(m_szMsgBuf), NULL);
	if (nRet > 2){//drop "\r\n";
		if ((m_szMsgBuf[nRet - 2] == '\r') && (m_szMsgBuf[nRet - 1] == '\n')){
			m_szMsgBuf[nRet - 2] = '\0';
			m_szMsgBuf[nRet - 1] = '\0';
		}
	}
	return m_szMsgBuf;
#else
	return dlerror();
#endif
}

bool NDDynamicLoad::initLibrary(const char* szLibraryFile)
{
	if (NULL == szLibraryFile) return false;
#ifdef WIN32
	m_handle = LoadLibrary( szLibraryFile );
#else
	m_handle = dlopen( szLibraryFile, RTLD_LAZY );
#endif
	if (NULL == m_handle) return false;

	return load();
}