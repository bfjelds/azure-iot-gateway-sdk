#include "pch.h"
#include "Module1.h"

using namespace SetOfCppModules;
using namespace Platform;

Module1::Module1()
{
}

void SetOfCppModules::Module1::Create(IotCoreGatewayUtilitiesCpp::MessageBus ^bus, Platform::String ^configuration)
{
	std::wstring formattedText = L"SetOfCppModules::Module1::Create\r\n";
	OutputDebugString(formattedText.c_str());
}

void SetOfCppModules::Module1::Destroy()
{
	std::wstring formattedText = L"SetOfCppModules::Module1::Destroy\r\n";
	OutputDebugString(formattedText.c_str());
}

void SetOfCppModules::Module1::Receive(IotCoreGatewayUtilitiesCpp::IModule ^source, IotCoreGatewayUtilitiesCpp::Message ^received_message)
{
	std::wstring content = received_message->GetContent()->Data();
	auto props = received_message->GetProperties();

	std::wstring sb = L"SetOfCppModules.Module1.Receive: Content=";
	sb += content;
	sb += L"\r\n";

	sb += L"SetOfCppModules.Module1.Receive: Properties={";
	
	std::for_each(begin(props), end(props), [&](Windows::Foundation::Collections::IKeyValuePair<String^, String^>^ item)
	{
		sb += item->Key->Data();
		sb += L"=";
		sb += item->Value->ToString()->Data();
		sb += L", ";
	});
	sb += L"}\r\n";

	OutputDebugString(sb.c_str());
}
