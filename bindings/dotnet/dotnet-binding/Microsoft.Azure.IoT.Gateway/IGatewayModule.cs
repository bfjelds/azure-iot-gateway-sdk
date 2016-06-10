﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Microsoft.Azure.IoT.Gateway
{
    /// <summary> Interface to be implemented by the .NET Module </summary>
    public interface IGatewayModule
    {
        /// <summary>
        ///     Creates a module using the specified configuration connecting to the specified message bus.
        /// </summary>
        /// <param name="bus">The bus onto which this module will connect.</param>
        /// <param name="configuration">A string with user-defined configuration for this module.</param>
        /// <returns></returns>
        void Create(MessageBus bus, string configuration);

        /// <summary>
        ///     Disposes of the resources allocated by/for this module.
        /// </summary>
        /// <param name="moduleHandle">The #MODULE_HANDLE of the module to be destroyed.</param>
        /// <returns></returns>
        void Destroy();

        /// <summary>
        ///     The module's callback function that is called upon message receipt.
        /// </summary>
        /// <param name="received_message">The message being sent to the module.</param>
        /// <returns></returns>                
        void Receive(Message received_message);
    }
}
