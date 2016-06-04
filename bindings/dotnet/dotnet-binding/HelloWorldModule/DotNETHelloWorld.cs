using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Azure.IoT.Gateway;
using System.Threading;

namespace HelloWorldModule
{

    public class Alpha
    {
        private MessageBus busToPublish;

        public Alpha(MessageBus busToPublish)
        {
            this.busToPublish = busToPublish;
        }

        // This method that will be called when the thread is started
        public void Beta()
        {
            int messageCounter = 0;

            while (true)
            {
                Console.WriteLine("Console.WriteLine --> Alpha.Beta is running in its own thread.");
                Console.WriteLine("Trying to publish a message id: " + messageCounter);

                Dictionary<string, string> thisIsMyProperty = new Dictionary<string, string>();
                thisIsMyProperty.Add("PropertyKeyC#", "PropertyValueC#");

                Message messageToPublish = new Message("Hey! This is message from C# module. MessageId: " + messageCounter, thisIsMyProperty);

                this.busToPublish.Publish(messageToPublish);

                //Publish a message every 5 seconds. 
                Thread.Sleep(5000);
                messageCounter++;
            }
        }
    };

    public class DotNETHelloWorld : IGatewayModule
    {

        private MessageBus busToPublish;

        public void Create(MessageBus bus, string configuration)
        {
            Console.WriteLine("This is C# Create WITH PARAMETERS!");
            this.busToPublish = bus;

            Alpha oAlpha = new Alpha(this.busToPublish);

            Thread oThread = new Thread(new ThreadStart(oAlpha.Beta));
            // Start the thread
            oThread.Start();
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
