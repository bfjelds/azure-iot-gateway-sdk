#include "pch.h"
#include "Message.h"

using namespace IotCoreGatewayUtilitiesCpp;


Message::Message()
{

}

Message::Message(Windows::Foundation::Collections::IVector<byte> ^msgInByteArray)
{

}

Message::Message(Platform::String ^content, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ properties)
{

}

Windows::Foundation::Collections::IVector<byte>^ Message::ToByteArray()
{
	Windows::Foundation::Collections::IVector<byte> ^byteArray =
		ref new Platform::Collections::Vector<byte>;
	return byteArray;
}

