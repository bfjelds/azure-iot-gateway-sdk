#pragma once

namespace SetOfCppModules
{
	public ref class Module2 sealed : Microsoft::Azure::IoT::Gateway::IGatewayModule
    {
    public:
		Module2();

		// Inherited via IGatewayModule
		virtual void Create(Microsoft::Azure::IoT::Gateway::MessageBus ^bus, Platform::String ^configuration);
		virtual void Destroy();
		virtual void Receive(Microsoft::Azure::IoT::Gateway::IGatewayModule ^source, Microsoft::Azure::IoT::Gateway::Message ^received_message);
	};
}
