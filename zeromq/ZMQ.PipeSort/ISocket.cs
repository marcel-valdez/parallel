namespace ZMQ.PipeSort
{
    using System;

    public interface ISocket : IDisposable
    {
        SendStatus Send(byte[] message);

        byte[] Recv(int timeout);        
    }
}
