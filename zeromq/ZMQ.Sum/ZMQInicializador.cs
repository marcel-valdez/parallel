using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZMQ.Sum
{
    public abstract class ZMQInicializador : IDisposable
    {

        private Context contexto;
        private Socket sender;
        private Socket receiver;
        public ZMQInicializador(int numeroDeProcesos)
        {
            this.contexto = new Context(numeroDeProcesos);

        }

        /// <summary>
        /// Inizializa ZMQ
        /// </summary>
        public void Inicializar(string direccion, Comunicacion tipo)
        {
            SocketType type = SocketType.REQ;
            switch (tipo)
            {
                case Comunicacion.SendRecv:
                    {
                        type = SocketType.REQ;
                        var socket = this.contexto.Socket(type);
                        this.sender = socket;
                        this.receiver = socket;
                        this.sender.Connect(direccion);
                    }
                    break;
                case Comunicacion.BroadcastRecv:
                    throw new NotImplementedException();
                    break;
                case Comunicacion.ScatterReduce:
                    throw new NotImplementedException();
                    break;
            }
        }

        public Socket Sender
        {
            get
            {
                return this.sender;
            }
        }

        public Socket Receiver
        {
            get
            {
                return this.receiver;
            }
        }

        #region IDisposable Members
        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            this.sender.Dispose();
            this.contexto.Dispose();
        }
        #endregion
    }
}
