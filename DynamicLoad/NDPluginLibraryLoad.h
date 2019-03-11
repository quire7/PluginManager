/********************************************************************
created:	2014/10/01
filename: 	NDPluginLibraryLoad.h
file base:	NDPluginLibraryLoad
purpose:	dynamic load plugin dynamic library(dll or so);
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_PLUGIN_LIBRARY_LOAD_H__
#define __SHARE_BASE_ND_PLUGIN_LIBRARY_LOAD_H__

#include <string>
using std::string;

#include "CPluginCommon.h"
#include "NDDynamicLoad.h"

class NDPluginLibraryLoad : public NDDynamicLoad
{
private:
	
	pInit				m_pInitFun;				//library init function address;
	pUnInit				m_pUnInitFun;			//library uninit function address;
	pExecute			m_pExecuteFun;			//library register function address;

	int					m_nLibVersion;			//library version;
	char*				m_pszLibRetJson;		//m_pUnInitFun return;
	string				m_strLibNameNoSuffix;	//library name no Suffix;

public:
	NDPluginLibraryLoad();
	~NDPluginLibraryLoad();
	
	bool			init( const char* szLibPath, const char* szLibraryName );
	
public:
	const char*		getLibNameNoSuffix() const	{ return m_strLibNameNoSuffix.c_str(); }
	int				getLibVersion() const		{ return m_nLibVersion; };
	
protected:
	bool load();
};



#endif