namespace ZMQ.Sum.SendRecv
{
    using System.Linq;
    using ZMQExt;

    public class SumadorEsclavo : Proceso<int>
    {
        public SumadorEsclavo(Socket sender, Socket receiver)
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

                // Calcular suma
                this.Result = numeros.Sum();

                // Regresar resultado al maestro                
                this.Sender.Send<int>(this.Result);
            }
            catch (Exception ex)
            {
                
                throw ex;
            }


            return false;
        }
    }
}
