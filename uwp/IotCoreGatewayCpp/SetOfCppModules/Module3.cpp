#include "pch.h"
#include "Module3.h"

using namespace SetOfCppModules;
using namespace Platform;

Module3::Module3()
{
}

void SetOfCppModules::Module3::Create(Microsoft::Azure::IoT::Gateway::MessageBus ^bus, Platform::String ^configuration)
{
	std::wstring formattedText = L"SetOfCppModules::Module3::Create\r\n";
	OutputDebugString(formattedText.c_str());
}

void SetOfCppModules::Module3::Destroy()
{
	std::wstring formattedText = L"SetOfCppModules::Module3::Destroy\r\n";
	OutputDebugString(formattedText.c_str());
}

void SetOfCppModules::Module3::Receive(Microsoft::Azure::IoT::Gateway::IGatewayModule ^source, Microsoft::Azure::IoT::Gateway::Message ^received_message)
{
	std::wstring content = received_message->GetContent()->Data();
	auto props = received_message->GetProperties();

	std::wstring sb = L"SetOfCppModules.Module3.Receive: Content=";
	sb += content;
	sb += L"\r\n";

	sb += L"SetOfCppModules.Module3.Receive: Properties={";

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
