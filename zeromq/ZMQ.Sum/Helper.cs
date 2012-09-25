using System;

namespace ZMQ.Extensions
{
    public static class Helper
    {
        public static int[] ToIntArray(this byte[] data)
        {
            int[] numeros;
            int tamano = data.Length / 4;
            numeros = new int[tamano];
            for (int i = 0; i < tamano; i++)
            {
                numeros[i] = BitConverter.ToInt32(data, i * 4);
            }

            return numeros;
        }
    }
}
