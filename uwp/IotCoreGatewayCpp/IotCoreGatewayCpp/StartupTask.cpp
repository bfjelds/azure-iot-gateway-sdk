#include "pch.h"
#include "StartupTask.h"

using namespace IotCoreGatewayCpp;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Background;
using namespace IotCoreGatewayUtilitiesCpp;
using namespace SetOfCppModules;


// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

void StartupTask::Run(IBackgroundTaskInstance^ taskInstance)
{
	deferral = taskInstance->GetDeferral();

	auto modules = ref new Vector<IModule^>();
	modules->Append(ref new Module2());
	modules->Append(ref new Module1());
	modules->Append(ref new Module3());
	
	messageBus = ref new MessageBus(modules);
}

