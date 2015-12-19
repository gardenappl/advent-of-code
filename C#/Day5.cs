
using System;
using System.IO;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day5 : Day
	{
		public override void Solve()
		{
			//Huge thanks to RegExr.com!
			//Part 1 patterns
			const string naughtyPattern = @"(ab|cd|pq|xy)"; //Nice strings don't contain "ab", "cd", "pq" or "xy"!
			const string vowelPattern = @"(.*[aeiou].*){3}"; //Nice strings contain at least 3 vowels
			const string repeatPattern =  @"^.*(\w)\1"; //Nice strings contain a letter that repeats twice in a row
			int niceStringsPart1 = 0;
			
			//Part 2 patterns
			const string repeatBetweenPattern = @"(\w)\w\1"; //Nice strings contain a letter which repeats with exactly one letter between them
			const string pairRepeatPattern = @"(\w\w)\w*\1"; //Nice strings contain a pair of letters which repeats without overlapping (e.g. "lloll" but not "kkk")
			int niceStringsPart2 = 0;
			
			using(var file = new StreamReader("Day5Input.txt"))
			{
				do
				{
					string line = file.ReadLine();
					if(!Regex.IsMatch(line, naughtyPattern) && Regex.IsMatch(line, vowelPattern) && Regex.IsMatch(line, repeatPattern))
						niceStringsPart1++;
					if(Regex.IsMatch(line, repeatBetweenPattern) && Regex.IsMatch(line, pairRepeatPattern))
						niceStringsPart2++;
				}while(!file.EndOfStream);
			}
			Console.WriteLine("Nice strings 1: " + niceStringsPart1);
			Console.WriteLine("Nice strings 2: " + niceStringsPart2);
		}
	}
}
