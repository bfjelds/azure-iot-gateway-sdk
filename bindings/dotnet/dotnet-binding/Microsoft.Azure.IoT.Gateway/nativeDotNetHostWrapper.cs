using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.Azure.IoT.Gateway
{
    class nativeDotNetHostWrapper
    {
        //extern __declspec(dllexport) bool dotnetHost_PublishMessage(MESSAGE_BUS_HANDLE bus, const unsigned char* source, int32_t size);
        [DllImport(@"dotnet.dll", EntryPoint = "dotnetHost_PublishMessage", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool dotnetHost_PublishMessage(IntPtr messageBus, byte[] source, Int32 size);

    }
}
