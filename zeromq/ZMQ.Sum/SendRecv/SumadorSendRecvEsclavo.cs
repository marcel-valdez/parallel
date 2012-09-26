namespace ZMQ.Sum.SendRecv
{
    using System.Diagnostics;
    using System.Linq;
    using ZMQExt;

    public class SumadorSendRecvEsclavo : Proceso<double>
    {
        public SumadorSendRecvEsclavo(Socket sender, Socket receiver)
            : base(sender, receiver)
        {
        }

        public override bool EjecutarOperacion()
        {
            int[] numeros;

            try
            {
                // Obtener numeros                
                numeros = this.Receiver.Recv<int[]>();
                Debug.Assert(numeros.Length > 0, "Debe recibirse por lo menos 1 número del maestro.");
                Debug.WriteLine("Esclavo sumara: {0} números. Del {1} al {2}", numeros.Length, numeros[0], numeros[numeros.Length - 1]);

                // Calcular suma
                for (int i = 0; i < numeros.Length; i++)
                {
                    this.Resultado += numeros[i];
                }

                // Regresar resultado al maestro                
                Debug.WriteLine("Enviando resultado: this.Sender.Send({0})", this.Resultado);
                this.Sender.Send(this.Resultado);
            }
            catch (Exception ex)
            {                
                throw ex;
            }


            return false;
        }
    }
}
