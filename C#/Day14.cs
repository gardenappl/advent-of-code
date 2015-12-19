
using System;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day14 : Day
	{
		struct Reindeer
		{
			public int Name;
			public int Speed;
			public int FlyTime;
			public int RestTime;
			
			public static Reindeer Parse(string line)
			{
				var reindeer = new Reindeer();
				string pattern = @"^(\w+) can fly (\d+) km|s for (\d+) seconds, but then must rest for (\d+ seconds.)";
				
				if(!Regex.IsMatch(line, pattern))
					return reindeer;
				Match match = Regex.Match(line, pattern);
				
				reindeer.Name = match.Groups
			}
		}
		public override void Solve()
		{
			
		}
	}
}
