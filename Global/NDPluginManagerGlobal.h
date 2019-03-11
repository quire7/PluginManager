/********************************************************************
created:	2014/10/01
filename: 	NDPluginManagerGlobal.h
file base:	NDPluginManagerGlobal
purpose:	plugin manger global singleton;
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __ND_PLUGIN_MANAGER_GLOBAL_H__
#define __ND_PLUGIN_MANAGER_GLOBAL_H__

#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;



struct NDPluginManagerConfig
{
	int		task_thread_num;				//task thread number;
	string  zmq_address;					//net service listen address;
	string	plugin_path;					//plugin library path;
	string	initfun_must_return_section;	//plugin init function must return section;
	string	method_name;					//recv msg parse json method_name;
	string	recv_msg_format_error;			//return recv msg format error infor;
	string	recv_msg_method_error;			//return recv msg method error infor;
	string	method_fun_not_exist;			//return method function not exist infor;
	string	method_fun_not_outer;			//return method function not outer infor;
};

class CriticalSection;
class NDNetService;
class NDDataProcess;
class NDPluginLibraryLoad;
class NDBufferPool;
class NDTaskThreadManager;
class NDPluginManagerGlobal
{
private:
	typedef map< string, NDPluginLibraryLoad* >		NDPluginLibraryLoadMap;
	typedef NDPluginLibraryLoadMap::iterator		NDPluginLibraryLoadMapIter;

private:
	CriticalSection*				m_pLogCriticalSection;		//log CriticalSection;
	NDNetService*					m_pNDNetService;			//net service;
	NDBufferPool*					m_pNDBufferPool;			//buffer pool;
	NDDataProcess*					m_pProcess;					//event dispose handle;
	NDTaskThreadManager*			m_pNDTaskThreadManager;		//task thread manager;
	string							m_strInfoForPluginLibrary;	//Plugin manager provide info to plugin library;
	NDPluginManagerConfig			m_strNDPluginManagerConfig; //Plugin manager config;
	NDPluginLibraryLoadMap			m_NDPluginLibraryLoadMap;


	static NDPluginManagerGlobal*	s_pNDPluginManagerGlobal;
public:
	NDPluginManagerGlobal();
	~NDPluginManagerGlobal();

	static NDPluginManagerGlobal*	getInstance();
	static void						releaseInstance();

	bool							init();
	void							release();
	void							loop();
	
	CriticalSection*				getLogCriticalSection() { return m_pLogCriticalSection; }
	NDDataProcess*					getDataProcess()		{ return m_pProcess; }
	NDNetService*					getNetService()			{ return m_pNDNetService; }
	NDTaskThreadManager*			getTaskThreadManager()	{ return m_pNDTaskThreadManager; }

	const char*						getInfoForPluginLibrary() const { return m_strInfoForPluginLibrary.c_str(); };
	const NDPluginManagerConfig&	getPluginManagerConfig() const { return m_strNDPluginManagerConfig; };

	void*							malloc( unsigned int bytes );
	void							free( void* ptr );

public:
	int								getLibVersion( const char* szLibNameNoSuffix );

private:
	//LibNameNoSuffix to find NDPluginLibraryLoad pointer;
	NDPluginLibraryLoad*			getNDPluginLibraryLoad( const char* szLibNameNoSuffix );
	//create infor for plugin library;
	bool							createInfoForPluginLibrary();
	//log init;
	bool							initLog();

	//load plugin manager config;
	bool							loadConfig( vector<string>* pFilesSortVec );

	bool							sortPlugin( vector<string>* pFilesSortVec, vector<string> allFilesVec );

private:
	NDPluginManagerGlobal( const NDPluginManagerGlobal& other );
	NDPluginManagerGlobal& operator = (const NDPluginManagerGlobal& other);
};



#endif // !__ND_PLUGIN_MANAGER_GLOBAL_H__
