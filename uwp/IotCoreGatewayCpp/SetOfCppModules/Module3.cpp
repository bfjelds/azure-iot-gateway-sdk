#include "pch.h"
#include "Module3.h"

using namespace SetOfCppModules;
using namespace Platform;

Module3::Module3()
{
}

void SetOfCppModules::Module3::Create(IotCoreGatewayUtilitiesCpp::MessageBus ^bus, Platform::String ^configuration)
{
	throw ref new Platform::NotImplementedException();
}

void SetOfCppModules::Module3::Destroy()
{
	throw ref new Platform::NotImplementedException();
}

void SetOfCppModules::Module3::Receive(IotCoreGatewayUtilitiesCpp::Message ^received_message)
{
	throw ref new Platform::NotImplementedException();
}
