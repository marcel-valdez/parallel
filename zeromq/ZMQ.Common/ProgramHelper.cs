namespace ZMQ.Common
{
    using System;
    using System.Collections.Generic;
    using Extensions;
    using System.Linq;
    using System.Reflection;

    /// <summary>
    /// Class ProgramHelper
    /// </summary>
    public static class ProgramHelper
    {
        private const string USO_INCORRECTO_ERROR = "El uso correcto del programa es:\n {0} [numero de esclavos] \n";
        private const string NUMERO_ESCLAVOS_ERROR = "Por lo menos tienes que indicar {0} minimo, máximo {1}.\n";
        public const int PUERTO_ENVIO_MASTER = 13578;
        public const int PUERTO_RECEPCION_ESCLAVO = PUERTO_ENVIO_MASTER;
        public const int PUERTO_ENVIO_ESCLAVO = 13577;
        public const int PUERTO_RECEPCION_MASTER = PUERTO_ENVIO_ESCLAVO;
        public static string VerificarArgumentos(
            this IEnumerable<string> args,
            string nombreDePrograma,
            out int numeroDeEsclavos,
            int minimo,
            int maximo,
            string usoDelPrograma)
        {
            return VerificarArgumentos(args.ToArray(),
                       nombreDePrograma,
                       out numeroDeEsclavos,
                       minimo,
                       maximo,
                       usoDelPrograma);
        }

        public static string VerificarArgumentos(this string[] args,
            string nombreDePrograma,
            out int numeroDeEsclavos,
            int minimo = 1,
            int maximo = 8,
            string usoDelPrograma = USO_INCORRECTO_ERROR)
        {
            if (!args.Validate<int>())
            {
                numeroDeEsclavos = -1;

                return string.Format(usoDelPrograma, nombreDePrograma);
            }

            args.Parse<int>(out numeroDeEsclavos);

            if (numeroDeEsclavos < minimo || numeroDeEsclavos > maximo)
            {
                return USO_INCORRECTO_ERROR + string.Format(NUMERO_ESCLAVOS_ERROR, minimo, maximo);
            }

            return string.Empty;
        }

        /// <summary>
        /// Muestra el error msj, si msj está vacío, no envía nada y regresa false
        /// </summary>
        /// <param name="msj">El mensaje.</param>
        public static bool MostrarError(this string msj)
        {
            if (!string.IsNullOrEmpty(msj))
            {
                Console.WriteLine(msj);
                return true;
            }

            return false;
        }


        /// <summary>
        /// Parsea los parametros del programa maestro.
        /// </summary>
        /// <param name="args">The args.</param>
        /// <param name="numeroDeEsclavos">The numero de esclavos.</param>
        /// <param name="cantidadDeNumeros">The cantidad de numeros.</param>
        /// <returns>true si hubo error en los argumentos, false de otra manera. </returns>
        public static bool ParsearParametrosMaestro(string[] args, out int numeroDeEsclavos, out int cantidadDeNumeros, string usoDelPrograma)
        {
            cantidadDeNumeros = 0;
            bool shouldReturn = false;
            numeroDeEsclavos = 0;
            string nombrePrograma = Assembly.GetExecutingAssembly().GetName().Name;

            string mensajeDeError = args.Take(1).VerificarArgumentos(nombrePrograma, out numeroDeEsclavos, 1, 8, usoDelPrograma: usoDelPrograma);

            if (mensajeDeError.MostrarError())
            {
                Console.ReadKey();
                shouldReturn = true;
                return shouldReturn;
            }

            cantidadDeNumeros = 1000;
            if (args.Length > 1)
            {
                mensajeDeError = args.Skip(1).VerificarArgumentos(
                        nombrePrograma,
                        out cantidadDeNumeros,
                        8,
                        int.MaxValue,
                        usoDelPrograma);

                if (mensajeDeError.MostrarError())
                {
                    Console.ReadKey();
                    shouldReturn = true;
                    return shouldReturn;
                }
            }

            return shouldReturn;
        }
    }
}
