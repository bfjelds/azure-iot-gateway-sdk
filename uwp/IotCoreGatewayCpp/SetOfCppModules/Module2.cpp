#include "pch.h"
#include "Module2.h"

using namespace SetOfCppModules;
using namespace Platform;

Module2::Module2()
{
}

void SetOfCppModules::Module2::Create(IotCoreGatewayUtilitiesCpp::MessageBus ^bus, Platform::String ^configuration)
{
	throw ref new Platform::NotImplementedException();
}

void SetOfCppModules::Module2::Destroy()
{
	throw ref new Platform::NotImplementedException();
}

void SetOfCppModules::Module2::Receive(IotCoreGatewayUtilitiesCpp::Message ^received_message)
{
	throw ref new Platform::NotImplementedException();
}
