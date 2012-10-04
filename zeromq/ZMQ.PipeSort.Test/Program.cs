using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System;
using System.Collections.Generic;
using System.Reflection;
using NUnit.ConsoleRunner;
namespace ZMQ.PipeSort.Test
{
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
