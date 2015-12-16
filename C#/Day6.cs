
using System;
using System.IO;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day6 : Day
	{
		public static bool isPart2 = false;
		
		struct Lamp
		{
			public int Brightness
			{
				get;
				private set;
			}
			
			public bool TurnedOn
			{
				get;
				private set;
			}
			
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
				else
				{
					if(Brightness != 0)
						Brightness--;
				}
			}
			public void Toggle()
			{
				if(!isPart2) 
				{
					if(TurnedOn)
						TurnedOn = false;
					else
						TurnedOn = true;
				}
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
			Console.WriteLine("Result 1: " + turnedOnLamps);
			
			isPart2 = true;
			
			lamps = FollowInstructions();
			
			int totalBrightness = 0;
			foreach(var lamp in lamps)
				totalBrightness += lamp.Brightness;
			Console.WriteLine("Result 2: " + totalBrightness);
		}
		
		static Lamp[,] FollowInstructions()
		{
			var lamps = new Lamp[1000,1000];
			using(var file = new StreamReader("Day6Input.txt"))
			{
				do
				{
					string line = file.ReadLine();
					string pattern = @"(?<cmd>turn on|turn off|toggle) (?<stX>\d+),(?<stY>\d+) through (?<endX>\d+),(?<endY>\d+)";
					
					var match = Regex.Match(line, pattern);
					for(int x = Convert.ToInt32(match.Groups["stX"].Value); x <= Convert.ToInt32(match.Groups["endX"].Value); x++)
					{
						for(int y = Convert.ToInt32(match.Groups["stY"].Value); y <= Convert.ToInt32(match.Groups["endY"].Value); y++)
						{
							switch(match.Groups["cmd"].Value)
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
					Console.Write('.');
				}while(!file.EndOfStream);
			}
			Console.WriteLine();
			return lamps;
		}
	}
}
