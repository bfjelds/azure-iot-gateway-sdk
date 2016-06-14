#pragma once

namespace SetOfCppModules
{
	public ref class Module2 sealed : IotCoreGatewayUtilitiesCpp::IModule
    {
    public:
		Module2();

		// Inherited via IModule
		virtual void Create(IotCoreGatewayUtilitiesCpp::MessageBus ^bus, Platform::String ^configuration);
		virtual void Destroy();
		virtual void Receive(IotCoreGatewayUtilitiesCpp::Message ^received_message);
	};
}
