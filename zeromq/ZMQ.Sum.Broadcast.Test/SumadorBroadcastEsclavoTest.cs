namespace ZMQ.Sum.Broadcast.Test
{
    using System.Diagnostics;
    using System.Linq;
    using System.Threading;
    using System.Threading.Tasks;
    using NUnit.Framework;
    using ZMQExt;

    [TestFixture]
    public class SumadorBroadcastEsclavoTest
    {
        private const string BIND_HOSTNAME = "*";
        private const string CONNECT_HOSTNAME = "localhost";
        private const int TEST_PORT = 13578;
        private static ConsoleTraceListener traceListener;

        [TestFixtureSetUp]
        public static void Setup()
        {
            traceListener = new ConsoleTraceListener();
            Debug.Listeners.Add(traceListener);
            Debug.WriteLine("SumadorBroadcastEsclavoTest");
            Debug.Indent();
        }

        [TestFixtureTearDown]
        public static void Teardown()
        {
            Debug.Unindent();
            Debug.Listeners.Remove(traceListener);
            traceListener.Dispose();
        }

        [TestAttribute]
        public void TestIfItCanSumItsRange()
        {
            // Arrange
            using (var context = new Context())
            {                
                using (var masterSend = context.Socket(SocketType.PUB))
                using (var masterRecv = context.Socket(SocketType.PULL))
                {
                    masterRecv.Bind(Transport.TCP, BIND_HOSTNAME, TEST_PORT);
                    masterSend.Bind(Transport.TCP, BIND_HOSTNAME, TEST_PORT + 1);

                    var slaveRecv = context.Socket(SocketType.SUB);
                    slaveRecv.Connect(Transport.TCP, CONNECT_HOSTNAME, TEST_PORT + 1);
                    slaveRecv.Subscribe(new byte[] { });

                    var slaveSend = context.Socket(SocketType.PUSH);
                    slaveSend.Connect(Transport.TCP, CONNECT_HOSTNAME, TEST_PORT);


                    // Act
                    using (SumadorBroadcastEsclavo target = new SumadorBroadcastEsclavo(1, 3, slaveSend, slaveRecv))
                    {
                        var task = Task.Factory.StartNew(() =>
                        {
                            target.EjecutarOperacion();
                        });

                        Thread.Sleep(10);
                        int[] numeros = new int[] { 1, 2, 4, 8, 16, 32, 64, 128 };

                        Thread.Sleep(10);
                        masterSend.Send(numeros);
                        double resultado = masterRecv.Recv<double>();

                        // Assert
                        Assert.That(resultado, Is.EqualTo(4 + 8));
                        Assert.That(target.Resultado, Is.EqualTo(resultado));

                        // Reset
                    }
                }
            }
        }
    }
}
