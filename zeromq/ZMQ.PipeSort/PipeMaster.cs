namespace ZMQ.PipeSort
{
    using System;
    using System.Diagnostics;

    public class PipeMaster : IDisposable
    {
        private readonly ISocket sendSocket;
        private readonly ISocket recvSocket;

        public PipeMaster(ISocket sendSocket, ISocket recvSocket)
        {
            this.sendSocket = sendSocket;
            this.recvSocket = recvSocket;
        }

        public int[] Sort(int[] input)
        {

            Debug.Write("Maestro: Enviando números: ");
            foreach (int number in input)
            {
                Debug.Write(number + ", ");
                this.sendSocket.Send(number);
            }

            Debug.WriteLine("");


            int[] result = new int[input.Length];
            var stopWatch = new Stopwatch();
            Debug.WriteLine("Maestro: Iniciando recepción de números.");
            for (int i = 0; i < input.Length; i++)
            {
                stopWatch.Restart();
                result[i] = this.recvSocket.RecvInt(10000);
                stopWatch.Stop();
                Debug.WriteLine("Maestro: Recepción de {0}", result[i]);
                if (stopWatch.ElapsedMilliseconds >= 100000)
                {
                    throw new TimeoutException("La operación duró más de 10 segundos, hubo un error de comunicación.");
                }
            }            

            return result;
        }

        #region IDisposable Members

        void IDisposable.Dispose()
        {
            ((IDisposable)this.sendSocket).Dispose();
            ((IDisposable)this.recvSocket).Dispose();
        }
        #endregion
    }
}
