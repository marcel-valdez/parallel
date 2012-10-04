namespace ZMQ.PipeSort.Test
{
    using System;
    using System.Threading;
    using NSubstitute;
    using NUnit.Framework;
    using TestingTools.Core;
    using TestingTools.Extensions;

    [TestFixture]
    public class PipeNodeTest
    {
        [Test]
        public void TestIfItCanSortNumbers()
        {

            // Arrange
            ManualResetEvent waitForLeft = new ManualResetEvent(false);
            ManualResetEvent waitForRight = new ManualResetEvent(false);
            int[] dataFromLeft = new int[] { 1, 3, 2 };
            int leftRecvIndex = 0;

            int[] dataFromRight = new int[] { 2, 1 };
            int rightRecvIndex = 0;

            int[] expectedSentToRight = new int[] { 1, 2 };
            int[] expectedSentToLeft = new int[] { 3, 2, 1 };

            PipeNode target = GetPipeNode();
            ISocket senderToRight = target.GetSenderToRight();
            ISocket receiverFromLeft = target.GetReceiverFromLeft();
            ISocket receiverFromRight = target.GetReceiverFromRight();
            ISocket senderToLeft = target.GetSenderToLeft();

            receiverFromLeft.Recv(Arg.Any<int>()).Returns(call =>
            {
                if (leftRecvIndex == dataFromLeft.Length)
                {
                    waitForLeft.Set();
                    Thread.Sleep(call.Arg<int>() + 1);
                    return new byte[] { 0, 0, 0, 0 };
                }

                return BitConverter.GetBytes(dataFromLeft[leftRecvIndex++]);
            });

            receiverFromRight.Recv(Arg.Any<int>()).Returns(call =>
            {
                if (leftRecvIndex == dataFromLeft.Length)
                {
                    if (rightRecvIndex < dataFromRight.Length)
                    {
                        return BitConverter.GetBytes(dataFromRight[rightRecvIndex++]);
                    }
                    else 
                    {
                        waitForRight.Set();
                        Thread.Sleep(call.Arg<int>() + 1);
                    }
                }
                else
                {
                    waitForLeft.WaitOne();
                    Thread.Sleep(call.Arg<int>() + 1);
                }

                return new byte[] { 0, 0, 0, 0 };
            });
            
            int expectedKept = 3;
            int actualKept = -1;

            // Act
            target.Start(0, 3);
            waitForRight.WaitOne();

            // Assert
            var actualSentToRight = senderToRight.GetSentIntArray();
            var actualSentToLeft = senderToLeft.GetSentIntArray();
            actualKept = target.GetKept();

            Verify.That(actualKept).IsEqualTo(expectedKept, "Bad expected number kept").Now();
            Verify.That(actualSentToRight).IsEqualTo(expectedSentToRight, "Bad expected sent to right numbers").Now();
            Verify.That(actualSentToLeft).IsEqualTo(expectedSentToLeft, "Bad expected sent to left numbers.").Now();

            // Reset
        }

        private static PipeNode GetPipeNode()
        {
            ISocket senderToRight = Substitute.For<ISocket>();
            ISocket receiverFromLeft = Substitute.For<ISocket>();
            ISocket senderToLeft = Substitute.For<ISocket>();
            ISocket receiverFromRight = Substitute.For<ISocket>();

            return new PipeNode(senderToRight, receiverFromLeft, senderToLeft, receiverFromRight);
        }
    }
}
