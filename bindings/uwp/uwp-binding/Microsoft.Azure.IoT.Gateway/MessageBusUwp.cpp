#include "pch.h"

#include "MessageBusUwp.h"
#include "MessageUwp.h"
#include "IGatewayModule.h"

#include "message_bus.h"

using namespace Windows::Foundation::Collections;
using namespace Microsoft::Azure::IoT::Gateway;

void MessageBus::Publish(IGatewayModule ^source, Message ^message)
{
	MODULE_HANDLE src = nullptr;
	MESSAGE_HANDLE msg = message->MessageHandle;
	MessageBus_Publish(message_bus_handle, src, msg);
}

