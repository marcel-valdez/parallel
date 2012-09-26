using System;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using ZMQ.Common;

namespace ZMQ.Sum.SendRecv
{
    public static class Program
    {
        private const string USO_CORRECTO = "Uso correcto:\n {0} [numero de esclavos] [numeros a sumar]";

        /// <summary>
        /// El punto de entrada de la aplicación
        /// </summary>
        [STAThread]
        public static void Main(string[] args)
        {
            Debug.Listeners.Add(new ConsoleTraceListener());
            
            int numeroDeEsclavos;
            int cantidadDeNumeros;
            if (ProgramHelper.ParsearParametrosMaestro(args, out numeroDeEsclavos, out cantidadDeNumeros, USO_CORRECTO))
            {
                return;
            }

            int[] numbers = Enumerable.Range(1, cantidadDeNumeros).ToArray();
            var stopWatch = Stopwatch.StartNew();
            double resultado = 0;
            using (var ctx = new Context())
            {
                var sendSocket = ctx.Socket(SocketType.PUSH);
                var recvSocket = ctx.Socket(SocketType.PULL);
                uint SENDPORT = 97532;
                uint RECVPORT = 97531;

                sendSocket.Bind(Transport.TCP, "*", SENDPORT);
                recvSocket.Bind(Transport.TCP, "*", RECVPORT);
                using (var master = new SumadorSendRecvMaestro(numeroDeEsclavos, numbers, sendSocket, recvSocket))
                {
                    Console.WriteLine("Ejecutando operación maestro.");
                    master.EjecutarOperacion();
                    resultado = master.Resultado;
                }
            }

            stopWatch.Stop();

            Console.WriteLine("Se tardó: {0} milisegundos", stopWatch.ElapsedMilliseconds);
            Console.WriteLine("Se tardó: {0} ticks de reloj", stopWatch.ElapsedTicks);
            Console.WriteLine("Resultado de sumar del 1 al {1}: {0}", resultado, numbers[numbers.Length - 1]);

            Console.ReadLine();
        }
    }
}
