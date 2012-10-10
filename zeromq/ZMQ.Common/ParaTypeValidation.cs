namespace ZMQ.Common.Extensions
{
    using System;
    using System.ComponentModel;
    using System.Diagnostics.Contracts;
    using System.Linq;

    /// <summary>
    /// Esta clase se encarga de validar parámetros en formato string[]
    /// </summary>
    public static class ParamTypeValidation
    {
        
        /// <summary>
        /// Esta operación se encarga validar los parámetros en string[] args, a los tipos Type[] 
        /// respectivamente, y regresa un object[] con los valores validar. 
        /// </summary>
        /// <param name="args">Son los valores string a intentar validar que sean de los tipos recibidos.</param>
        /// <param name="types">Son los tipos de valores esperados de args.</param>
        /// <returns>Si el arreglo string[]
        /// no está en formato correcto, se regresa false; de lo contrario se regresa true.</returns>
        public static bool Validate(this string[] args, params Type[] types)
        {
            Contract.Requires(args != null, "args is null.");
            Contract.Requires(types != null, "types is null.");

            if (args.Length != types.Length)
            {
                return false;
            }

            foreach (int index in Enumerable.Range(0, args.Length))
            {
                TypeConverter converter = TypeDescriptor.GetConverter(types[index]);
                if (!converter.CanConvertFrom(typeof(string)) || !converter.IsValid(args[index]))
                {
                    return false;
                }
            }

            return true;
        }

        
        /// <summary>
        /// Esta operación se encarga validar los parámetros en string[] args, al tipo T
        /// </summary>
        /// <typeparam name="T">El tipo genérico de argumento esperado</typeparam>
        /// <param name="args">Son los valores string a intentar validar que sean de los tipos recibidos</param>
        /// <returns>Si el arreglo string[] no está en formato correcto, se regresa false; de lo contrario se regresa true.</returns>
        public static bool Validate<T>(this string[] args)
        {
            Contract.Requires(args != null, "args is null");
            return ParaTypeValidation.Validate(args, typeof(T));
        }

        
        /// <summary>
        /// Esta operación se encarga validar los parámetros en string[] args, al tipo T1 y T2 respectivamente.
        /// </summary>
        /// <typeparam name="T1">El tipo genérico #1 de argumento esperado.</typeparam>
        /// <typeparam name="T2">El tipo genérico #2 de argumento esperado.</typeparam>
        /// <param name="args">Son los valores string a intentar validar que sean de los tipos recibidos</param>
        /// <returns>
        /// Si el arreglo string[] no está en formato correcto, se regresa false; de lo contrario se regresa true.
        /// </returns>
        public static bool Validate<T1, T2>(this string[] args)
        {
            Contract.Requires(args != null, "args is null");
            return ParaTypeValidation.Validate(args, typeof(T1), typeof(T2));
        }
    }
}
