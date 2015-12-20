
using System;
using System.IO;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day6 : Day
	{
		public static bool isPart2;
		
		struct Lamp
		{
			public int Brightness{ get; private set; }
			
			public bool TurnedOn{ get; private set; }
			
			public void TurnOn()
			{
				if(!isPart2)
					TurnedOn = true;
				else
					Brightness++;
			}
			public void TurnOff()
			{
				if(!isPart2)
					TurnedOn = false;
				else if(Brightness != 0)
						Brightness--;
			}
			public void Toggle()
			{
				if(!isPart2) 
					TurnedOn = !TurnedOn;
				else
					Brightness += 2;
			}
		}
		
		public override void Solve()
		{
			var lamps = FollowInstructions();
			
			int turnedOnLamps = 0;
			foreach(var lamp in lamps)
				if(lamp.TurnedOn)
					turnedOnLamps++;
			Console.WriteLine("Turned on lamps: " + turnedOnLamps);
			
			isPart2 = true;
			
			lamps = FollowInstructions();
			
			int totalBrightness = 0;
			foreach(var lamp in lamps)
				totalBrightness += lamp.Brightness;
			Console.WriteLine("Total brightness: " + totalBrightness);
		}
		
		Lamp[,] FollowInstructions()
		{
			var lamps = new Lamp[1000, 1000];
			using(var file = new StreamReader("Day6Input.txt"))
			{
				do
				{
					string line = file.ReadLine();
					string pattern = @"(?<cmd>turn on|turn off|toggle) (?<stX>\d+),(?<stY>\d+) through (?<endX>\d+),(?<endY>\d+)";
					
					var match = Regex.Match(line, pattern);
					int stX = Convert.ToInt32(match.Groups["stX"].Value);
					int endX = Convert.ToInt32(match.Groups["endX"].Value);
					int stY = Convert.ToInt32(match.Groups["stY"].Value);
					int endY = Convert.ToInt32(match.Groups["endY"].Value);
					string op = match.Groups["cmd"].Value;
					for(int x = stX; x <= endX; x++)
					{
						for(int y = stY; y <= endY; y++)
						{
							switch(op)
							{
								case "turn on":
									lamps[x, y].TurnOn();
									break;
								case "turn off":
									lamps[x, y].TurnOff();
									break;
								case "toggle":
									lamps[x, y].Toggle();
									break;
							}
						}
					}
				}while(!file.EndOfStream);
			}
			return lamps;
		}
	}
}
