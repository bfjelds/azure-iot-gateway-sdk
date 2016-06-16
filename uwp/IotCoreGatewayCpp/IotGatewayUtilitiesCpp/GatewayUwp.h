#pragma once
#include "pch.h"

#define UWP
#define WIN32

#include "..\..\..\core\inc\gateway.h"
#include "ModuleUwp.h"

namespace IotCoreGatewayUtilitiesCpp
{
	interface class IModule;
	ref class MessageBus;

	class InternalModule : public IGatewayModule
	{
	public:
		InternalModule(IModule ^moduleImpl) { _moduleImpl = moduleImpl; }

		void Module_Create(MESSAGE_BUS_HANDLE busHandle, const void* configuration);
		void Module_Destroy();
		void Module_Receive(MODULE_HANDLE moduleHandle, MESSAGE_HANDLE messageHandle);

	private:
		IModule^ _moduleImpl;
	};

	public ref class Gateway sealed
	{
	public:
		Gateway(Windows::Foundation::Collections::IVector<IotCoreGatewayUtilitiesCpp::IModule^>^ modules);

	private:
		GATEWAY_HANDLE gateway_handle;
		MESSAGE_BUS_HANDLE messagebus_handle;

	};
};


