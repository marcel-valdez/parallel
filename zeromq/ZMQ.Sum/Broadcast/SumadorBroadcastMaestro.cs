using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using ZMQ.ZMQExt;

namespace ZMQ.Sum.Broadcast
{
    public class SumadorBroadcastMaestro : Proceso<double>
    {
        private readonly ManualResetEvent semaphore;
        private readonly int[] numeros;
        private Context context;

        public SumadorBroadcastMaestro(int[] numeros, Context context, Socket sender, Socket receiver)
            : base(sender, receiver)
        {
            this.numeros = numeros;
            this.context = context;
            this.semaphore = new ManualResetEvent(true);
            this.SenalDeFinalizacion = new ManualResetEventSlim(false);
        }

        public override bool EjecutarOperacion()
        {
            // Primero se prepara el gather de números
            PollItem pollItem = this.Receiver.CreatePollItem(IOMultiPlex.POLLIN);
            pollItem.PollInHandler += ReceptorDeMensaje;
            PollItem[] pollArgs = new PollItem[] { pollItem };

            Debug.WriteLine("Master: Preparando 'polling'");
            Task.Factory.StartNew(() =>
            {
                while (RecepcionesRecibidas < RecepcionesEsperadas)
                {
                    Debug.WriteLine("Master: context.Poll(pollArgs);");
                    context.Poll(pollArgs);
                }

                Debug.WriteLine("Master: Senalizando porque {0} >= {1}", RecepcionesRecibidas, RecepcionesEsperadas);
                this.SenalDeFinalizacion.Set();
            });            

            // Broadcast de numeros
            Debug.WriteLine("Master: Sender.Send(int[{0}])", this.numeros.Length);
            Sender.Send(this.numeros);

            return true;
        }

        private void ReceptorDeMensaje(Socket socket, IOMultiPlex revents)
        {
            // Evita que se ejecute asíncronamente 2 veces. 
            // Se prefiere el AutoResetEvent que el lock, porque tiene menos overhead.            
            double numero = socket.Recv<double>();
            int recepciones = this.RecepcionesRecibidas;
            if (this.semaphore.WaitOne(2000))
            {
                this.Resultado += numero;
                this.RecepcionesRecibidas++;
                this.semaphore.Set();
                Debug.WriteLine("Master: Se recibió un número {0}, en la recepción #{1}", numero, recepciones);
            }
            else
            {
                Debug.WriteLine("Master: El semáforo hizo time-out!");
            }

        }

        public ManualResetEventSlim SenalDeFinalizacion
        {
            get;
            set;
        }

        /// <summary>
        /// Número de mensajes recibidos de clientes
        /// </summary>        
        public int RecepcionesRecibidas
        {
            get;
            private set;
        }

        /// <summary>
        /// Hagalo false, para finalizar la ejecución del algoritmo.
        /// </summary>   
        public int RecepcionesEsperadas
        {
            get;
            set;
        }
    }
}
