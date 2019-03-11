#ifndef __ND_CRITICAL_SECTION_H__
#define __ND_CRITICAL_SECTION_H__


#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER) || defined(WIN32)
	#ifndef	WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#elif defined( LINUX )
	#include <pthread.h>
#else
	#error not support platform
#endif

class CriticalSection
{
public:
	CriticalSection()	{ init(); };
	~CriticalSection()	{ uninit(); };
	
	inline bool init()
	{
	#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
		InitializeCriticalSection( &m_Lock );
	#elif defined( LINUX )
		pthread_mutexattr_t attr;
		pthread_mutexattr_init( &attr );
		pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
		pthread_mutex_init( &m_Lock, &attr );
		pthread_mutexattr_destroy( &attr );
	#else
		#error not support platform
	#endif
		return true;
	}

	inline bool uninit()
	{
	#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
		DeleteCriticalSection( &m_Lock );
	#elif defined( LINUX )
		pthread_mutex_destroy( &m_Lock );
	#else
		#error not support platform
	#endif
		return true;
	}

	inline void lock()
	{
	#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
		EnterCriticalSection( &m_Lock );
	#elif defined( LINUX )
		pthread_mutex_lock( &m_Lock );
	#else
		#error not support platform
	#endif
	}

	inline void unlock()
	{
	#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
		LeaveCriticalSection( &m_Lock );
	#elif defined( LINUX )
		pthread_mutex_unlock( &m_Lock );
	#else
		#error not support platform
	#endif
	}

private:

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64)
	CRITICAL_SECTION 	m_Lock;
#elif defined( LINUX )
	pthread_mutex_t		m_Lock;
#else
	#error not support platform
#endif

};

class GuardLock
{
public:
	GuardLock( CriticalSection* pCriticalSection ) : m_pCriticalSection( pCriticalSection ) { m_pCriticalSection->lock(); }
	~GuardLock() { m_pCriticalSection->unlock(); }

private:
	CriticalSection* m_pCriticalSection;
};

#endif // !__ND_CRITICAL_SECTION_H__