
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Day6
{
	public class Program
	{
		public static void Main()
		{
			var frequencies = new Dictionary<char, int>[8];
			for(int i = 0; i < 8; i++)
				frequencies[i] = new Dictionary<char, int>();
			using(var reader = new StreamReader("input.txt"))
			{
				while(!reader.EndOfStream)
				{
					string line = reader.ReadLine();
					for(int i = 0; i < line.Length; i++)
					{
						if(frequencies[i].ContainsKey(line[i]))
							frequencies[i][line[i]]++;
						else
							frequencies[i].Add(line[i], 1);
					}
				}
			}
			Console.Write("Answer Part 1: ");
			foreach(var freq in frequencies)
				Console.Write(freq.OrderByDescending(kvp => kvp.Value).First().Key);
			Console.Write("\nAnswer Part 2: ");
			foreach(var freq in frequencies)
				Console.Write(freq.OrderBy(kvp => kvp.Value).First().Key);
			Console.ReadKey();
		}
	}
}
