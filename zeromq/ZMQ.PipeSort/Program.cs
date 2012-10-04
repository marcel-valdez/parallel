namespace ZMQ.PipeSort
{
    using System;
    using System.Diagnostics;
    using System.Linq;

    class Program
    {
        private const int NUM_NODOS = 7;

        public static void Main(string[] args)
        {
            Debug.Listeners.Add(new ConsoleTraceListener());
            // Informar que se van a ordenar 8 números.
            Console.WriteLine("Se ordenadaran 7 números.");
            int[] numeros = new int[] { 1, 3, 6, 5, 4, 7, 2 };

            // Imprimir números desordenados.
            Console.Write("Números: ");
            numeros.ToList().ForEach(n => Console.Write("{0}, ", n));
            Console.WriteLine();

            // Crear sockets esclavos y esclavos.
            using (var ctx = new Context())
            {
                // Crear sockets maestro y maestro.
                const int MASTER_PORT_IN = 5666;
                const int MASTER_PORT_OUT = 6666;
                Socket masterIn = GetPullSocket(ctx);
                Bind(masterIn, MASTER_PORT_IN);

                Socket masterOut = GetPushSocket(ctx);
                Bind(masterOut, MASTER_PORT_OUT);
                Console.WriteLine("Iniciando maestro.");
                using (var master = new PipeMaster(new SocketAdapter(masterOut), new SocketAdapter(masterIn)))
                {
                    PipeNode[] nodes = new PipeNode[NUM_NODOS];                    
                    Console.Write("Iniciando esclavos: ");
                    for (int i = 0; i < NUM_NODOS; i++)
                    {
                        Console.Write("{0} de {1}, ", i + 1, NUM_NODOS);
                        SocketAdapter sendRight = null;
                        SocketAdapter recvLeft = null;
                        SocketAdapter sendLeft = null;
                        SocketAdapter recvRight = null;

                        Socket sockInLeft = GetPullSocket(ctx);
                        Connect(sockInLeft, MASTER_PORT_OUT + i);
                        recvLeft = new SocketAdapter(sockInLeft);

                        Socket sockOutLeft = GetPushSocket(ctx);
                        Connect(sockOutLeft, MASTER_PORT_IN + i);
                        sendLeft = new SocketAdapter(sockOutLeft);

                        if (i < NUM_NODOS - 1)
                        {
                            Socket sockInRight = GetPullSocket(ctx);
                            Bind(sockInRight, MASTER_PORT_IN + i + 1);
                            recvRight = new SocketAdapter(sockInRight);

                            Socket sockOutRight = GetPushSocket(ctx);
                            Bind(sockOutRight, MASTER_PORT_OUT + i + 1);
                            sendRight = new SocketAdapter(sockOutRight);
                        }

                        PipeNode node = new PipeNode(sendRight, recvLeft, sendLeft, recvRight);
                        nodes[i] = node;
                    }

                    Console.WriteLine();
                    // Iniciar temporizador.
                    var stopWatch = Stopwatch.StartNew();

                    // Realizar ordenamiento.
                    Console.WriteLine("Ejecutando ordenamiento.");
                    int ncount = 0;
                    Debug.Write("Iniciando nodos...");
                    nodes.ToList().ForEach(node =>
                        {
                            node.Start(ncount++, NUM_NODOS);
                            Debug.Write(ncount + ", ");                            
                        });

                    Debug.WriteLine("");
                    int[] result = master.Sort(numeros);

                    // Imprimir tiempo de procesamiento.
                    stopWatch.Stop();
                    Console.WriteLine("Tiempo de cómputo: {0}", stopWatch.ElapsedMilliseconds);

                    // Imprimir números ordenados.
                    Console.Write("Resultado: ");
                    result.ToList().ForEach(n => Console.Write("{0}, ", n));
                    Console.WriteLine();

                    nodes.ToList().ForEach(node => ((IDisposable)node).Dispose());
                }                
            }                       
        }

        private static Socket GetPullSocket(Context ctx)
        {
            return ctx.Socket(SocketType.PULL);
        }

        private static Socket GetPushSocket(Context ctx)
        {
            return ctx.Socket(SocketType.PUSH);
        }

        private static void Connect(Socket socket, int port)
        {
            socket.Connect(Transport.TCP, "localhost", (uint)port);
        }

        private static void Bind(Socket socket, int port)
        {
            socket.Bind(Transport.TCP, "*", (uint)port);
        }
    }
}
