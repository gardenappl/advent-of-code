
using System;
using System.IO;
using System.Collections.Generic;

namespace AdventOfCode
{
	public class Day8 : Day
	{
		//Here I tried to be as efficient as possible. Sorry is it's not too readable. 
		public override void Solve()
		{
			int totalCodeLength = 0;
			int totalStringLength = 0;
			using(var file = new StreamReader("Day8Input.txt"))
			{
				do
				{
					string line = file.ReadLine();
					
					totalCodeLength += line.Length;
					bool escapingHexCharacter = false;
					bool escapedOneHexChar = false;
					bool prevCharWasBackslash = false;
					foreach(char c in line)
					{
						switch(c)
						{
							case '\\': //single backslash
								if(prevCharWasBackslash)
								{
									totalStringLength++;
									prevCharWasBackslash = false;
								}
								else
									prevCharWasBackslash = true;
								escapingHexCharacter = false;
								escapedOneHexChar = false;
								break;
							case '"':
								if(prevCharWasBackslash)
								{
									totalStringLength++;
									prevCharWasBackslash = false;
								}
								break;
							case 'x':
								if(prevCharWasBackslash)
								{
									prevCharWasBackslash = false;
									escapingHexCharacter = true;
									break;
								}
								totalStringLength++;
								break;
							default:
								if(escapingHexCharacter)
								{
									if(!escapedOneHexChar)
									{
										escapedOneHexChar = true;
										break;
									}
									totalStringLength++;
									escapingHexCharacter = false;
									escapedOneHexChar = false;
									break;
								}
								totalStringLength++;
								break;
						}
					}
				}while(!file.EndOfStream);
			}
			
			Console.WriteLine(totalCodeLength - totalStringLength);
		}
	}
}
