
using System;
using System.Collections.Generic;
using System.IO;

namespace Day1
{
	public class Program
	{
		public static void Main()
		{
			string input = File.ReadAllText("input.txt");
			string[] instructions = input.Split(new []{", "}, StringSplitOptions.None);
			
			int x = 0;
			int y = 0;
			var visited = new List<Tuple<int, int>>();
			Tuple<int, int> visitedTwice = null;
			
			var direction = Direction.North;
			foreach(string instruction in instructions)
			{
				if(instruction[0] == 'L')
				{
					if(direction == Direction.North)
						direction = Direction.West;
					else
						direction--;
				}
				else
				{
					if(direction == Direction.West)
						direction = Direction.North;
					else
						direction++;
				}
				int num = Int32.Parse(instruction.Substring(1));
				if(visitedTwice != null)
				{
					switch(direction)
					{
						case Direction.North:
							y += num;
							break;
						case Direction.East:
							x += num;
							break;
						case Direction.South:
							y -= num;
							break;
						case Direction.West:
							x -= num;
							break;
					}
				}
				else
				{
					for(int i = 0; i < num; i++)
					{
						switch(direction)
						{
							case Direction.North:
								y++;
								break;
							case Direction.East:
								x++;
								break;
							case Direction.South:
								y--;
								break;
							case Direction.West:
								x--;
								break;
						}
						var point = new Tuple<int, int>(x, y);
						if(!visited.Contains(point))
							visited.Add(point);
						else
							visitedTwice = point;
					}
				}
				Console.WriteLine(direction + " " + instruction.Substring(1) + " X: " + x + " Y: " + y);
			}
			Console.WriteLine();
			Console.WriteLine("Answer Part 1: " + (Math.Abs(x) + Math.Abs(y)));
			Console.WriteLine("Answer Part 2: " + (Math.Abs(visitedTwice.Item1) + Math.Abs(visitedTwice.Item2)));
			Console.ReadKey();
		}
		
		enum Direction : byte
		{
			North,
			East,
			South,
			West
		}
		
//		struct Point
//		{
//			int X,
//			int Y;
//
//			public Point(int x, int y)
//			{
//				X = x;
//				Y = y;
//			}
//		}
	}
}
