#include "pch.h"

#include "MessageBusUwp.h"
#include "MessageUwp.h"
#include "ModuleUwp.h"

#include "message_bus.h"

using namespace Windows::Foundation::Collections;
using namespace IotCoreGatewayUtilitiesCpp;

void MessageBus::Publish(IModule ^source, Message ^message)
{
	MODULE_HANDLE src = nullptr;
	MESSAGE_HANDLE msg = message->MessageHandle;
	MessageBus_Publish(message_bus_handle, src, msg);
}

