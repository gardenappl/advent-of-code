
using System;
using System.IO;

namespace AdventOfCode
{
	public class Day1 : Day
	{
		public override void Solve()
		{
			int floor = 0;
			int basementFloorIndex = 0;
			
			using(var file = new StreamReader("Day1Input.txt"))
			{
				for(int i = 0; !file.EndOfStream; i++)
				{
					char c = (char)file.Read();
					if(c == '(')
						floor++;
					else if(c == ')')
						floor--;
					else
					{
						Console.WriteLine("Found an invalid character. Somebody was messing with the input file!");
						Console.WriteLine("Whoever did it is totally going to the naughtly list this year...");
					}
					if(floor == -1 && basementFloorIndex == 0)
						basementFloorIndex = i + 1;
				}
				
				Console.WriteLine("Result 1: {0}", floor);
				Console.WriteLine("Result 2: {0}", basementFloorIndex);
			}
		}
	}
}