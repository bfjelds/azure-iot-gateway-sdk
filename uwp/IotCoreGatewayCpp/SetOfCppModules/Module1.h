#pragma once

namespace SetOfCppModules
{
	public ref class Module1 sealed : IotCoreGatewayUtilitiesCpp::IModule
    {
    public:
		Module1();

		// Inherited via IModule
		virtual void Create(IotCoreGatewayUtilitiesCpp::MessageBus ^bus, Platform::String ^configuration);
		virtual void Destroy();
		virtual void Receive(IotCoreGatewayUtilitiesCpp::Message ^received_message);
	};
}
