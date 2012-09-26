using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NUnit.Framework;
using ZMQ.Sum.SendRecv;
using ZMQ.ZMQExt;
using Fasterflect;
using ZMQ.Extensions;
using System.Threading;

namespace ZMQ.Sum.Test.SendRecv
{
    [TestFixture]
    public class SumadorSendRecvMaestroTest
    {
        private const int TEST_PORT = 13579;
        private const string TEST_BIND_ADDR = "*";
        private const string LOCALHOST = "localhost";
        [Test]
        public void TestIfItCanSendData()
        {
            // Arrange
            Context ctx;
            Socket masterSocket = null;
            Socket slaveSocket1 = null;
            Socket slaveSocket2 = null;

            using (ctx = new Context())
            {
                using (slaveSocket1 = ctx.Socket(SocketType.PULL))
                using (slaveSocket2 = ctx.Socket(SocketType.PULL))
                {
                    masterSocket = ctx.Socket(SocketType.PUSH);
                    masterSocket.Bind(Transport.TCP, TEST_BIND_ADDR, TEST_PORT);

                    slaveSocket1.Connect(Transport.TCP, LOCALHOST, TEST_PORT);
                    slaveSocket2.Connect(Transport.TCP, LOCALHOST, TEST_PORT);

                    int[] numeros = new int[] { 1, 2, 3, 4, 5 };
                    int[] recibidos1 = null;
                    int[] recibidos2 = null;

                    using (var target = new SumadorSendRecvMaestro(2, numeros, masterSocket, masterSocket))
                    {

                        // Act
                        var task1 = Task.Factory.StartNew(() =>
                            {
                                recibidos1 = slaveSocket1.Recv<int[]>();
                            });
                        //.ContinueWith(_ => slaveSocket1.Send<int>(1));
                        var task2 = Task.Factory.StartNew(() =>
                            {
                                recibidos2 = slaveSocket2.Recv<int[]>();
                            });
                        //.ContinueWith(_ => slaveSocket2.Send<int>(1));

                        Thread.Sleep(100);

                        target.CallMethod("EnviarDatos", 2, 1);

                        task1.Wait();
                        task2.Wait();

                        // Assert
                        Assert.That(recibidos1, Is.EquivalentTo(new int[] { 1, 2 }));
                        Assert.That(recibidos2, Is.EquivalentTo(new int[] { 3, 4, 5 }));

                        // Reset
                    }
                }
            }
        }

        [TestAttribute]
        public void TestIfItCanReceiveSum()
        {
            // Arrange
            Context ctx;
            Socket masterSocket = null;
            Socket slaveSocket1 = null;
            Socket slaveSocket2 = null;

            using (ctx = new Context())
            {
                using (slaveSocket1 = ctx.Socket(SocketType.PUSH))
                using (slaveSocket2 = ctx.Socket(SocketType.PUSH))
                {
                    masterSocket = ctx.Socket(SocketType.PULL);
                    masterSocket.Bind(Transport.TCP, TEST_BIND_ADDR, TEST_PORT);

                    slaveSocket1.Connect(Transport.TCP, LOCALHOST, TEST_PORT);
                    slaveSocket2.Connect(Transport.TCP, LOCALHOST, TEST_PORT);

                    using (var target = new SumadorSendRecvMaestro(2, new int[] { }, masterSocket, masterSocket))
                    {

                        // Act           
                        var task = Task.Factory.StartNew(() => target.CallMethod("RecibirDatos"));
                        Thread.Sleep(10);
                        slaveSocket1.Send<double>(1);
                        Thread.Sleep(10);
                        slaveSocket2.Send<double>(2);

                        task.Wait(1000);

                        // Assert
                        Assert.That(target.Resultado, Is.EqualTo(3));

                        // Reset
                    }
                }
            }
        }
    }
}
