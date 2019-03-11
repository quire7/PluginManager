/********************************************************************
created:	2014/10/01
filename: 	CFunctionImpl.h
file base:	NDStaticFunctionGlobal
purpose:	static function global struct;
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __ND_STATIC_FUNCTION_GLOBAL_H__
#define __ND_STATIC_FUNCTION_GLOBAL_H__

#include <string>
using std::string;

#include <vector>
using std::vector;

struct NDStaticFunctionGlobal
{
	static bool		isValidParam( const char* szBuf );
	static string	getCurrentDirectory();
	static bool		getFilesByDirectory( const char* szDir, vector<string>* pFilesVec );
	static bool		getLibNameNoSuffix( const char* szLibraryName, string* pStrLibNameNoSuffix );

	/************************************************************************
	** function DESCRIPTION:
	**  NDTime nSleepMSTime: milliseconds, sleep milliseconds.
	************************************************************************/
	static void		sleep(int nSleepMSTime);

	/************************************************************************
	** function DESCRIPTION:
	**  this is BKDR HASH function.
	**  transform calculate string code to only NDUint32 code.
	**  return CRC len.
	************************************************************************/
	static int		bkdr_hash(const char* pBuf);

	/************************************************************************
	** function DESCRIPTION:
	**	return true-- x is right pointer, false -- x is error pointer.
	************************************************************************/
	static bool		isValidPointer(const void* x);
};

#endif // !__ND_STATIC_FUNCTION_GLOBAL_H__
