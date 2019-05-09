
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;

namespace AdventOfCode
{
	public class Day2 : Day
	{
		public override void Solve()
		{
			int totalPaper = 0;
			int totalRibbon = 0;
			using(var file = new StreamReader("Day2Input.txt"))
			{
				do
				{
					int X = 0;
					int Y = 0;
					int Z = 0;
					string[] dimensions = file.ReadLine().Split('x');
					try
					{
						X = Int32.Parse(dimensions[0]);
						Y = Int32.Parse(dimensions[1]);
						Z = Int32.Parse(dimensions[2]);
					}
					catch(Exception)
					{
						Console.WriteLine("Found an invalid box size. Somebody was messing with the input file!");
						Console.WriteLine("Whoever did it is totally going to the naughtly list this year...");
					}
					totalPaper += 2 * X * Y;
					totalPaper += 2 * Y * Z;
					totalPaper += 2 * X * Z;
					int[] sortedDimensions = Sort(X, Y, Z);
					totalPaper += sortedDimensions[0] * sortedDimensions[1]; //Area of smallest side
					
					totalRibbon += 2 * (sortedDimensions[0] + sortedDimensions[1]); //Perimeter of the smallest side
					totalRibbon += X * Y * Z;
				} while(!file.EndOfStream);
			}
			Console.WriteLine("Total paper: " + totalPaper);
			Console.WriteLine("Total ribbon: " + totalRibbon);
		}
		
		int[] Sort(params int[] ints)
		{
			var list = new List<int>(ints);
			list.Sort();
			return list.ToArray();
		}
	}
}
