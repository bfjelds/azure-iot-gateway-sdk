using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Azure.IoT.Gateway;

namespace HelloWorldModule
{
    public class DotNETHelloWorld : IGatewayModule
    {
        public void Create(MessageBus bus, string configuration)
        {
            Console.WriteLine("This is C# Create WITH PARAMETERS!");
        }

        public void Destroy()
        {
            Console.WriteLine("This is C# Destroy!");
        }

        public void Receive(Message received_message)
        {
            Console.WriteLine("This is C# Receive!");
            Console.WriteLine("The MessageContent Received is: " + System.Text.Encoding.UTF8.GetString(received_message.Content, 0, received_message.Content.Length));

            Console.WriteLine("And the Properties are: ");

            foreach(KeyValuePair<string, string> propertiItem in received_message.Properties)
            {
                Console.WriteLine("PropertyKey: " + propertiItem.Key);
                Console.WriteLine("PropertyValue: " + propertiItem.Value);

            }

        }
    }
}
