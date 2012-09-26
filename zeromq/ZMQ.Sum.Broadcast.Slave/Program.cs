namespace ZMQ.Sum.Broadcast.Slave
{
    using System;
    using System.Diagnostics;
    using System.Threading;
    using System.Threading.Tasks;
    using Common;

    public class Program
    {
        private static Semaphore SenalEsclavosListos;
        private static int NumeroDeEsclavos = 0;
        
        public static void EjecutarEsclavos(int cantidadDeEsclavos)
        {
            Program.NumeroDeEsclavos = cantidadDeEsclavos;
            SenalEsclavosListos = new Semaphore(0, cantidadDeEsclavos);

            if (Debug.Listeners.Count == 0)
            {
                Debug.Listeners.Add(new ConsoleTraceListener());
            }                        

            // Crear contexto
            using (var ctx = new Context())
            {
                // Por cada esclavo:
                Debug.WriteLine("for (int i = 0; i < {0}; i++)", cantidadDeEsclavos);
                Debug.Indent();
                var tasks = new Task[cantidadDeEsclavos];
                for (int i = 0; i < cantidadDeEsclavos; i++)
                {
                    // Crear socket de envio (PUSH)
                    Socket socketEnvio = ctx.Socket(SocketType.PUSH);
                    uint PUERTO_ENVIO = ProgramHelper.PUERTO_ENVIO_ESCLAVO;
                    socketEnvio.Connect(Transport.TCP, "localhost", PUERTO_ENVIO);

                    // Crear socket de recepcion (SUBSCRIBER)
                    uint PUERTO_RECEPCION = ProgramHelper.PUERTO_RECEPCION_ESCLAVO;
                    Socket socketRecepcion = ctx.Socket(SocketType.SUB);
                    socketRecepcion.Connect(Transport.TCP, "localhost", PUERTO_RECEPCION);
                    socketRecepcion.Subscribe(new byte[] { });

                    // Inicializar objeto esclavo
                    var esclavo = new SumadorBroadcastEsclavo(i, cantidadDeEsclavos, socketEnvio, socketRecepcion);
                    int indice = i;

                    // Inicializar sumatoria
                    Action task = () =>
                    {
                        using (esclavo)
                        {
                            Debug.WriteLine("Ejecutando operacion en esclavo {0}", indice);
                            SenalEsclavosListos.Release(1);
                            esclavo.EjecutarOperacion();
                        }
                    };

                    tasks[i] = Task.Factory.StartNew(task);
                    Thread.Sleep(5);
                }

                Debug.Unindent();
                // Esperar a que terminen los esclavos
                Task.WaitAll(tasks);
                // Fin.                
            }
        }

        public static void EsperarEsclavosListos()
        {
            for (int i = 0; i < Program.NumeroDeEsclavos; i++)
            {
                Debug.WriteLine("{0} esclavos listos.", i);
                SenalEsclavosListos.WaitOne();
            }
        }        
    }
}
