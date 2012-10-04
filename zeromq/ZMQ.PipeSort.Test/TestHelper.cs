namespace ZMQ.PipeSort.Test
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading;
    using NSubstitute;
    using NSubstitute.Core;

    public static class TestHelper
    {
        public static int[] GetSentIntArray(this ISocket @this)
        {
            IEnumerable<ICall> calls = @this.ReceivedCalls();
            var sendCalls = calls.Where(call => call.GetMethodInfo().Name.Contains("Send"));
            int[] sent = new int[sendCalls.Count()];
            int i = 0;

            foreach (var sendCall in sendCalls)
            {
                byte[] sentpacket = (byte[])(sendCall.GetArguments()[0]);
                sent[i] = BitConverter.ToInt32(sentpacket, 0);
                i++;
            }

            return sent;
        }

        /// <summary>
        /// Sets the data to be received by the socket.
        /// </summary>
        /// <param name="this">The socket.</param>
        /// <param name="data">The data to set.</param>
        /// <returns>An unsignaled reset event, it gets signaled once the entire arrayhas been received.</returns>
        public static ManualResetEventSlim SetRecvData(this ISocket @this, int[] data)
        {
            ManualResetEventSlim waitToRecvAll = new ManualResetEventSlim(false);
            int index = 0;
            @this.Recv(Arg.Any<int>())
                 .Returns(call =>
                    {
                        int timeout = call.Arg<int>();
                        if (index < data.Length)
                        {
                            return BitConverter.GetBytes(data[index++]);
                        }
                        else
                        {
                            waitToRecvAll.Set();
                            Thread.Sleep(timeout);
                            return new byte[] { };
                        }
                    });

            return waitToRecvAll;
        }

        public static ISocket GetSocket()
        {
            return Substitute.For<ISocket>();
        }

        public static PipeMaster GetPipeMaster(ISocket sendSocket, ISocket recvSocket)
        {
            return new PipeMaster(sendSocket, recvSocket);
        }

        public static ISocket GetSenderToLeft(this PipeNode target)
        {
            return (ISocket)Fasterflect.FieldExtensions.GetFieldValue(target, "senderToLeft");
        }

        public static ISocket GetReceiverFromRight(this PipeNode target)
        {
            return (ISocket)Fasterflect.FieldExtensions.GetFieldValue(target, "receiverFromRight");
        }

        public static ISocket GetReceiverFromLeft(this PipeNode target)
        {
            return (ISocket)Fasterflect.FieldExtensions.GetFieldValue(target, "receiverFromLeft");
        }

        public static ISocket GetSenderToRight(this PipeNode target)
        {
            return (ISocket)Fasterflect.FieldExtensions.GetFieldValue(target, "senderToRight");
        }

        public static int GetKept(this PipeNode target)
        {
            return (int)Fasterflect.FieldExtensions.GetFieldValue(target, "keptNumber");
        }
    }
}
