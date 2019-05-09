
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day9 : Day
	{
		public override void Solve()
		{
			//Key is the name of a town
			//Value is another Dictionary where keys are the names of all other towns and value is the distance between A and B.
			var routes = new Dictionary<string, Dictionary<string, int>>();
			string[] input = File.ReadAllLines("Day9Input.txt");
			var pattern = new Regex( @"^(\w+) to (\w+) = (\d+)$");
			
			foreach(string line in input)
			{
				var match = pattern.Match(line);
				string pointA = match.Groups[1].Value;
				string pointB = match.Groups[2].Value;
				int distance = Int32.Parse(match.Groups[3].Value);
				
				if(!routes.ContainsKey(pointA))
					routes.Add(pointA, new Dictionary<string, int>());
				if(!routes[pointA].ContainsKey(pointB))
					routes[pointA].Add(pointB, distance);
				
				if(!routes.ContainsKey(pointB))
					routes.Add(pointB, new Dictionary<string, int>());
				if(!routes[pointB].ContainsKey(pointA))
					routes[pointB].Add(pointA, distance);
			}
			
			var scores = GetBestPath(routes);
			Console.WriteLine("Shortest path: " + scores.Item1);
			Console.WriteLine("Longest path: " + scores.Item2);
		}
		
		Tuple<int, int> GetBestPath(Dictionary<string, Dictionary<string, int>> routes)
		{
			int lowestScore = Int32.MaxValue;
			int highestScore = Int32.MinValue;
			foreach(string startLocation in routes.Keys)
				GetBestPathRecursive(routes, startLocation, new HashSet<string>{startLocation}, 0, ref lowestScore, ref highestScore);
			return Tuple.Create(lowestScore, highestScore);
		}
		
		void GetBestPathRecursive(Dictionary<string, Dictionary<string, int>> routes, string currentLocation,
		                              HashSet<string> visited, int currentScore, ref int lowestScore, ref int highestScore)
		{
			if(visited.Count >= routes.Count) //Reached the end
			{
				lowestScore = Math.Min(lowestScore, currentScore);
				highestScore = Math.Max(highestScore, currentScore);
			}
			
			foreach(var destination in routes[currentLocation])
			{
				if(visited.Contains(destination.Key))
					continue;
				var newVisitedLocations = new HashSet<string>(visited);
				newVisitedLocations.Add(destination.Key);
				GetBestPathRecursive(routes, destination.Key, newVisitedLocations, currentScore + destination.Value, ref lowestScore, ref highestScore);
			}
		}
	}
}
