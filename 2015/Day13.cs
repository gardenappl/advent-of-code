using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day13 : Day
	{
		Dictionary<string, Guest> Guests = new Dictionary<string, Guest>();
		
		public override void Solve()
		{
			string[] input = File.ReadAllLines("Day13Input.txt");
			var pattern = new Regex(@"^(\w+) would (gain|lose) (\d+) happiness units by sitting next to (\w+).$");
			
			Guests = new Dictionary<string, Guest>();
			foreach(string line in input)
			{
				var match = pattern.Match(line);
				int interest = Int32.Parse(match.Groups[3].Value) * (match.Groups[2].Value == "gain" ? 1 : -1);
				if(!Guests.ContainsKey(match.Groups[1].Value))
					Guests.Add(match.Groups[1].Value, new Guest());
				Guests[match.Groups[1].Value][match.Groups[4].Value] = interest;
			}
			Console.WriteLine("Best combination: " + GetBestArrangement());
			
			var me = new Guest();
			foreach(string guest in Guests.Keys)
				me[guest] = 0;
			foreach(var relationships in Guests.Values)
				relationships.Add("Me lol", 0);
			Guests.Add("Me lol", me);
			
			Console.WriteLine("Best combination (with me): " + GetBestArrangement());
		}
		
		int GetBestArrangement()
		{
			int score = 0;
			GetArrangementsRecursive(0, new string[Guests.Count], ref score);
			Console.WriteLine();
			return score;
		}
		
		void GetArrangementsRecursive(int i, string[] arrangement, ref int bestScore)
		{
			if(i == arrangement.Length)
			{
				bestScore = Math.Max(bestScore, GetArrangementScore(arrangement));
				return;
			}
			foreach(string guest in Guests.Keys)
			{
				if(i == 0)
					Console.Write('.');
				if(!arrangement.Contains(guest))
				{
					var newArrangement = new string[arrangement.Length];
					arrangement.CopyTo(newArrangement, 0);
					newArrangement[i] = guest;
					GetArrangementsRecursive(i + 1, newArrangement, ref bestScore);
				}
			}
		}
		
		int GetArrangementScore(string[] arrangement)
		{
			int score = 0;
//			score += Guests[arrangement[0]][arrangement[arrangement.Length - 1]]; //first & last
			var thing = Guests[arrangement[0]];
			score += thing[arrangement[arrangement.Length - 1]];
			score += Guests[arrangement[0]][arrangement[1]]; //first & second
			for(int i = 1; i < arrangement.Length - 1; i++)
			{
				score += Guests[arrangement[i]][arrangement[i - 1]];
				score += Guests[arrangement[i]][arrangement[i + 1]];
			}
			score += Guests[arrangement[arrangement.Length - 1]][arrangement[0]]; //last & first
			score += Guests[arrangement[arrangement.Length - 1]][arrangement[arrangement.Length - 2]]; //last & second to last
			return score;
		}
		
		class Guest : Dictionary<string, int> {}
	}
}