#pragma once

#include "pch.h"

namespace Microsoft { namespace Azure { namespace IoT { namespace Gateway {

	ref class Message;
	ref class MessageBus;

	public interface class IGatewayModule
	{
		void Create(MessageBus^ bus, Platform::String ^configuration);
		void Destroy();
		void Receive(IGatewayModule^ source, Message ^received_message);
	};

}}}};

