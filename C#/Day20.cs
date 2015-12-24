
using System;
using System.Collections.Generic;
using System.IO;

namespace AdventOfCode
{
	public class Day20 : Day
	{
		public override void Solve()
		{
			int minPresents = Int32.Parse(File.ReadAllText("Day20Input.txt"));
			int maxElves = minPresents / 10;
			int[] presents = new int[maxElves];
			
			for(int elf = 1; elf < maxElves; elf++)
				for(int house = 0; house < presents.Length; house += elf)
					if(house != 0) //Every elf delivers to house 0 which causes integer overflow.
						presents[house] += elf * 10;
			
			Console.WriteLine("============ Part 1 ============");
			Console.WriteLine("{0} elves succesfully delivered presents to {0} houses...", maxElves);
			Console.WriteLine("Now looking for house with {0} presents...", minPresents);
			Console.WriteLine();
			
			for(int house = 1; house < presents.Length; house++)
			{
				if(presents[house] >= minPresents)
				{
					Console.WriteLine("House {0} got {1} presents!", house, presents[house]);
					break;
				}
			}
			
			
			maxElves = minPresents / 11;
			presents = new int[maxElves];
			
			for(int elf = 1; elf < maxElves; elf++)
				for(int house = 0; house <= 50 * elf && house < presents.Length; house += elf)
					if(house != 0)
						presents[house] += elf * 11;
			
			Console.WriteLine("============ Part 2 ============");
			Console.WriteLine("{0} elves succesfully delivered presents to {0} houses...", maxElves);
			Console.WriteLine("Now looking for house with {0} presents...", minPresents);
			Console.WriteLine();
			
			bool found = false;
			for(int house = 1; house < presents.Length; house++)
			{
				if(presents[house] >= minPresents)
				{
					Console.WriteLine("House {0} got {1} presents!", house, presents[house]);
					found = true;
					break;
				}
			}
			if(!found)
				Console.WriteLine("House not found!");
		}
	}
}
