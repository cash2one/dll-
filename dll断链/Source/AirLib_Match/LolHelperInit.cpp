
#include "LSemaphore.h"
#include "LolHelperInit.h"
#include "LShareMem.h"
#include "LOL\LOLDef.h"
#include "logfile/LoLHelperLocalLog.h"
#include "AirLibRunOneInstance.h"

BOOL LolHelperInit()
{
	LoLHelperLocalLogInit("WxAirLibDLL");
	
	BOOL runOnceFlag = AirLibOnlyRunOnceInit();
	if (runOnceFlag) {
		LoLLocalLogInfo("AirLibOnlyRunOnceInit success ");
		LoLMemShareAccessInit();
		LoLMemShareInit(sizeof(stLOLShareMem));
	}else {
		LoLLocalLogInfo("AirLibOnlyRunOnceInit fail ");
	}

	return runOnceFlag;
}

extern void MutextPlayerMemDestory();
void LolHelperExit()
{
	MutextPlayerMemDestory();
	LoLMemShareExit();
	LoLMemShareAccessExit();
	AirLibOnlyRunOnceExit();
}