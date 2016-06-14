#pragma once

#include "pch.h"


namespace IotCoreGatewayUtilitiesCpp
{
	public ref class Message sealed
	{
	public:
		Message();
		Message(Windows::Foundation::Collections::IVector<byte> ^msgInByteArray);
		Message(Platform::String ^content, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ properties);

		Windows::Foundation::Collections::IVector<byte>^ ToByteArray();
	};
};

