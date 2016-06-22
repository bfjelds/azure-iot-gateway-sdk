#pragma once

#include "pch.h"

#include "..\..\..\core\inc\message.h"


namespace Microsoft { namespace Azure { namespace IoT { namespace Gateway {

	public ref class Message sealed
	{
	internal:
		Message(MESSAGE_HANDLE message_handle);

		property MESSAGE_HANDLE MessageHandle
		{
			MESSAGE_HANDLE get() { return _message_handle; }
		};

	public:
		Message(Windows::Foundation::Collections::IVector<byte> ^msgInByteArray);
		Message(Platform::String ^content, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ properties);

		Platform::String^ GetContent();
		Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ GetProperties();

	private:
		MESSAGE_HANDLE _message_handle;
	};

}}}};

