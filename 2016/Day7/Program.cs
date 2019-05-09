
using System;
using System.Collections.Generic;
using System.IO;

namespace Day7
{
	public class Program
	{
		public static void Main()
		{
			int total = 0;
			using(var reader = new StreamReader("input.txt"))
			{
				while(!reader.EndOfStream)
				{
					string line = reader.ReadLine();
					bool abba = false;
					bool hypernet = false;
					bool invalid = false;
					for(int i = 0; i <= line.Length - 4; i++)
					{
						if(line[i] == '[')
						{
							hypernet = true;
							continue;
						}
						if(line[i] == ']')
						{
							hypernet = false;
							continue;
						}
						if(line[i] == line[i + 3] && line[i + 1] == line[i + 2] && line[i] != line[i + 1])
						{
							if(hypernet)
							{
								Console.ForegroundColor = ConsoleColor.Red;
								Console.WriteLine(new String(new []{line[i], line[i + 1], line[i + 2], line[i + 3]}));
								invalid = true;
							}
							else
							{
								Console.ForegroundColor = ConsoleColor.Gray;
								Console.WriteLine(new String(new []{line[i], line[i + 1], line[i + 2], line[i + 3]}));
								abba = true;
							}
						}
					}
					if(abba && !invalid)
						total++;
				}
			}
			Console.WriteLine("Answer Part 1: " + total);
			total = 0;
			using(var reader = new StreamReader("input.txt"))
			{
				while(!reader.EndOfStream)
				{
					string line = reader.ReadLine();
					var ABAs = new List<char[]>();
					var BABs = new List<char[]>();
					bool hypernet = false;
					bool valid = false;
					for(int i = 0; i <= line.Length - 3; i++)
					{
						if(line[i] == '[')
						{
							hypernet = true;
							continue;
						}
						if(line[i] == ']')
						{
							hypernet = false;
							continue;
						}
						if(line[i] == line[i + 2] && line[i] != line[i + 1])
						{
							if(!hypernet)
								ABAs.Add(new []{line[i], line[i + 1], line[i + 2]});
							else
								BABs.Add(new []{line[i], line[i + 1], line[i + 2]});
						}
						foreach(var aba in ABAs)
							foreach(var bab in BABs)
								if(aba[0] == bab[1] && aba[1] == bab[0])
									valid = true;
					}
					if(valid)
						total++;
				}
			}
			Console.WriteLine("Answer Part 2: " + total);
			Console.ReadKey();
		}
	}
}
