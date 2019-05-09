
using System;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;

namespace Day8
{
	public class Program
	{
		public static void Main()
		{
			bool[,] screen = new bool[50, 6];
			
			var patternRect = new Regex(@"rect (\d+)x(\d+)");
			var patternRotateRow = new Regex(@"rotate row y=(\d+) by (\d+)");
			var patternRotateColumn = new Regex(@"rotate column x=(\d+) by (\d+)");
			using(var reader = new StreamReader("input.txt"))
			{
				while(!reader.EndOfStream)
				{
					string line = reader.ReadLine();
					if(patternRect.IsMatch(line))
					{
						var match = patternRect.Match(line);
						int width = Int32.Parse(match.Groups[1].Value);
						int height = Int32.Parse(match.Groups[2].Value);
						for(int x = 0; x < width; x++)
							for(int y = 0; y < height; y++)
								screen[x, y] = true;
					}
					if(patternRotateRow.IsMatch(line))
					{
						var match = patternRotateRow.Match(line);
						int y = Int32.Parse(match.Groups[1].Value);
						int shift = Int32.Parse(match.Groups[2].Value);
						bool[,] newScreen = screen.Copy();
						for(int x = 0; x < 50; x++)
							newScreen[(x + shift) % 50, y] = screen[x, y];
						screen = newScreen;
					}
					if(patternRotateColumn.IsMatch(line))
					{
						var match = patternRotateColumn.Match(line);
						int x = Int32.Parse(match.Groups[1].Value);
						int shift = Int32.Parse(match.Groups[2].Value);
						bool[,] newScreen = screen.Copy();
						for(int y = 0; y < 6; y++)
							newScreen[x, (y + shift) % 6] = screen[x, y];
						screen = newScreen;
					}
					Console.SetCursorPosition(0, 0);
					for(int y = 0; y < 6; y++)
					{
						for(int x = 0; x < 50; x++)
							Console.Write(screen[x, y] ? '█' : '▒');
						Console.WriteLine();
					}
//					Console.ReadKey();
				}
			}
			int count = 0;
			foreach(bool pixel in screen)
				if(pixel)
					count++;
			Console.WriteLine(count);
			
			
			Console.ReadKey();
		}
	}
	
	static class Extensions
	{
		public static T[,] Copy<T>(this T[,] array)
		{
			T[,] copy = new T[array.GetLength(0), array.GetLength(1)];
			for(int x = 0; x < array.GetLength(0); x++)
				for(int y = 0; y < array.GetLength(1); y++)
					copy[x, y] = array[x, y];
			return copy;
		}
	}
}
