using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ZMQ.PipeSort
{
    using ZMQExt;
    using ZMQ.Common;

    public static class HelperExtensions
    {
        public static SendStatus Send(this ISocket @this, int[] data)
        {
            byte[] packet = data.ToByteArray();
            return @this.Send(packet);            
        }

        /// <summary>
        /// Receives an int array from an ISocket
        /// </summary>
        /// <param name="this">The socket.</param>
        /// <param name="timeout">The timeout interval in miliseconds.</param>
        /// <returns>The array received, if the operation times out, then an empty array is returned.</returns>
        public static int[] RecvIntArray(this ISocket @this, int timeout)
        {            
            byte[] packet = @this.Recv(timeout);
            if (packet != null)
            {
                return packet.ToIntArray();
            }

            return new int[] { };
        }

        public static SendStatus Send(this ISocket @this, int data)
        {            
            byte[] packet = BitConverter.GetBytes(data);
            return @this.Send(packet);
        }

        /// <summary>
        /// Receives an int array from an ISocket
        /// </summary>
        /// <param name="this">The socket.</param>
        /// <param name="timeout">The timeout interval in miliseconds.</param>
        /// <returns>The array received, if the operation times out, the int.MinValue is returned.</returns>
        public static int RecvInt(this ISocket @this, int timeout)
        {
            byte[] packet = @this.Recv(timeout);
            if (packet != null)
            {
                return BitConverter.ToInt32(packet, 0);
            }

            return int.MinValue;
        }
    }
}
