using System;
using System.Diagnostics;
using System.Linq;

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
                Console.WriteLine("Cuántos esclavos quieres?\nUso: sendrecv.master.exe [numero de esclavos]");
                return;
            }

            int slaveCount;
            if (!int.TryParse(args[0], out slaveCount))
            {
                Console.WriteLine("Debes indicar el número de esclavos.\nUso: sendrecv.master.exe [numero de esclavos]");
            }

            if (slaveCount < 1 || slaveCount > 8)
            {
                Console.WriteLine("Por lo menos tienes que indicar 1 esclavo, máximo 8 esclavos.\nUso: sendrecv.master.exe [numero de esclavos]");
            }

            int[] numbers = Enumerable.Range(1, 1000).ToArray();
            var stopWatch = Stopwatch.StartNew();
            int resultado = 0;
            using (var ctx = new Context())
            {
                var sendSocket = ctx.Socket(SocketType.PUSH);
                var recvSocket = ctx.Socket(SocketType.PULL);
                uint SENDPORT = 97532;
                uint RECVPORT = 97531;

                sendSocket.Bind(Transport.TCP, "*", SENDPORT);
                recvSocket.Bind(Transport.TCP, "*", RECVPORT);
                using(var master = new SumadorMaestro(slaveCount, numbers, sendSocket, recvSocket))
                {
                    Console.WriteLine("Ejecutando operación maestro.");
                    master.EjecutarOperacion();
                    resultado = master.Result;
                }
            }

            stopWatch.Stop();

            Console.WriteLine("Se tardó: {0} milisegundos", stopWatch.ElapsedMilliseconds);
            Console.WriteLine("Resultado de sumar del 1 al 1000: {0}", resultado);

            Console.ReadLine();
        }
    }
}
