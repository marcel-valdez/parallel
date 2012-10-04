namespace ZMQ.Extensions.Test
{
    using System;
    using System.Linq;
    using NUnit.Framework;

    [TestFixture]
    public class ExtenionsTest
    {
        [Test]
        public void TestIfItCanConvertByteArrayToIntArray()
        {
            // Arrange
            int[] expected = { 1, 2, 4, 16, 32, 64, 128 };
            byte[] parameter =
                              BitConverter.GetBytes(expected[0]).Concat(
                              BitConverter.GetBytes(expected[1])).Concat(
                              BitConverter.GetBytes(expected[2])).Concat(
                              BitConverter.GetBytes(expected[3])).Concat(
                              BitConverter.GetBytes(expected[4])).Concat(
                              BitConverter.GetBytes(expected[5])).Concat(
                              BitConverter.GetBytes(expected[6])).ToArray();

            int[] actual = null;

            // Act
            actual = Common.Helper.ToIntArray(parameter);

            // Assert
            for(int index = 0; index < actual.Length; index++)
            {
                int number = actual[index];
                int expectedNumber = expected[index];
                Assert.That(number, Is.EqualTo(expectedNumber));
            }


            // Reset

        }
    }
}
