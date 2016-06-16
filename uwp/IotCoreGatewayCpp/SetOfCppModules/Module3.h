#pragma once

namespace SetOfCppModules
{
	public ref class Module3 sealed : IotCoreGatewayUtilitiesCpp::IModule
    {
    public:
		Module3();

		// Inherited via IModule
		virtual void Create(IotCoreGatewayUtilitiesCpp::MessageBus ^bus, Platform::String ^configuration);
		virtual void Destroy();
		virtual void Receive(IotCoreGatewayUtilitiesCpp::IModule ^source, IotCoreGatewayUtilitiesCpp::Message ^received_message);
	};
}
