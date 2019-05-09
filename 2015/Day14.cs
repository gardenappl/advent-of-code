
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day14 : Day
	{
		internal class Reindeer
		{
			public readonly string Name;
			public readonly int FlySpeed;
			public readonly int CanFlyTime;
			public readonly int NeedRestTime;
			public int Distance;
			public int Points;
			int ActionTime;
			bool IsResting;
			
			const string pattern = @"^(\w+) can fly (\d+) km/s for (\d+) seconds, but then must rest for (\d+) seconds.$";
			
			public Reindeer(string line)
			{
				Match match = Regex.Match(line, pattern);
				
				if(!match.Success)
					throw new ArgumentException("Reindeer descritption is invalid", "line");
				
				Name = match.Groups[1].Value;
				FlySpeed = Int32.Parse(match.Groups[2].Value);
				CanFlyTime = Int32.Parse(match.Groups[3].Value);
				NeedRestTime = Int32.Parse(match.Groups[4].Value);
			}
			
			public void Update()
			{
				ActionTime++;
				if(IsResting)
				{
					if(ActionTime == NeedRestTime)
					{
						ActionTime = 0;
						IsResting = false;
					}
				}
				else
				{
					Distance += FlySpeed;
					if(ActionTime == CanFlyTime)
					{
						ActionTime = 0;
						IsResting = true;
					}
				}
			}
			
			public override string ToString()
			{
				return string.Format("{0} has travelled {1} km and has been {2} for {3} seconds. Has {4} points.", Name, Distance, IsResting ? "resting" : "flying", ActionTime, Points);
			}
		}
		
		
		internal static List<Reindeer> reindeerList = new List<Reindeer>();
		
		public override void Solve()
		{
			foreach(string line in File.ReadAllLines("Day14Input.txt"))
				reindeerList.Add(new Reindeer(line));
			
			//reindeerList.Add(new Reindeer("Comet can fly 14 km/s for 10 seconds, but then must rest for 127 seconds."));
			//reindeerList.Add(new Reindeer("Dancer can fly 16 km/s for 11 seconds, but then must rest for 162 seconds."));
			
			for(int ticks = 1; ticks <= 2503; ticks++)
			{
				foreach(var reindeer in reindeerList)
					reindeer.Update();
				
				foreach(var reindeer in reindeerList)
					if(reindeer.Distance >= reindeerList.Max(deer => deer.Distance))
						reindeer.Points++;
			}
			
			Console.WriteLine("Fastest reindeer: " + reindeerList.Max(reindeer => reindeer.Distance));
			Console.WriteLine("Most points: " + reindeerList.Max(reindeer => reindeer.Points));
		}
	}
}
