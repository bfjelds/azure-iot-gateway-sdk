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
            throw new NotImplementedException();
        }

        public void Destroy()
        {
            throw new NotImplementedException();
        }

        public void Receive(Message received_message)
        {
            throw new NotImplementedException();
        }
    }
}
