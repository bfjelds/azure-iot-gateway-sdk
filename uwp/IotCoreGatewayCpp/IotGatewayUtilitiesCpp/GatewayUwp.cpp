#include "pch.h"

#include "GatewayUwp.h"
#include "MessageBusUwp.h"
#include "MessageUwp.h"

using namespace Windows::Foundation::Collections;
using namespace Microsoft::Azure::IoT::Gateway;


ref class Message;

void InternalGatewayModule::Module_Create(MESSAGE_BUS_HANDLE busHandle, const void* configuration)
{
	_moduleImpl->Create(ref new MessageBus(busHandle), L"");
}
void InternalGatewayModule::Module_Destroy()
{
	_moduleImpl->Destroy();
}
void InternalGatewayModule::Module_Receive(MODULE_HANDLE moduleHandle, MESSAGE_HANDLE messageHandle)
{
	auto msg = ref new Microsoft::Azure::IoT::Gateway::Message(messageHandle);
	_moduleImpl->Receive(msg);
}

Gateway::Gateway(IVector<IGatewayModule^>^ modules)
{
	messagebus_handle = MessageBus_Create();

	VECTOR_HANDLE modules_handle = VECTOR_create(sizeof(MODULE));

	for each (auto mod in modules)
	{
		InternalGatewayModule *imod = new InternalGatewayModule(mod);
		imod->Module_Create(messagebus_handle, NULL);

		MODULE_CPP_STYLE *cppModule = new MODULE_CPP_STYLE;
		cppModule->module_instance = imod;

		MODULE *module = new MODULE;
		module->module_type = MODERN_CPP_TYPE;
		module->module_data = cppModule;

		VECTOR_push_back(modules_handle, module, 1);
	}

	gateway_handle = Gateway_LL_Create2(modules_handle, messagebus_handle);

	const unsigned char content[] = "Hello!!!";
	const char key[] = "Key1";
	const char value[] = "Value1";
	MESSAGE_CONFIG msg;
	msg.source = &content[0];
	msg.size = 9;
	msg.sourceProperties = Map_Create(NULL);
	Map_Add(msg.sourceProperties, &key[0], &value[0]);

	auto message_handle = Message_Create(&msg);
	MessageBus_Publish(messagebus_handle, NULL, message_handle);
	

}


