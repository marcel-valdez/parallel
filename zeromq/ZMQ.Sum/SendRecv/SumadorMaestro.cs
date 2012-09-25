using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using ZMQ.ZMQExt;

namespace ZMQ.Sum.SendRecv
{
    public class SumadorMaestro : Proceso<int>
    {
        private readonly int numeroDeEsclavos = 0;
        private readonly int[] numeros;

        public SumadorMaestro(int numeroDeEsclavos, int[] numeros, Socket sender, Socket receiver)
            : base(sender, receiver)
        {
            this.numeroDeEsclavos = numeroDeEsclavos;
            this.numeros = numeros;
        }

        
        public override bool EjecutarOperacion()
        {
            int numerosPorEsclavo = numeros.Length / numeroDeEsclavos;
            int remanente = numeros.Length % numeroDeEsclavos;
            // Mapear datos
            Console.WriteLine("Mapeando datos a {0} esclavos", numeroDeEsclavos);
            this.EnviarDatos(numerosPorEsclavo, remanente);
            // Recibir resultados
            // Guardar resultados conglomerados
            Console.WriteLine("Recibiendo datos de {0} esclavos.", numeroDeEsclavos);
            this.RecibirDatos();
            
            return true;
        }

        private void EnviarDatos(int numerosPorEsclavo, int remanente)
        {
            IEnumerable<int> current = numeros;
            
            for (int indiceDeEsclavo = 0; indiceDeEsclavo < numeroDeEsclavos; indiceDeEsclavo++)
            {
                int cantidadAEnviar = numerosPorEsclavo;

                if (indiceDeEsclavo == numeroDeEsclavos - 1)
                {
                    cantidadAEnviar += remanente;
                }
                
                current = current.Skip(numerosPorEsclavo);
                int[] datos = current.Take(cantidadAEnviar)                                      
                                      .ToArray();                

                this.Sender.Send(datos);
                Thread.Sleep(10);
            }
        }

        private void RecibirDatos()
        {
            for (
                int cuentaDeEsclavos = 0;
                cuentaDeEsclavos < this.numeroDeEsclavos; 
                cuentaDeEsclavos++)
            {
                this.Result += this.Receiver.Recv<int>();
            }
        }
    }
}
