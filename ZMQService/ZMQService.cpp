// ZMQService.cpp ½Ó¿Ú²ã

#include "ZMQService.h"
#include "ZMQServiceImpl.h"


ZMQService::ZMQService()
{
    m_pImpl = new ZMQServiceImpl(this);
}

ZMQService::~ZMQService()
{
	if ( NULL != m_pImpl ) {
		delete m_pImpl;
		m_pImpl = NULL;
	}
}

int ZMQService::start(const char* svrAddr)
{
    return m_pImpl->start(svrAddr);
}

void ZMQService::stop()
{
    m_pImpl->stop();
}

void ZMQService::sendMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen)
{
    m_pImpl->sendMessage(remoteId, remoteIdLen, msgData, msgLen);
}
