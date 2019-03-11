#include "NDStaticFunctionGlobal.h"

#include "CFunctionImpl.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#else
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#endif


bool NDStaticFunctionGlobal::isValidParam(const char* szBuf)
{
	return ( NULL != szBuf && ( '\0' != szBuf[0] ) );
}

string	NDStaticFunctionGlobal::getCurrentDirectory()
{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string strBuffer(buffer);
	return strBuffer.substr( 0, strBuffer.find_last_of("\\/") );
#else
	char path[256] = { 0 };
	int cnt = readlink("proc/self/exe", path, sizeof(path));
	if (cnt<0 || cnt >= sizeof(path)) {
		return std::string("");
	}
	int i;
	for (i = cnt; i >= 0; i--) {
		if (path[i] == '/') {
			path[i + 1] = '\0';
			break;
		}
	}
	return string(path);
#endif
}

bool NDStaticFunctionGlobal::getFilesByDirectory(const char* szDir, vector<string>* pFilesVec)
{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	std::string strImageDir = string(szDir) + "\\*";
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile( strImageDir.c_str(), &fd );
	if ( hFile == INVALID_HANDLE_VALUE ) {
		CPMLog_Error(" getFilesByDirectory Dir[%s] errorno:[%d].", strImageDir.c_str(), GetLastError() );
		return false;
	}
	while ( FindNextFile(hFile, &fd) ) {
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			continue;
		}
		pFilesVec->push_back(fd.cFileName);
	}
	FindClose(hFile);
#else
	struct stat s;
	DIR* pDir;
	struct dirent *pDirent = NULL;

	lstat( szDir, &s );
	if ( !S_ISDIR(s.st_mode) ) {
		CPMLog_Error( " getFilesByDirectory Dir[%s] S_ISDIR is error.", szDir);
		return false;
	}
	pDir = opendir( szDir );
	if ( NULL == pDir ) {
		CPMLog_Error( " getFilesByDirectory Dir[%s] opendir is error.", szDir );
		return false;
	}
	while ( (pDirent = readdir(pDir)) != NULL ) {
		if ( strcmp(pDirent->d_name, ".") == 0 || (strcmp(pDirent->d_name, "..") == 0) ) {
			continue;
		}
		pFilesVec->push_back(pDirent->d_name);
	}
	closedir(pDir);
#endif
	return true;
}

bool NDStaticFunctionGlobal::getLibNameNoSuffix(const char* szLibraryName, string* pStrLibNameNoSuffix)
{
	if ( !isValidParam(szLibraryName) || (NULL == pStrLibNameNoSuffix) ) {
		return false;
	}

	string strLibraryName(szLibraryName);

#if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
	*pStrLibNameNoSuffix = strLibraryName.substr(0, strLibraryName.find_last_of('.'));
#else
	*pStrLibNameNoSuffix = strLibraryName.substr(3, strLibraryName.find_last_of('.') - 3);
#endif

	return true;
}

void NDStaticFunctionGlobal::sleep(int nSleepMSTime)
{
#ifdef WIN32
	// Translate to milliseconds;
	Sleep((int)nSleepMSTime);
#else
	// Translate to microseconds;
	usleep((int)(nSleepMSTime * 1000));
#endif
}

int NDStaticFunctionGlobal::bkdr_hash(const char* pBuf)
{
	if ( NULL == pBuf || '\0' == pBuf[0] ) {
		return 0;
	}

	char* pBufTemp = (char*)pBuf;

	int	nSeed = 131;	//1313 13131;
	int	nHash = 0;

	while (*pBufTemp) {
		nHash = ((nHash * nSeed) + (*pBufTemp++));
	}

	return (nHash & 0x7FFFFFFF);
}

bool NDStaticFunctionGlobal::isValidPointer(const void* x)
{
#ifdef WIN32
	return (x != (void*)0xcccccccc) && (x != (void*)0xdeadbeef) && (x != (void*)0xfeeefeee) && (x != NULL);
#else
	return (x != NULL);
#endif
}
