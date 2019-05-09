
using System;
using System.IO;
using System.Text;

namespace AdventOfCode
{
	public class Day10 : Day
	{
		public override void Solve()
		{
			LookAndSay(File.ReadAllText("Day10Input.txt"), 40);
			LookAndSay(File.ReadAllText("Day10Input.txt"), 50);
		}
		
		void LookAndSay(string originalString, int times)
		{
			string previousString = originalString;
			var sb = new StringBuilder();
			
			for(int i = 0; i < times; i++)
			{
				char prevDigit = '.';
				short prevDigitNum = 1;
				foreach(char digit in previousString)
				{
					if(digit == prevDigit)
						prevDigitNum++;
					else
					{ 
						if(prevDigit != '.')
						{
							sb.Append(prevDigitNum);
							sb.Append(prevDigit);
						}
						prevDigit = digit;
						prevDigitNum = 1;
					}
				}
				//Add the last characters
				sb.Append(prevDigitNum);
				sb.Append(prevDigit);
				previousString = sb.ToString();
				sb.Clear();
			}
			Console.WriteLine("Looked and said {0} times...", times);
			Console.WriteLine("Length of string: " + previousString.Length);
			Console.WriteLine();
		}
	}
}
