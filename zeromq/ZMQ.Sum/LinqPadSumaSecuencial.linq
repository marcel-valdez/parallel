<Query Kind="Program" />

public const int CIEN = 100;
public const int MIL = 1000;
public const int CIENMIL = 100000;
public const int UNMILLON = 1000000;
public const int DIEZMILLONES = 10000000;
public const int CIENMILLONES = 100000000;

public static readonly int[] CANTIDADES = new int[] { CIEN, MIL, CIENMIL, UNMILLON, DIEZMILLONES, CIENMILLONES };
public static readonly double[] RESULTADOS_ESPERADOS = new double[] 
{ 
	(Math.Pow(CIEN, 2) + CIEN)/2.0, 
	(Math.Pow(MIL, 2) + MIL)/2.0, 
	(Math.Pow(CIENMIL, 2) + CIENMIL)/2.0, 
	(Math.Pow(UNMILLON, 2) + UNMILLON)/2.0, 
	(Math.Pow(DIEZMILLONES, 2) + DIEZMILLONES)/2.0,
	(Math.Pow(CIENMILLONES, 2) + CIENMILLONES)/2.0 
};

void Main()
{		
	for(int i = 0; i < CANTIDADES.Length; i++)
	{
		var stopWatch = Stopwatch.StartNew();
		double result = 0;
		for (int j = 1; j <= CANTIDADES[i]; j++)
		{
			result += j;
		}
		
		stopWatch.Stop();
		//RESULTADOS_ESPERADOS[i].Dump("Resultado Esperado");
		(string.Format("Resultado: {0}\n", result) +
		string.Format("Milisegundos: {0}, Ticks: {1}\n", 
					  stopWatch.ElapsedMilliseconds, stopWatch.ElapsedTicks))
			  .Dump(string.Format("Ejecución Secuencial de Suma del 1 al {0}", CANTIDADES[i]));
	}	
}

// Define other methods and classes here
