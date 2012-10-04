using System;

namespace ZMQ.Common
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

        public static byte[] ToByteArray(this int[] data)
        {
            byte[] result = new byte[data.Length * 4];
            for(int i = 0, j = 0; i < data.Length; i++, j+= 4) {
                byte[] numberData = BitConverter.GetBytes(data[i]);
                result[j] = numberData[0];
                result[j + 1] = numberData[1];
                result[j + 2] = numberData[2];
                result[j + 3] = numberData[3];
            }

            return result;
        }
    }
}
