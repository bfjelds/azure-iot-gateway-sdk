﻿using System;
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
        Microsoft.Azure.IoT.Gateway.Gateway gateway;
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            deferral = taskInstance.GetDeferral();

            var modules = new List<Microsoft.Azure.IoT.Gateway.IGatewayModule>();
            modules.Add(new SetOfCsModules.Module2());
            modules.Add(new SetOfCsModules.Module1());
            modules.Add(new SetOfCsModules.Module3());

            gateway = new Microsoft.Azure.IoT.Gateway.Gateway(modules);
        }
    }
}
