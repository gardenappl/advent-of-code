
using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace AdventOfCode
{
	public class Program
	{
		static readonly Day[] days = new Day[26];
		
		public static void Main()
		{
			days[1] = new Day1();
			days[2] = new Day2();
			days[3] = new Day3();
			days[4] = new Day4();
			days[5] = new Day5();
			days[6] = new Day6();
			days[7] = new Day7();
			days[8] = new Day8();
			days[9] = new Day9();
			days[10] = new Day10();
			days[11] = new Day11();
			days[12] = new Day12();
			days[13] = new Day13();
			days[14] = new Day14();
			days[15] = new Day15();
			days[16] = new Day16();
			days[17] = new Day17();
			days[18] = new Day18();
			days[19] = new Day19();
			days[20] = new Day20();
			days[21] = new Day21();
			days[22] = new Day22();
			days[23] = new Day23();
			days[24] = new Day24();
			days[25] = new Day25();
			
			for(int i = 1; i <= 25; i++) //A little thing that shows which days have solutions
			{
				if(days[i] != null)
					Console.ForegroundColor = ConsoleColor.Green;
				else
					Console.ForegroundColor = ConsoleColor.Red;
				Console.Write(i);
				Console.ForegroundColor = ConsoleColor.Gray;
				if(i != 25)
					Console.Write("|");
			}
			Console.WriteLine();
			
			int day = 0;
			while(days[day] == null)
			{
				Console.Write("Choose a day to solve: ");
				string input = Console.ReadLine();
				
				if(!Int32.TryParse(input, out day) || day < 1 || day > 25)
				{
					day = 0;
					Console.WriteLine("Only numbers between 1 and 25 please!");
				}
				else if(days[day] == null)
					Console.WriteLine("There is no solution for that day's puzzle yet! Sorry!");
			}
			
			Console.ForegroundColor = ConsoleColor.Cyan;
			bool success = true;
			var watch = Stopwatch.StartNew();
			try
			{
				days[day].Solve();
			}
			catch(Exception e)
			{
				success = false;
				Console.WriteLine(e);
			}
			watch.Stop();
			Console.ForegroundColor = ConsoleColor.Gray;
			if(success)
				Console.WriteLine("Solution took {0} milliseconds.", watch.ElapsedMilliseconds);
			else
				Console.WriteLine("Things crashed after running for {0} milliseconds!", watch.ElapsedMilliseconds);
			Console.WriteLine("Press any key to continue...");
			Console.ReadKey();
		}
		
	}
}
