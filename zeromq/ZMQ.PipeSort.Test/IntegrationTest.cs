namespace ZMQ.PipeSort.Test
{
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading;
    using NSubstitute;
    using NUnit.Framework;
    using TestingTools.Core;
    using TestingTools.Extensions;

    [TestFixture]
    public class IntegrationTest
    {
        [Test]
        public void TestIfItCanSortAnArray()
        {
            // Arrange
            int[] data = new int[] { 1, 4, 3, 2 };
            int[] expected = data.OrderByDescending(n => n).ToArray();
            IEnumerable<PipeNode> nodes = GetConnectedPipes(4);
            ISocket masterRecv = TestHelper.GetSocket();
            ISocket masterSend = TestHelper.GetSocket();
            PipeNode firstPipe = nodes.ElementAt(0);
            Connect(sender: masterSend, to: firstPipe.GetReceiverFromLeft());
            Connect(sender: firstPipe.GetSenderToLeft(), to: masterRecv);
            PipeMaster master = TestHelper.GetPipeMaster(masterSend, masterRecv);

            // Act
            int count = 0;
            nodes.ToList().ForEach(n => n.Start(count++, 4));
            int[] actual = master.Sort(data);

            // Assert
            Verify.That(actual).IsEqualTo(expected).Now();

            // Reset
        }

        [Test]
        public void TestIfConnectActuallyWorks()
        {
            for (int i = 0; i < 10; i++) // Prueba idempotencia
            {
                // Arrange
                var sender = TestHelper.GetSocket();
                var receiver = TestHelper.GetSocket();
                byte[] sent = new byte[] { 0, 1, 2, 3 };
                // Act
                Connect(sender, to: receiver);
                sender.Send(sent);
                byte[] recv = receiver.Recv(1000);

                // Assert
                Verify.That(sent).IsEqualTo(recv).Now();

                // Reset
            }
        }

        private void Connect(ISocket sender, ISocket to)
        {
            AutoResetEvent asyncSendEvent = new AutoResetEvent(false);
            AutoResetEvent asyncRecvEvent = new AutoResetEvent(true);
            byte[] data = null;
            to.Recv(Arg.Any<int>())
                .Returns(call =>
                    {
                        byte[] recv = null;
                        asyncSendEvent.WaitOne();
                        recv = data;
                        asyncRecvEvent.Set();

                        return recv;
                    });
            sender.When(_ => _.Send(Arg.Any<byte[]>()))
                .Do(call =>
                {
                    asyncRecvEvent.WaitOne();
                    data = call.Arg<byte[]>();
                    asyncSendEvent.Set();
                });
        }

        private IEnumerable<PipeNode> GetConnectedPipes(int amount)
        {
            ISocket[] leftReceivers = new ISocket[amount];
            ISocket[] leftSenders = new ISocket[amount];
            ISocket[] rightReceivers = new ISocket[amount];
            ISocket[] rightSenders = new ISocket[amount];
            PipeNode[] nodes = new PipeNode[amount];
            for (int i = 0; i < amount; i++)
            {
                leftReceivers[i] = TestHelper.GetSocket();
                leftSenders[i] = TestHelper.GetSocket();
                if (i < amount - 1)
                {
                    rightReceivers[i] = TestHelper.GetSocket();
                    rightSenders[i] = TestHelper.GetSocket();
                }

                if (i > 0)
                {
                    // Se conecta con el nodo anterior
                    Connect(sender: rightSenders[i - 1], to: leftReceivers[i]);
                    Connect(sender: leftSenders[i], to: rightReceivers[i - 1]);
                }

                nodes[i] = new PipeNode(rightSenders[i], leftReceivers[i], leftSenders[i], rightReceivers[i]);
            }

            return nodes;
        }
    }
}
