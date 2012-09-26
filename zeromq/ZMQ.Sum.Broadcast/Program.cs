using System;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using ZMQ.Common;
using SlaveProgram = ZMQ.Sum.Broadcast.Slave.Program;

namespace ZMQ.Sum.Broadcast.Master
{
    class Program
    {
        private const string USO_CORRECTO = "Uso correcto:\n {0} [numero de esclavos] [numeros a sumar]";

        static void Main(string[] args)
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
                /* Crear socket de envío */
                uint PUERTO_ENVIO = ProgramHelper.PUERTO_ENVIO_MASTER;
                var sendSocket = ctx.Socket(SocketType.PUB);                
                sendSocket.Bind(Transport.TCP, "*", PUERTO_ENVIO);
                
                /* Crear socket de recepción */
                uint PUERTO_RECEPCION = ProgramHelper.PUERTO_RECEPCION_MASTER;
                Socket recvSocket = ctx.Socket(SocketType.PULL);
                recvSocket.Bind(Transport.TCP, "*", PUERTO_RECEPCION);

                /* Inicializando esclavos */
                Debug.WriteLine("Master: Inicializando esclavos");
                Task.Factory.StartNew(() => SlaveProgram.EjecutarEsclavos(numeroDeEsclavos));
                Thread.Sleep(5);
                Debug.WriteLine("Master: Esperando a que se inicializen los esclavos.");
                SlaveProgram.EsperarEsclavosListos();
                Debug.WriteLine("Master: Esclavos listos, empieza inicialización de maestro.");

                using (var master = new SumadorBroadcastMaestro(numbers, ctx, sendSocket, recvSocket))
                {
                    Debug.WriteLine("Master: new SumadorBroadcastMaestro(int[{0}], ctx, sendSocket, recvSocket)", numbers.Length);
                    Console.WriteLine("Master: Ejecutando operación maestro.");
                    Debug.Indent();
                    master.RecepcionesEsperadas = numeroDeEsclavos;
                    master.EjecutarOperacion();

                    Debug.WriteLine("Master: Esperando a que termine la operacion...");
                    master.SenalDeFinalizacion.Wait();
                    Debug.WriteLine("Master: Se recibió la senal de finalización.");
                    Debug.Unindent();
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
