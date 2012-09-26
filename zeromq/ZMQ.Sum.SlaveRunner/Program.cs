using System;
using System.Diagnostics;
using System.Reflection;
using System.Threading.Tasks;
using ZMQ.Common;

namespace ZMQ.Sum.SendRecv
{
    static class Program
    {
        private const string USO_CORRECTO = "Uso correcto:\n {0} [numero de esclavos]";

        /// <summary>
        /// El punto de entrada de la aplicación
        /// </summary>
        [STAThread]
        public static void Main(string[] args)
        {
            Debug.Listeners.Add(new ConsoleTraceListener());

            int numeroDeEsclavos = 0;
            string nombrePrograma = Assembly.GetExecutingAssembly().GetName().Name;            
            string msj = args.VerificarArgumentos(nombrePrograma, out numeroDeEsclavos, usoDelPrograma: USO_CORRECTO);

            if (msj.MostrarError())
            {
                return;
            }

            using (var ctx = new Context())
            {
                Task[] tasks = new Task[numeroDeEsclavos];
                for (int i = 0; i < numeroDeEsclavos; i++)
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
                        using (var esclavo = new SumadorSendRecvEsclavo(sendSocket, recvSocket))
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
