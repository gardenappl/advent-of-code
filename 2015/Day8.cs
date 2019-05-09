
using System;
using System.IO;
using System.Collections.Generic;

namespace AdventOfCode
{
	public class Day8 : Day
	{
		//Here I tried to be as efficient as possible. Sorry if it's not readable. 
		//It runs in 2 ms on my ancient laptop though and I'm proud of that.
		public override void Solve()
		{
			int totalCodeLength = 0;
			int totalLengthPart1 = 0;
			int totalLengthPart2 = 0;
			using(var file = new StreamReader("Day8Input.txt"))
			{
				do
				{
					string line = file.ReadLine();
					
					totalCodeLength += line.Length;
					totalLengthPart2 += 2; //"s in the beginning and in the end of the string. 
					
					bool escapingHexCharacter = false;
					bool escapedOneHexChar = false;
					bool prevCharWasBackslash = false;
					foreach(char c in line)
					{
						switch(c)
						{
							case '\\': //single backslash
								totalLengthPart2 += 2;
								if(prevCharWasBackslash)
								{
									totalLengthPart1++;
									prevCharWasBackslash = false;
								}
								else
									prevCharWasBackslash = true;
								break;
							case '"':
								totalLengthPart2 += 2;
								if(prevCharWasBackslash)
								{
									totalLengthPart1++;
									prevCharWasBackslash = false;
								}
								break;
							case 'x':
								totalLengthPart2++;
								if(prevCharWasBackslash)
								{
									prevCharWasBackslash = false;
									escapingHexCharacter = true;
									break;
								}
								totalLengthPart1++;
								break;
							default:
								totalLengthPart2++;
								if(escapingHexCharacter)
								{
									if(!escapedOneHexChar)
									{
										escapedOneHexChar = true;
									}
									else
									{
										totalLengthPart1++;
										escapingHexCharacter = false;
										escapedOneHexChar = false;
									}
									break;
								}
								totalLengthPart1++;
								break;
						}
					}
				}while(!file.EndOfStream);
			}
			Console.WriteLine("Answer to part 1: " + (totalCodeLength - totalLengthPart1));
			Console.WriteLine("Answer to part 2: " + (totalLengthPart2 - totalCodeLength));
		}
	}
}
