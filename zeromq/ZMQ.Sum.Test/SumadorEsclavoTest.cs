using NUnit.Framework;
using ZMQ.Sum.SendRecv;
using ZMQ.ZMQExt;
using System.Linq;
using System.Threading.Tasks;

namespace ZMQ.SendRecv.Test
{
    [TestFixture]
    public class SumadorEsclavoTest
    {
        private const string BIND_HOSTNAME = "*";
        private const string CONNECT_HOSTNAME = "localhost";
        private const int TEST_PORT = 13578;
        [Test]
        public void TestIfItCanSum()
        {
            // Arrange
            Context context = null;
            Socket slave = null;
                        
            ZMQ.Socket master = null;
            using (context = new Context())
            {                
                using (master = context.Socket(SocketType.REQ))
                {
                    master.Connect(Transport.TCP, CONNECT_HOSTNAME, TEST_PORT);

                    slave = context.Socket(SocketType.REP);
                    slave.Bind(Transport.TCP, BIND_HOSTNAME, TEST_PORT);

                    using (SumadorSendRecvEsclavo target = new SumadorSendRecvEsclavo(slave, slave))
                    {
                        int[] numeros = new int[] { 1, 2, 4, 8, 16, 32, 64, 128 };

                        // Act
                        var task = Task.Factory.StartNew(() => target.EjecutarOperacion());
                        master.Send(numeros);
                        double resultado = master.Recv<double>();

                        // Assert
                        Assert.That(resultado, Is.EqualTo(numeros.Sum()));
                        Assert.That(target.Resultado, Is.EqualTo(resultado));

                        // Reset
                    }
                }
            }
        }
    }
}