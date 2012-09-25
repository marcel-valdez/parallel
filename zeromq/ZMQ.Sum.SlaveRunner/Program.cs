using System;
using System.Threading.Tasks;
using ZMQ.Sum.SendRecv;

namespace ZMQ.Sum.SendRecv
{
    static class Program
    {
        /// <summary>
        /// El punto de entrada de la aplicación
        /// </summary>
        [STAThread]
        public static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Cuántos esclavos quieres?\nUso: sendrecv.slave.exe [numero de esclavos]");
                return;
            }

            int slaveCount;
            if (!int.TryParse(args[0], out slaveCount))
            {
                Console.WriteLine("Debes indicar el número de esclavos.\nUso: sendrecv.slave.exe [numero de esclavos]");
            }

            if (slaveCount < 1 || slaveCount > 8)
            {
                Console.WriteLine("Por lo menos tienes que indicar 1 esclavo, máximo 8 esclavos.\nUso: sendrecv.slave.exe [numero de esclavos]");
            }
            
            
            using (var ctx = new Context())
            {
                Task[] tasks = new Task[slaveCount];
                for (int i = 0; i < slaveCount; i++)
                {
                    var sendSocket = ctx.Socket(SocketType.PUSH);
                    var recvSocket = ctx.Socket(SocketType.PULL);
                    uint RECVPORT = 97532;
                    uint SENDPORT = 97531;

                    sendSocket.Connect(Transport.TCP, "localhost", SENDPORT);
                    recvSocket.Connect(Transport.TCP, "localhost", RECVPORT);
                    tasks[i] = Task.Factory.StartNew(() =>
                    {
                        int indice = i;
                        using (var esclavo = new SumadorEsclavo(sendSocket, recvSocket))
                        {
                            Console.WriteLine("Ejecutando operación esclavo {0}.", indice);
                            esclavo.EjecutarOperacion();
                        }
                    });
                }

                Task.WaitAll(tasks);
            }

            Console.ReadLine();
        }
    }
}
