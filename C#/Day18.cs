
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace AdventOfCode
{
	public class Day18 : Day
	{
		public override void Solve()
		{
			bool[,] lamps = Animate(100, false);
			
			int turnedOnTotalLamps = 0;
			foreach(var lamp in lamps)
				if(lamp)
					turnedOnTotalLamps++;
			Console.WriteLine();
			Console.WriteLine("Turned on lamps 1: " + turnedOnTotalLamps);
			
			lamps = Animate(100, true);
			
			turnedOnTotalLamps = 0;
			foreach(var lamp in lamps)
				if(lamp)
					turnedOnTotalLamps++;
			Console.WriteLine();
			Console.WriteLine("Turned on lamps 2: " + turnedOnTotalLamps);
		}
		
		static bool[,] Animate(int ticks, bool isPart2)
		{
			var lamps = new bool[100, 100];
			
			string[] lines = File.ReadAllLines("Day18Input.txt");
			//string[] lines = {".#.#.#", "...##.", "#....#", "..#...", "#.#..#", "####.."};
			for(int x = 0; x < lines.Length; x++)
				for(int y = 0; y < lines[x].Length; y++)
					if(lines[x][y] == '#')
						lamps[x, y] = true;
			
			if(isPart2)
			{
				lamps[0, 0] = true;
				lamps[0, 99] = true;
				lamps[99, 0] = true;
				lamps[99, 99] = true;
			}
			
			for(int i = 0; i < ticks; i++)
			{
				var newLamps = new bool[100, 100];
				for(int x = 0; x < 100; x++)
				{
					for(int y = 0; y < 100; y++)
					{
						var neighbors = new List<bool>();
						for(int neighX = x - 1; neighX <= x + 1; neighX++)
							for(int neighY = y - 1; neighY <= y + 1; neighY++)
								if(!(neighX == x && neighY == y) && neighX >= 0 && neighX < 100 && neighY >= 0 && neighY < 100)
									neighbors.Add(lamps[neighX, neighY]);
						
						if(isPart2 && IsCorner(x, y))
							newLamps[x, y] = true;
						else
						{
							if(lamps[x, y])
								newLamps[x, y] = neighbors.Sum(lamp => lamp ? 1 : 0) == 2 || neighbors.Sum(lamp => lamp ? 1 : 0) == 3;
							else
								newLamps[x, y] = neighbors.Sum(lamp => lamp ? 1 : 0) == 3;
						}
					}
				}
				lamps = newLamps;
				Console.Write('.');
				//PrintLamps(lamps);
			}
			return lamps;
		}
		
		static bool IsCorner(int x, int y)
		{
			return (x == 0 && y == 0) || (x == 0 && y == 99) || (x == 99 && y == 0) || (x == 99 && y == 99);
		}
		
		static void PrintLamps(bool[,] lamps)
		{
			for(int x = 0; x < lamps.GetLength(0); x++)
			{
				for(int y = 0; y < lamps.GetLength(1); y++)
					Console.Write(lamps[x, y] ? '#' : '.');
				Console.WriteLine();
			}
			Console.WriteLine();
		}
	}
}
