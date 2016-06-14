#include "pch.h"
#include "Module1.h"

using namespace SetOfCppModules;
using namespace Platform;

Module1::Module1()
{
}

void SetOfCppModules::Module1::Create(IotCoreGatewayUtilitiesCpp::MessageBus ^bus, Platform::String ^configuration)
{
	throw ref new Platform::NotImplementedException();
}

void SetOfCppModules::Module1::Destroy()
{
	throw ref new Platform::NotImplementedException();
}

void SetOfCppModules::Module1::Receive(IotCoreGatewayUtilitiesCpp::Message ^received_message)
{
	throw ref new Platform::NotImplementedException();
}
