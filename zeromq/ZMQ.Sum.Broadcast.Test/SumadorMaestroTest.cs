namespace ZMQ.Sum.Broadcast.Test
{
    using System.Diagnostics;
    using System.Threading;
    using System.Threading.Tasks;
    using NUnit.Framework;
    using ZMQExt;

    [TestFixture]
    public class SumadorMaestroTest
    {
        private const int TEST_PORT = 13579;
        private static ConsoleTraceListener mConsoleListener;
        private const string TEST_BIND_ADDR = "*";
        private const string LOCALHOST = "localhost";

        [TestFixtureSetUp]
        public static void Setup()
        {
            mConsoleListener = new ConsoleTraceListener();
            Debug.Listeners.Add(mConsoleListener);
            Debug.WriteLine("SumadorMaestroTest");
            Debug.Indent();
        }

        [TestFixtureTearDown]
        public static void Teardown()
        {
            Debug.Unindent();
            Debug.Listeners.Remove(mConsoleListener);
        }

        [Test]
        public void TestIfItCanBroadcastData()
        {
            // Arrange
            Context ctx;

            using (ctx = new Context())
            {
                Socket slaveSendSock1 = null;
                Socket slaveRecvSock1 = null;
                Socket slaveSendSock2 = null;
                Socket slaveRecvSock2 = null;

                using (slaveSendSock1 = ctx.Socket(SocketType.PUSH))
                using (slaveRecvSock1 = ctx.Socket(SocketType.SUB))
                using (slaveSendSock2 = ctx.Socket(SocketType.PUSH))
                using (slaveRecvSock2 = ctx.Socket(SocketType.SUB))
                {
                    /* Organizar sockets para maestro */
                    /* Envío */
                    Socket masterSendSocket = ctx.Socket(SocketType.PUB);
                    masterSendSocket.Bind(Transport.TCP, TEST_BIND_ADDR, TEST_PORT);
                    /* Recepción */
                    Socket masterRecvSocket = ctx.Socket(SocketType.PULL);
                    masterRecvSocket.Bind(Transport.TCP, TEST_BIND_ADDR, TEST_PORT + 1);

                    /* Organizar sockets para esclavos */
                    /* Envío */
                    slaveSendSock1.Connect(Transport.TCP, LOCALHOST, TEST_PORT + 1);
                    slaveSendSock2.Connect(Transport.TCP, LOCALHOST, TEST_PORT + 1);
                    /* Recepción */
                    slaveRecvSock1.Connect(Transport.TCP, LOCALHOST, TEST_PORT);
                    slaveRecvSock1.Subscribe(new byte[] { });
                    slaveRecvSock2.Connect(Transport.TCP, LOCALHOST, TEST_PORT);
                    slaveRecvSock2.Subscribe(new byte[] { });                    

                    int[] numeros = new int[] { 1, 2, 3, 4, 5 };
                    int[] recibidos1 = null;
                    int[] recibidos2 = null;

                    using (var target = new SumadorBroadcastMaestro(numeros, ctx, masterSendSocket, masterRecvSocket))
                    {
                        target.RecepcionesEsperadas = 2;

                        var task1 = Task.Factory.StartNew(() =>
                        {
                            recibidos1 = slaveRecvSock1.Recv<int[]>();
                            Thread.Sleep(10);
                            slaveSendSock1.Send(1d);
                        });

                        var task2 = Task.Factory.StartNew(() =>
                        {
                            recibidos2 = slaveRecvSock2.Recv<int[]>();
                            Thread.Sleep(10);
                            slaveSendSock2.Send(3d);
                        });

                        // Act
                        Thread.Sleep(100);

                        target.EjecutarOperacion();

                        task1.Wait();
                        task2.Wait();
                        int totalWaitTime = 3000;
                        while (target.RecepcionesRecibidas < 2 && totalWaitTime > 0)
                        {
                            Thread.Sleep(100);
                            totalWaitTime -= 100;
                        }

                        // Assert
                        Assert.That(recibidos1, Is.EquivalentTo(new int[] { 1, 2, 3, 4, 5 }));
                        Assert.That(recibidos2, Is.EquivalentTo(new int[] { 1, 2, 3, 4, 5 }));
                        Assert.That(target.Resultado, Is.EqualTo(4d));

                        // Reset
                    }
                }
            }
        }
    }
}
