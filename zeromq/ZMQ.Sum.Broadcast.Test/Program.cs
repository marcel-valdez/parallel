namespace ZMQ.Sum.Broadcast.Test
{
    using System;
    using System.Collections.Generic;
    using System.Reflection;
    using NUnit.ConsoleRunner;

    class Program
    {
        [STAThread]
        public static void Main(string[] args)
        {
            var argList = new List<string>();
            argList.Add(Assembly.GetExecutingAssembly().Location);
            argList.AddRange(args);
            
            Runner.Main(argList.ToArray());
        }
    }
}
