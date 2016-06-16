#include "pch.h"

#define UWP
#define WIN32

#include "GatewayUwp.h"
#include "MessageBusUwp.h"
#include "MessageUwp.h"

using namespace Windows::Foundation::Collections;
using namespace IotCoreGatewayUtilitiesCpp;

ref class Message;

void InternalModule::Module_Create(MESSAGE_BUS_HANDLE busHandle, const void* configuration)
{
	_moduleImpl->Create(ref new MessageBus(busHandle), L"");
}
void InternalModule::Module_Destroy()
{
	_moduleImpl->Destroy();
}
void InternalModule::Module_Receive(MODULE_HANDLE moduleHandle, MESSAGE_HANDLE messageHandle)
{
	auto msg = ref new IotCoreGatewayUtilitiesCpp::Message(messageHandle);
	IotCoreGatewayUtilitiesCpp::IModule^ mdl = nullptr;
	_moduleImpl->Receive(mdl, msg);
}

Gateway::Gateway(IVector<IModule^>^ modules)
{
	messagebus_handle = MessageBus_Create();

	VECTOR_HANDLE modules_handle = VECTOR_create(sizeof(MODULE));

	for each (auto mod in modules)
	{
		InternalModule *imod = new InternalModule(mod);
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


