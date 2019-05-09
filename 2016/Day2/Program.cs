
using System;
using System.IO;

namespace Day2
{
	public class Program
	{
		static string[] Lines;
		
		public static void Main()
		{
			Lines = File.ReadAllLines("input.txt");
			string result1 = Solve(new string[]
			{
				"123",
				"456",
				"789"
			}, 3, 3);
			string result2 = Solve(new string[]
			{
				"  1  ",
				" 234 ",
				"56789",
				" ABC ",
				"  D  "
			}, 5, 5);
			Console.WriteLine();
			Console.WriteLine("Answer Part 1: " + result1);
			Console.WriteLine("Answer Part 2: " + result2);
			Console.ReadKey();
		}
		
		static string Solve(string[] grid, int width, int height)
		{
			width--;
			height--;
			int x = width / 2;
			int y = height / 2;
			string result = "";
			foreach(string line in Lines)
			{
				foreach(char c in line)
				{
					switch(c)
					{
						case 'U':
							if(y != 0 && grid[y - 1][x] != ' ')
								y--;
							break;
						case 'R':
							if(x != width && grid[y][x + 1] != ' ')
								x++;
							break;
						case 'D':
							if(y != height && grid[y + 1][x] != ' ')
								y++;
							break;
						case 'L':
							if(x != 0 && grid[y][x - 1] != ' ')
								x--;
							break;
					}
					Console.WriteLine("({0}, {1})", x, y);
				}
				Console.WriteLine("NEXT");
				result += grid[y][x];
			}
			return result;
		}
	}
}
