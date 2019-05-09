
using System;
using System.Collections.Generic;

namespace Day13
{
	public class Program
	{
		public static void Main(string[] args)
		{
			int num = 0;
			int targetX = 0;
			int targetY = 0;
			if(args.Length < 1 || !Int32.TryParse(args[0], out num))
			{
				Console.WriteLine("Specify the office designer's favourite number as first program argument.");
				Console.ReadKey();
				return;
			}
			if(args.Length < 2 || !Int32.TryParse(args[1], out targetX) ||
			   args.Length < 3 || !Int32.TryParse(args[2], out targetY))
			{
				Console.WriteLine("Specify the target X and Y as 2nd and 3rd program arguments.");
				Console.ReadKey();
				return;
			}
			
//			num = 10;
//			targetX = 7;
//			targetY = 4;
			
			Traverse(false, num, targetX, targetY);
			Traverse(true, num);
			Console.ReadKey();
		}
		
		static void Traverse(bool part2, int num, int targetX = 0, int targetY = 0)
		{
			//Tuple: x, y, steps
			var visited = new Dictionary<Tuple<int, int>, int>();
			var queue = new Queue<Tuple<int, int, int>>();
			if(!part2)
				queue.Enqueue(new Tuple<int, int, int>(targetX, targetY, 0));
			else
				queue.Enqueue(new Tuple<int, int, int>(1, 1, 0));
			
			while(queue.Count > 0)
			{
				var point = queue.Dequeue();
				var pointXY = new Tuple<int, int>(point.Item1, point.Item2);
				if(visited.ContainsKey(pointXY))
				{
					if(part2)
						continue;
					if(visited[pointXY] <= point.Item3)
						continue;
				}
				else
					visited.Add(pointXY, point.Item3);
				if(part2 && point.Item3 == 50)
					continue;
				
				if(!part2 && point.Item1 == 1 && point.Item2 == 1)
				{
					Console.WriteLine("Shortest path: " + point.Item3);
					return;
				}
				
				if(point.Item1 != 0 && !IsWall(point.Item1 - 1, point.Item2, num))
					queue.Enqueue(new Tuple<int, int, int>(point.Item1 - 1, point.Item2, point.Item3 + 1));
				
				if(!IsWall(point.Item1 + 1, point.Item2, num))
					queue.Enqueue(new Tuple<int, int, int>(point.Item1 + 1, point.Item2, point.Item3 + 1));
				
				if(point.Item2 != 0 && !IsWall(point.Item1, point.Item2 - 1, num))
					queue.Enqueue(new Tuple<int, int, int>(point.Item1, point.Item2 - 1, point.Item3 + 1));
				
				if(!IsWall(point.Item1, point.Item2 + 1, num))
					queue.Enqueue(new Tuple<int, int, int>(point.Item1, point.Item2 + 1, point.Item3 + 1));
			}
			
			if(part2)
				Console.WriteLine("Visited points with <=50 steps: " + visited.Count);
			else
				Console.WriteLine("Path not found!");
		}
		
		static bool IsWall(int x, int y, int number)
		{
			int num = x*x + 3*x + 2*x*y + y + y*y + number;
			int bits = 0;
			for(int i = 0; i < 32; i++)
			{
				int num2 = num >> i;
				if(num2 % 2 == 1)
					bits++;
			}
			return bits % 2 == 1;
		}
	}
}
