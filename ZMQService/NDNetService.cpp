#include "NDNetService.h"

#include "NDPluginManagerGlobal.h"
#include "NDTaskThreadManager.h"


NDNetService::NDNetService():ZMQService()
{
}

NDNetService::~NDNetService()
{
}

void NDNetService::onRecvMessage(const char* remoteId, int remoteIdLen, const char* msgData, int msgLen)
{
	static NDPluginManagerGlobal* pNDPluginManagerGlobal = NDPluginManagerGlobal::getInstance();
	NDTaskThreadManager* pNDTaskThreadManager = pNDPluginManagerGlobal->getTaskThreadManager();

	pNDTaskThreadManager->insertOperateData( remoteId, remoteIdLen, msgData, msgLen );
}
