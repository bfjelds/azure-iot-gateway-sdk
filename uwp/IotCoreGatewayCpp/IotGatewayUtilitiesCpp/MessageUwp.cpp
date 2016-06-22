#include "pch.h"
#include "MessageUwp.h"

using namespace Microsoft::Azure::IoT::Gateway;


Message::Message(MESSAGE_HANDLE message_handle)
{
	_message_handle = message_handle;
}

Message::Message(Windows::Foundation::Collections::IVector<byte> ^msgInByteArray)
{
	//_message_handle = Message_CreateFromByteArray();
}

Message::Message(Platform::String ^content, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ properties)
{

}

Platform::String^ Message::GetContent()
{
	Platform::String^ content = ref new Platform::String();
	auto content_buffer = Message_GetContent(_message_handle);
	std::string content_string(reinterpret_cast<const char*>(content_buffer->buffer), content_buffer->size);
	std::wstring content_wstring(content_string.begin(), content_string.end());
	return ref new Platform::String(content_wstring.c_str());
}

Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ Message::GetProperties()
{
	Windows::Foundation::Collections::IMap<Platform::String^, Platform::String^>^ properties = ref new
		Platform::Collections::Map<Platform::String^, Platform::String^>();

	const char* const* keys;
	const char* const* values;
	size_t nProperties;

	auto constmap_handle = Message_GetProperties(_message_handle);

	if (ConstMap_GetInternals(constmap_handle, &keys, &values, &nProperties) == CONSTMAP_OK)
	{
		for (size_t i = 0; i < nProperties; i++)
		{
			std::string key_string(reinterpret_cast<const char*>(keys[i]));
			std::wstring key_wstring(key_string.begin(), key_string.end());

			std::string value_string(reinterpret_cast<const char*>(values[i]));
			std::wstring value_wstring(value_string.begin(), value_string.end());

			properties->Insert(ref new Platform::String(key_wstring.c_str()), ref new Platform::String(value_wstring.c_str()));
		}
	}

	return properties->GetView();
}
