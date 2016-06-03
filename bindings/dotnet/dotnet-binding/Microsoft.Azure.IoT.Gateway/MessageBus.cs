using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Microsoft.Azure.IoT.Gateway
{
    /// <summary> Object that represents the bus, to where a messsage is going to be published </summary>
    public class MessageBus
    {
        private long msgBusHandle;

        public MessageBus(long msgBus)
        {
            this.msgBusHandle = msgBus;
            Console.WriteLine("This is MessageBus Constructor with an long with value: {0}", this.msgBusHandle);
        }

        /// <summary>
        ///     Publish a message into the gateway message bus. 
        /// </summary>
        /// <param name="message">Object representing the message to be published into the bus.</param>
        /// <returns></returns>
        public void Publish(Message message)
        {
        }
    }
}