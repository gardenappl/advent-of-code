using System;
using System.IO;
using System.Linq;

namespace Day3
{
	public class Program
	{
		public static void Main()
		{
			string[] lines = File.ReadAllLines("input.txt");
			int result1 = 0;
			int result2 = 0;
			
			foreach(string triangle in lines)
			{
				var sides =
					(from side in triangle.Split(new []{' '}, StringSplitOptions.RemoveEmptyEntries)
					 select Int32.Parse(side)).ToArray();
				
//				if(sides.Max() < sides.Sum() - sides.Max())
//					result1++;
				if(sides[0] < sides[1] + sides[2] &&
				   sides[1] < sides[0] + sides[2] &&
				   sides[2] < sides[0] + sides[1])
					result1++;
				else
					Console.WriteLine("{0}, {1}, {2} is invalid!", sides[0], sides[1], sides[2]);
			}
			
			
			int i = 0;
			int[][] rows = new int[3][];
			foreach(string row in lines)
			{
				rows[i] = (from side in row.Split(new []{' '}, StringSplitOptions.RemoveEmptyEntries)
				           select Int32.Parse(side)).ToArray();
				i++;
				if(i == 3)
				{
					for(int x = 0; x < 3; x++)
						if(rows[0][x] < rows[1][x] + rows[2][x] &&
						   rows[1][x] < rows[0][x] + rows[2][x] &&
						   rows[2][x] < rows[0][x] + rows[1][x])
							result2++;
						else
							Console.WriteLine("{0}, {1}, {2} is invalid!", rows[0][x], rows[1][x], rows[2][x]);
					i = 0;
					rows = new int[3][];
				}
			}
			Console.WriteLine("Answer Part 1: " + result1);
			Console.WriteLine("Answer Part 2: " + result2);
			Console.ReadKey();
		}
	}
}