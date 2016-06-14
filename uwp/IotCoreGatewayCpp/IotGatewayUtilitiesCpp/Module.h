#pragma once

#include "pch.h"

namespace IotCoreGatewayUtilitiesCpp
{
	ref class Message;
	ref class MessageBus;

	public interface class IModule
	{
		void Create(MessageBus^ bus, Platform::String ^configuration);
		void Destroy();
		void Receive(Message ^received_message);
	};
};

