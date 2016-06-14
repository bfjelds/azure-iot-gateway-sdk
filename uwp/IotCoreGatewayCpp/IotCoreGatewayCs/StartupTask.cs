using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

namespace IotCoreGatewayCs
{
    public sealed class StartupTask : IBackgroundTask
    {
        BackgroundTaskDeferral deferral;
        IotCoreGatewayUtilitiesCpp.MessageBus messageBus;
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            deferral = taskInstance.GetDeferral();

            IList<IotCoreGatewayUtilitiesCpp.IModule> modules = new List<IotCoreGatewayUtilitiesCpp.IModule>();
            modules.Add(new SetOfCsModules.Module2());
            modules.Add(new SetOfCsModules.Module1());
            modules.Add(new SetOfCsModules.Module3());
            messageBus = new IotCoreGatewayUtilitiesCpp.MessageBus(modules);
        }
    }
}
