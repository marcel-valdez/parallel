namespace ZMQ.PipeSort
{
    using System;
    using System.Diagnostics;
    using System.Threading.Tasks;

    public class PipeNode : IDisposable
    {
        private volatile int keptNumber = int.MinValue;
        private readonly ISocket senderToRight;
        private readonly ISocket receiverFromLeft;
        private readonly ISocket senderToLeft;
        private readonly ISocket receiverFromRight;

        public PipeNode(ISocket senderToRight, ISocket receiverFromLeft, ISocket senderToLeft, ISocket receiverFromRight)
        {
            this.senderToRight = senderToRight;
            this.receiverFromLeft = receiverFromLeft;
            this.senderToLeft = senderToLeft;
            this.receiverFromRight = receiverFromRight;
        }

        private static int count = 0;
        private int myCount = 0;
        private Task task;

        public void Start(int index, int amountOfNumbers)
        {
            myCount = count++;
            // En realidad se requieren 2 Poll Listener: Uno para escuchar continuamente de la izquierda, y otro para escuchar
            // continuamente de la derecha (PUSH PULL ambos)            
            this.task = Task.Factory.StartNew(() =>
            {
                SendForwards(index, amountOfNumbers);
                if (this.senderToRight != null)
                {
                    SendBackwards(index, amountOfNumbers);
                }
            });
        }

        private void SendForwards(int index, int amountOfNumbers)
        {
            var stopWatch = new Stopwatch();
            bool firstValueReceived = false;
            int loopCount = amountOfNumbers - index;
            while (loopCount-- > 0)
            {
                stopWatch.Restart();
                int numberRecv = this.receiverFromLeft.RecvInt(1000);
                stopWatch.Stop();
                if (stopWatch.ElapsedMilliseconds < 1000) // Si se recibió número
                {
                    Debug.WriteLine("El nodo {0} recibió el número {1}", this.myCount + 1, numberRecv);
                    int numberToSend = numberRecv;
                    if (numberRecv > keptNumber)
                    {
                        numberToSend = keptNumber;
                        keptNumber = numberRecv;
                    }

                    if (senderToRight != null) // Si este nodo no es el último
                    {
                        if (firstValueReceived)
                        {
                            Debug.WriteLine("El nodo {0} enviará el número {1} y se quedará con {2}",
                                this.myCount,
                                numberToSend,
                                this.keptNumber);
                            this.senderToRight.Send(numberToSend);
                        }
                        else
                        {
                            firstValueReceived = true;
                        }
                    }
                    else
                    {
                        Debug.WriteLine("El nodo {0} enviará el número {1} a la izquierda.", this.myCount, this.keptNumber);
                        this.senderToLeft.Send(this.keptNumber);
                    }
                }
            }
        }

        private void SendBackwards(int index, int amountOfNumbers)
        {
            bool sentMyNumber = false;
            var stopWatch = new Stopwatch();
            int loopCount = amountOfNumbers - index;
            while (loopCount-- > 0)
            {
                stopWatch.Restart();
                byte[] numberData = this.receiverFromRight.Recv(1000);
                stopWatch.Stop();
                if (stopWatch.ElapsedMilliseconds < 1000)  // Si se recibió un número
                {
                    if (!sentMyNumber) // Sólo la primera vez, se debe enviar el número guardado.
                    {
                        Debug.WriteLine("El nodo {0} enviara el número {1} a la izquierda", this.myCount, this.keptNumber);
                        this.senderToLeft.Send(this.keptNumber);
                        sentMyNumber = true;
                    }

                    Debug.WriteLine("El nodo {0} enviara el número {1} a la izquierda",
                        this.myCount,
                        BitConverter.ToInt32(numberData, 0));
                    this.senderToLeft.Send(numberData);
                }
            }
        }
        #region IDisposable Members

        void IDisposable.Dispose()
        {
            if (this.task != null)
            {
                this.task.Wait();
                this.task.Dispose();
                this.task = null;
            }

            if (senderToLeft != null)
                this.senderToLeft.Dispose();

            if (senderToRight != null)
                this.senderToRight.Dispose();

            if (receiverFromLeft != null)
                this.receiverFromLeft.Dispose();

            if (receiverFromRight != null)
                this.receiverFromRight.Dispose();
        }

        #endregion
    }
}
