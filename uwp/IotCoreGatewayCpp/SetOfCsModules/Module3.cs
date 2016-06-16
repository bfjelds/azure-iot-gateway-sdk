using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using IotCoreGatewayUtilitiesCpp;

namespace SetOfCsModules
{
    public sealed class Module3 : IotCoreGatewayUtilitiesCpp.IModule
    {
        public void Create(MessageBus bus, string configuration)
        {
            System.Diagnostics.Debug.WriteLine("SetOfCsModules.Module3.Create");
        }

        public void Destroy()
        {
            System.Diagnostics.Debug.WriteLine("SetOfCsModules.Module3.Destroy");
        }

        public void Receive(IModule source, Message received_message)
        {
            string content = received_message.GetContent();
            var props = received_message.GetProperties();

            StringBuilder sb = new StringBuilder();
            sb.Append("SetOfCsModules.Module3.Receive: Content=" + content + "\r\n");
            sb.Append("SetOfCsModules.Module3.Receive: Properties={");
            foreach (var key in props.Keys)
            {
                sb.Append(key);
                sb.Append("=");
                sb.Append(props[key] as string);
                sb.Append(", ");
            }
            sb.Append("}\r\n");

            System.Diagnostics.Debug.WriteLine(sb.ToString());
        }
    }
}
