namespace ZMQ.PipeSort.Test
{
    using System.Linq;
    using NSubstitute;
    using NUnit.Framework;
    using TestingTools.Core;
    using TestingTools.Extensions;

    [TestFixture]
    class PipeMasterTest
    {
        [TestAttribute]
        public void TestIfItCanStartAndEndSort()
        {
            // Arrange
            ISocket sendSocket = TestHelper.GetSocket();
            ISocket recvSocket = TestHelper.GetSocket();

            PipeMaster target = TestHelper.GetPipeMaster(sendSocket, recvSocket);
            int[] input = new int[] { 1, 6, 3, 4, 5, 2, 7 };
            int[] expected = input.OrderBy(n => n).ToArray();
            recvSocket.SetRecvData(expected);
            int[] actual = null;

            // Act
            actual = target.Sort(input);

            // Assert
            int[] actualSentData = sendSocket.GetSentIntArray();
            Verify.That(actual).IsEqualTo(expected).Now();
            Verify.That(actualSentData).IsEqualTo(input).Now();

            // Reset
        }        
    }
}
