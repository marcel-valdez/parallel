using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZMQ.Sum
{
    /// <summary>
    /// Representa la clase abstracta que deben implementar cada suma.
    /// </summary>
    public abstract class SumSlave
    {
        public int Sum(int a, int b)
        {
            return a + b;
        }

        /// <summary>
        /// Envía el resultado al proceso indicado, debe variar con cada implementación.
        /// </summary>
        /// <param name="process">The process.</param>
        /// <param name="result">The result.</param>
        public abstract void SendToMaster(int process, int result);
    }
}
