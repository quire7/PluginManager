/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDThread.h
	file base:	NDThread
	purpose:	thread class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_THREAD_H__
#define __SHARE_BASE_ND_THREAD_H__


#ifdef WIN32
#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <process.h>
typedef	unsigned int		ThreadID;
#elif defined(LINUX)
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
typedef pthread_t			ThreadID;
#endif

//线程状态枚举;
enum EThreadStatus
{
	EThreadStatus_invalid,		//无效;
	EThreadStatus_ready,		//当前线程处于准备状态;
	EThreadStatus_running,		//当前线程处于运行状态;
	EThreadStatus_exiting,		//当前线程处于正在退出状态;
	EThreadStatus_exit,			//当前线程处于退出状态;
};

#ifndef ND_THREAD_NAME_MAX
#define ND_THREAD_NAME_MAX		(64)			//thread name max length;
#endif

//不要改成PIMPL模式,因为static函数的问题吧;
class NDThread
{
private:
	ThreadID			m_nThreadID;
	EThreadStatus		m_eThreadStatus;
	char				m_szName[ND_THREAD_NAME_MAX];

#ifdef WIN32
	HANDLE				m_hThread;
#endif

public:
	NDThread();
	virtual ~NDThread();
	
	void	start();
	void	exit( void* pRetVal );

	virtual	void	run();
	virtual void	stop();

	ThreadID		getThreadID() const					{ return m_nThreadID; };

	EThreadStatus	getStatus() const					{ return m_eThreadStatus; };
	void			setStatus( EThreadStatus eStatus )	{ m_eThreadStatus = eStatus; };

	const char*		getName() const						{ return m_szName; };
	void			setName( const char* szName );


#ifdef WIN32
	static	unsigned int WINAPI myThreadProcess( void* pDerivedThread );
#elif defined(LINUX)
	static	void*	myThreadProcess( void* pDerivedThread );
#endif

};


#endif

