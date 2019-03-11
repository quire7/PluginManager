#include <stdlib.h>
#include <stdio.h>


#include "NDPluginManagerGlobal.h"




int main(int argc, char** argv)
{
	if ( !NDPluginManagerGlobal::getInstance()->init() ) {
		printf(" NDPluginManagerGlobal init failed.");
		return 1;
	}

	NDPluginManagerGlobal::getInstance()->loop();

	NDPluginManagerGlobal::getInstance()->release();

	NDPluginManagerGlobal::releaseInstance();
	
	
	system("pause");
	return 0;
}