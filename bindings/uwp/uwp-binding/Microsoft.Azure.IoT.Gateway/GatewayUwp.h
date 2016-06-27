#pragma once
#include "pch.h"


#include "..\..\..\core\inc\gateway.h"
#include "IGatewayModule.h"

namespace Microsoft { namespace Azure { namespace IoT { namespace Gateway {

	interface class IGatewayModule;
	ref class MessageBus;

	class InternalGatewayModule : public IInternalGatewayModule
	{
	public:
		InternalGatewayModule(IGatewayModule ^moduleImpl) { _moduleImpl = moduleImpl; }

		void Module_Create(MESSAGE_BUS_HANDLE busHandle, const void* configuration);
		void Module_Destroy();
		void Module_Receive(MODULE_HANDLE moduleHandle, MESSAGE_HANDLE messageHandle);

	private:
		IGatewayModule^ _moduleImpl;
	};

	public ref class Gateway sealed
	{
	public:
		Gateway(Windows::Foundation::Collections::IVector<Microsoft::Azure::IoT::Gateway::IGatewayModule^>^ modules);

	private:
		GATEWAY_HANDLE gateway_handle;
		MESSAGE_BUS_HANDLE messagebus_handle;

	};

}}}};


