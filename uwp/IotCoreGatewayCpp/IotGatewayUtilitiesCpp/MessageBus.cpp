#include "pch.h"

#include "MessageBus.h"
#include "Module.h"

using namespace Windows::Foundation::Collections;
using namespace IotCoreGatewayUtilitiesCpp;

MessageBus::MessageBus(IVector<IModule^>^ modules)
{

	_Modules = modules;

}

void MessageBus::Publish(Message ^message)
{
	for each (auto module in _Modules)
	{
		module->Receive(message);
	}

}

