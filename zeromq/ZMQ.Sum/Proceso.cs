using System;

namespace ZMQ
{

    public abstract class Proceso<T> : IDisposable
    {
        private readonly Socket sender;
        private readonly Socket receiver;

        public Proceso(Socket sender, Socket receiver)
        {
            this.sender = sender;
            this.receiver = receiver;
        }

        /// <summary>
        /// Inicia la sumatoria, preparando el ambiente ZMQ
        /// </summary>
        public abstract bool EjecutarOperacion();

        protected Socket Sender
        {
            get
            {
                return this.sender;
            }
        }

        protected Socket Receiver
        {
            get
            {
                return this.receiver;
            }
        }

        public T Result
        {
            get;
            protected set;
        }
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (sender != null)
                    sender.Dispose();
                if (sender != receiver && receiver != null)
                    receiver.Dispose();
            }
        }

        ~Proceso()
        {
            Dispose(false);
        }
    }
}
