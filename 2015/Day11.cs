
using System;
using System.IO;

namespace AdventOfCode
{
	public class Day11 : Day
	{
		public override void Solve()
		{
			string password = File.ReadAllText("Day11Input.txt");
			do
			{
				password = Increment(password);
			} while(!IsGoodPassword(password) || password == "zzzzzzzz");
			Console.WriteLine("Next password: " + password);
			
			do
			{
				password = Increment(password);
			} while(!IsGoodPassword(password) || password == "zzzzzzzz");
			Console.WriteLine("Next password: " + password);
		}
		
		string Increment(string password)
		{
			for(int i = password.Length - 1; i >= 0; i--)
			{
				if(password[i] == 'z')
				{
					password = password.Remove(i, 1);
					password = password.Insert(i, "a");
					continue;
				}
				else
				{
					char incrementChar = password[i];
					incrementChar++;
					password = password.Remove(i, 1);
					password = password.Insert(i, incrementChar.ToString());
					return password;
				}
			}
			return "zzzzzzzz";
		}
		
		bool IsGoodPassword(string password)
		{
			char previousChar = '.';
			char previousPair = '.';
			bool hasTwoDifferentPairs = false;
			short increasingChars = 1;
			bool hasThreeIncreasingChars = false;
			for(int i = 0; i < password.Length; i++)
			{
				char c = password[i];
				if(c == 'i' || c == 'o' || c == 'l')
					return false;
				if(c == previousChar)
				{
					if(c != previousPair && previousPair != '.')
						hasTwoDifferentPairs = true;
					previousPair = c;
				}
				if(c == (char)(previousChar + 1))
				{
					increasingChars++;
					if(increasingChars >= 3)
						hasThreeIncreasingChars = true;
				}
				else
					increasingChars = 1;
				previousChar = c;
			}
			return hasTwoDifferentPairs && hasThreeIncreasingChars;
		}
	}
}
