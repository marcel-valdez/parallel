namespace ZMQ.PipeSort
{
    using System;
    class SocketAdapter : ISocket
    {
        private readonly Socket socket;

        public SocketAdapter(Socket socket)
        {
            this.socket = socket;
        }

        #region IDisposable Members

        void IDisposable.Dispose()
        {
            this.socket.Dispose();
        }

        #endregion

        #region ISocket Members

        public SendStatus Send(byte[] message)
        {
            return this.socket.Send(message);
        }

        public byte[] Recv(int timeout)
        {
            return this.socket.Recv(timeout);
        }
        #endregion
    }
}
