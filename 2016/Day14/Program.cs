using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace Day14
{
	public class Program
	{
		public static void Main(string[] args)
		{
			if(args.Length != 1)
			{
				Console.WriteLine("Specify hash salt as the 1st program argument.");
				Console.ReadKey();
				return;
			}
			string salt = args[0];
//			string salt = "abc";
			Find64thKeyIndex(salt);
			Find64thKeyIndex(salt, true);
			Console.ReadKey();
		}
		
		static int Find64thKeyIndex(string salt, bool part2 = false)
		{
			var md5 = MD5.Create();
			
			if(part2)
				Console.WriteLine(GetMD5Hash(md5, "abc22551", true));
			
			var foundTriple = new Dictionary<char, List<int>>();
			int lastTriple = 0;
			var keys = new HashSet<int>();
			int index = 0;
			while(keys.Count < 64 || (keys.Count == 64 && index < lastTriple + 10000))
			{
				string hash = GetMD5Hash(md5, salt + index, part2);
				char prevC = ' ';
				int repeat = 1;
				foreach(char c in hash)
				{
					bool foundOneTriple = false;
					if(c == prevC)
						repeat++;
					else
					{
						if(repeat >= 5 && foundTriple.ContainsKey(prevC))
						{
							Console.WriteLine("Quintuple at {0}: {1}", index, hash);
							foreach(int prevIndex in foundTriple[prevC])
							{
								if(prevIndex > index - 1000)
								{
									if(keys.Count == 64)
									{
										int max = keys.Max();
										if(lastTriple < max)
										{
											keys.Remove(max);
											keys.Add(prevIndex);
										}
									}
									else
										keys.Add(prevIndex);
									
									if(keys.Count == 64 && lastTriple == 0)
										lastTriple = prevIndex;
									
									Console.WriteLine("   Key {0} at {1}: {2}", keys.Count, prevIndex, GetMD5Hash(md5, salt + prevIndex, part2));
								}
							}
						}
						if(repeat >= 3 && !foundOneTriple)
						{
							if(!foundTriple.ContainsKey(prevC))
								foundTriple[prevC] = new List<int>();
							foundTriple[prevC].Add(index);
							foundOneTriple = true;
						}
						
						repeat = 1;
					}
					prevC = c;
				}
				
				index++;
			}
			Console.ForegroundColor = ConsoleColor.Green;
			var sortedKeys = keys.OrderBy(i => i).ToArray();
			for(int i = 0; i < keys.Count; i++)
			{
				Console.WriteLine("Key {0} at {1}: {2}", i, sortedKeys[i], GetMD5Hash(md5, salt + sortedKeys[i], part2));
			}
			Console.ResetColor();
			return keys.Max();
		}
		
		static string GetMD5Hash(MD5 md5, string input, bool part2 = false)
		{
			string hash = input;
			var sb = new StringBuilder(32);
			
			for(int i = 0; i <= (part2 ? 2016 : 0); i++)
			{
				byte[] data = md5.ComputeHash(Encoding.UTF8.GetBytes(hash));
				
				foreach(byte b in data)
					sb.Append(b.ToString("x2"));
				hash = sb.ToString();
				sb.Clear();
			}
			
			return hash;
		}
	}
}
