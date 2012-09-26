namespace ZMQ.Sum.Broadcast
{
    using System.Diagnostics;
    using ZMQExt;

    public class SumadorBroadcastEsclavo : Proceso<double>
    {
        private readonly int numeroDeEsclavos;
        private readonly int miIndice;

        public SumadorBroadcastEsclavo(int miIndice, int numeroDeEsclavos, Socket sender, Socket receiver)
            : base(sender, receiver)
        {
            this.miIndice = miIndice;
            this.numeroDeEsclavos = numeroDeEsclavos;    
        }

        public override bool EjecutarOperacion()
        {
            // Recibir datos de servidor
            Debug.WriteLine("Proc: {0}, Receiver.Recv<int[]>()... Esperando los datos.", this.miIndice);
            int[] datos = Receiver.Recv<int[]>();
            Debug.WriteLine("Proc: {0}, se recibieron int[{1}] datos.", this.miIndice, datos.Length);
            
            // Obtener tamano de slice
            int tamanoSlice = datos.Length / numeroDeEsclavos;
            // Establecer inicio de slice
            int inicio = miIndice * tamanoSlice;            
            int remanente = miIndice == numeroDeEsclavos - 1 ? datos.Length % numeroDeEsclavos : 0;
            // Establecer fin de slice
            int fin = inicio + tamanoSlice + remanente;

            // Calcular mi suma en base al slice
            Debug.WriteLine("Proc: {2}, for (int i = {0}; i < {1}; i++)", inicio, fin, this.miIndice);
            for (int i = inicio; i < fin; i++)
            {
                this.Resultado += datos[i];
            }

            // Enviar resultados al servidor
            Debug.WriteLine("Proc: {0}, Sender.Send({1})", this.miIndice, this.Resultado);
            Sender.Send(this.Resultado);

            return true;
        }
    }
}
