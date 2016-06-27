#pragma once

#include "pch.h"
#include "..\..\..\core\inc\message_bus.h"

namespace Microsoft { namespace Azure { namespace IoT { namespace Gateway {

	interface class IGatewayModule;
	ref class Message;

	public ref class MessageBus sealed
	{
	internal:
		MessageBus(MESSAGE_BUS_HANDLE handle) 
		{ 
			message_bus_handle = handle;
		}

	public:
		void Publish(IGatewayModule ^source, Message ^message);

	private:
		MESSAGE_BUS_HANDLE message_bus_handle;
	};

}}}};
