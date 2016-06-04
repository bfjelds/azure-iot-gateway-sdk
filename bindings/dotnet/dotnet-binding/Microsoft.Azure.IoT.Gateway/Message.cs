using System;
using System.Collections.Generic;
using System.Collections;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Microsoft.Azure.IoT.Gateway
{
    /// <summary> Object that represents a message on the message bus. </summary>
    public class Message
    {
        public byte[] Content { set; get; }

        public Dictionary<string, string> Properties { set; get; }

        public Message()
        {
            Console.WriteLine("This is Message Constructor with no parameters.");
        }

        private byte[] readNullTerminatedByte(MemoryStream bis)
        {
            ArrayList byteArray = new ArrayList();

            byte b = (byte)bis.ReadByte();

            while (b != '\0')
            {
                byteArray.Add(b);
                b = (byte)bis.ReadByte();
            }

            byte[] result = new byte[byteArray.Count];
            for (int index = 0; index < result.Length; index++)
            {
                result[index] = (byte)byteArray[index];
            }

            return result;
        }

        private int readIntFromMemoryStream(MemoryStream input)
        {
            byte[] byteArray = new byte[4];

            input.Read(byteArray, 0, 4);

            Array.Reverse(byteArray); //Have to reverse because BitConverter expects a MSB 
            return BitConverter.ToInt32(byteArray, 0);
        }

        public Message(byte[] msgInByteArray)
        {
            if (msgInByteArray.Length > 14)
            {
                MemoryStream stream = new MemoryStream(msgInByteArray);

                byte header1 = (byte)stream.ReadByte();
                byte header2 = (byte)stream.ReadByte();

                if(header1 == (byte)0xA1 && header2 == (byte)0x60)
                {
                    int arraySizeInInt = readIntFromMemoryStream(stream); 

                    int propCount = readIntFromMemoryStream(stream);

                    if (propCount > 0)
                    {
                        //Here is where we are going to read the properties.
                        this.Properties = new Dictionary<string, string>();
                        for (int count = 0; count < propCount; count++)
                        {
                            byte[] key = readNullTerminatedByte(stream);
                            byte[] value = readNullTerminatedByte(stream);
                            this.Properties.Add(System.Text.Encoding.UTF8.GetString(key, 0, key.Length), System.Text.Encoding.UTF8.GetString(value, 0, value.Length));
                        }

                    }

                    int contentLength = readIntFromMemoryStream(stream);

                    byte[] content = new byte[contentLength];
                    stream.Read(content, 0, contentLength);

                    this.Content = content;
                }

            }
            else
            {
                throw new Exception("Invalid byte array size.");
            }
        }

        public Message(string content, Dictionary<string, string> properties)
        {
            this.Content = System.Text.Encoding.UTF8.GetBytes(content);
            this.Properties = properties;
        }

        public Message(Message message)
        {
            Console.WriteLine("This is Message Constructor with Message Object.");
        }


        private int getPropertiesByteAmount()
        {
            int sizeOfPropertiesInBytes = 0;
            foreach (KeyValuePair<string, string> propertiItem in this.Properties)
            {
                sizeOfPropertiesInBytes += propertiItem.Key.Length + 1;
                sizeOfPropertiesInBytes += propertiItem.Value.Length + 1;
            }

            return sizeOfPropertiesInBytes;
        }
        private int fillByteArrayWithPropertyInBytes(byte[] dst)
        {
            //The content needs to be filled from byte 11th. 
            int currentIndex = 10;
            
            foreach (KeyValuePair<string, string> propertiItem in this.Properties)
            {
                for(int currentChar = 0; currentChar < propertiItem.Key.Length;currentChar++)
                {
                    dst[currentIndex++] = (byte)propertiItem.Key[currentChar];
                }

                dst[currentIndex++] = 0;

                for (int currentChar = 0; currentChar < propertiItem.Value.Length; currentChar++)
                {
                    dst[currentIndex++] = (byte)propertiItem.Value[currentChar];
                }

                dst[currentIndex++] = 0;
            }

            return currentIndex;
        }

        public byte[] ToByteArray()
        {
            //Now this is the poitn where I will serialize a message. 
            //Requirement: 
            //- 2 (0xA1 0x60) = fixed header
            //-4(0x00 0x00 0x00 0x0E) = arrray size[14 bytes in total]
            //- 4(0x00 0x00 0x00 0x00) = 0 properties that follow
            //- 4(0x00 0x00 0x00 0x00) = 0 bytes of message content


            //List of things that I need to do: 
            //1-Calculate the size of the array;
            //2-Create the byte array; 
            //3-Fill the first 2 bytes with 0xA1 and 0x60
            //4-Fill the 4 bytes with the array size;
            //5-Fill the 4 bytes with the amount of properties;
            //6-Fill the bytes with content from key/value of properties (null terminated string separated);
            //7-Fill the amount of bytes on the content in 4 bytes after the properties; 
            //8-Fill up the bytes with the message content. 

            //1-Calculate the size of the array;
            int sizeOfArrray = 2 + 4 + 4 + getPropertiesByteAmount() + 4 + this.Content.Length;

            //2-Create the byte array;
            byte[] returnByteArray = new Byte[sizeOfArrray];

            //3-Fill the first 2 bytes with 0xA1 and 0x60
            returnByteArray[0] = 0xA1;
            returnByteArray[1] = 0x60;

            //4-Fill the 4 bytes with the array size;
            byte[] sizeOfArrayByteArray = BitConverter.GetBytes(sizeOfArrray);
            Array.Reverse(sizeOfArrayByteArray); //Have to reverse because this is not MSB and needs to be.
            returnByteArray[2] = sizeOfArrayByteArray[0];
            returnByteArray[3] = sizeOfArrayByteArray[1];
            returnByteArray[4] = sizeOfArrayByteArray[2];
            returnByteArray[5] = sizeOfArrayByteArray[3];

            //5-Fill the 4 bytes with the amount of properties;
            byte[] numberOfPropertiesInByteArray = BitConverter.GetBytes(this.Properties.Count);
            Array.Reverse(numberOfPropertiesInByteArray); //Have to reverse because this is not MSB and needs to be. 
            returnByteArray[6] = numberOfPropertiesInByteArray[0];
            returnByteArray[7] = numberOfPropertiesInByteArray[1];
            returnByteArray[8] = numberOfPropertiesInByteArray[2];
            returnByteArray[9] = numberOfPropertiesInByteArray[3];

            //6-Fill the bytes with content from key/value of properties (null terminated string separated);
            int msgContentShallStartFromHere = fillByteArrayWithPropertyInBytes(returnByteArray);

            //7-Fill the amount of bytes on the content in 4 bytes after the properties; 
            byte[] contentSizeInByteArray = BitConverter.GetBytes(this.Content.Length);
            Array.Reverse(contentSizeInByteArray); //Have to reverse because this is not MSB and needs to be. 
            returnByteArray[msgContentShallStartFromHere++] = contentSizeInByteArray[0];
            returnByteArray[msgContentShallStartFromHere++] = contentSizeInByteArray[1];
            returnByteArray[msgContentShallStartFromHere++] = contentSizeInByteArray[2];
            returnByteArray[msgContentShallStartFromHere++] = contentSizeInByteArray[3];

            //8-Fill up the bytes with the message content. 

            foreach (byte contentElement in this.Content)
            {
                returnByteArray[msgContentShallStartFromHere++] = contentElement;
            }


            return returnByteArray;
        }
    }
}
