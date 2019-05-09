
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Day4
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
					string name = line.Substring(0, line.LastIndexOf('-'));
					int id = Int32.Parse(line.Substring(line.LastIndexOf('-') + 1, 3));
					string checksum = line.Substring(line.LastIndexOf('[') + 1, 5);
					
//					var count = new Dictionary<char, int>();
//					foreach(char c in name)
//					{
//						if(!count.ContainsKey(c))
//							count[c] = 1;
//						else
//							count[c]++;
//					}
					
					char[] decrypt = name.ToArray();
					for(int i = 0; i < name.Length; i++)
					{
						if(name[i] == '-')
							decrypt[i] = ' ';
						else for(int k = 0; k < id % 26; k++)
						{
							decrypt[i] = decrypt[i] == 'z' ? 'a' : (char)(decrypt[i] + 1);
						}
					}
					if(new String(decrypt).Contains("northpole"))
						Console.WriteLine(new String(decrypt) + ": id" + id);
					
					name = name.Replace("-", String.Empty);
					if(new String(name.GroupBy(c => c)
					              .Select(g => new { Value = g.Key, Count = g.Count() })
					              .OrderByDescending(x => x.Count)
					              .ThenBy(x => x.Value)
					              .Take(5)
					              .Select(x => x.Value)
					              .ToArray()) == checksum)
					{
						total += id;
					}
				}
			}
			Console.WriteLine("Answer Part 1: " + total);
			Console.ReadKey();
		}
	}
}
