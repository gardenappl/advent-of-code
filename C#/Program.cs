
using System;
using System.Collections.Generic;

namespace AdventOfCode
{
	public class Program
	{
		static readonly Day[] days = new Day[25];
		
		public static void Main()
		{
			days[1] = new Day1();
			days[2] = new Day2();
			days[3] = new Day3();
			days[4] = new Day4();
			days[5] = new Day5();
			
			int day = 0;
			while(day == 0)
			{
				Console.Write("Choose a day to solve: ");
				try
				{
					int input = Int32.Parse(Console.ReadLine());
					if(days[input] == null)
						Console.WriteLine("There is no solution for that day's puzzle yet! Sorry!");
					else
						day = input;
				}
				catch(Exception)
				{
					Console.WriteLine("Only numbers between 1 and 25 please!");
				}
			}
			
			DateTime startTime = DateTime.Now;
			try
			{
				days[day].Solve();
			}
			catch(Exception e)
			{
				Console.WriteLine(e);
			}
			Console.WriteLine("Solution took {0} milliseconds.", (DateTime.Now - startTime).Milliseconds);
			Console.WriteLine("Press any key to continue...");
			Console.ReadKey();
		}
	}
}
