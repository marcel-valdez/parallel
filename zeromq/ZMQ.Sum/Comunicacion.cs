using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZMQ.Sum
{
    public enum Comunicacion
    {
        SendRecv,
        BroadcastRecv,
        ScatterReduce
    }
}
