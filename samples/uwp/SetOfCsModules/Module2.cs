using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Azure.IoT.Gateway;

namespace SetOfCsModules
{
    public sealed class Module2 : Microsoft.Azure.IoT.Gateway.IGatewayModule
    {
        public void Create(MessageBus bus, string configuration)
        {
            System.Diagnostics.Debug.WriteLine("SetOfCsModules.Module2.Create");
        }

        public void Destroy()
        {
            System.Diagnostics.Debug.WriteLine("SetOfCsModules.Module2.Destroy");
        }

        public void Receive(Message received_message)
        {
            string content = received_message.GetContent();
            var props = received_message.GetProperties();

            StringBuilder sb = new StringBuilder();
            sb.Append("SetOfCsModules.Module2.Receive: Content=" + content + "\r\n");
            sb.Append("SetOfCsModules.Module2.Receive: Properties={");
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
