#pragma once

#include "pch.h"

namespace IotCoreGatewayUtilitiesCpp
{
	interface class IModule;
	ref class Message;

	public ref class MessageBus sealed
	{
	public:
		MessageBus(Windows::Foundation::Collections::IVector<IotCoreGatewayUtilitiesCpp::IModule^>^ modules);

		void Publish(Message ^message);

	private:
		Windows::Foundation::Collections::IVector<IotCoreGatewayUtilitiesCpp::IModule^>^ _Modules;

	};
};

