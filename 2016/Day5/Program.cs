
using System;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace Day5
{
	public class Program
	{
		public static void Main()
		{
			bool part1;
			
			Console.ForegroundColor = ConsoleColor.Green;
			Console.Write("Hack door 1 or 2?");
			char c = ' ';
			while(c != '1' && c != '2')
				c = Console.ReadKey(true).KeyChar;
			part1 = c == '1';
			Console.Clear();
			Console.WriteLine("Hacking...");
			Console.WriteLine("--------");
			
			string input = File.ReadAllText("input.txt");
//			string input = "abc";
			int i = 0;
			int num = 0;
			bool[] hacked = new bool[8];
			using(var md5 = MD5.Create())
			{
				if(part1) while(true)
				{
					HackingProcedure(num);
					string hash = GetMD5Hash(md5, input + num);
					
					if(hash.StartsWith("00000"))
					{
						Console.SetCursorPosition(i, 1);
						Console.Write(hash[5]);
						Console.SetCursorPosition(0, 2 + i);
						Console.WriteLine(input + num + ": " + hash);
						i++;
						if(i == 8)
							break;
					}
					num++;
				}
				else while(true)
				{
					HackingProcedure(num);
					string hash = GetMD5Hash(md5, input + num);
					
					if(hash.StartsWith("00000"))
					{
						if(hash[5] < '8')
						{
							int position = Int32.Parse(hash[5].ToString());
							if(hacked[position])
							{
								num++;
								continue;
							}
							Console.SetCursorPosition(position, 1);
							Console.Write(hash[6]);
							hacked[position] = true;
							if(hacked.All(pos => pos))
								break;
						}
						Console.SetCursorPosition(0, 2 + i);
						Console.Write(input + num + ": " + hash);
						Console.WriteLine(hash[5] >= '8' ? " -INVALID" : String.Empty);
						i++;
					}
					num++;
				}
			}
			Console.SetCursorPosition(0, 0);
			Console.WriteLine("HACK COMPLETE");
			Console.ReadKey();
		}
		
		static string GetMD5Hash(MD5 md5, string input)
		{
			byte[] data = md5.ComputeHash(Encoding.UTF8.GetBytes(input));
			
			var sb = new StringBuilder(32);
			foreach(byte b in data)
				sb.Append(b.ToString("x2"));
			
			return sb.ToString();
		}
		
		static void HackingProcedure(int num)
		{
			switch(num % 200000)
			{
				case 0:
					Console.SetCursorPosition(10, 0);
					Console.Write('|');
					break;
				case 50000:
					Console.SetCursorPosition(10, 0);
					Console.Write('/');
					break;
				case 100000:
					Console.SetCursorPosition(10, 0);
					Console.Write('-');
					break;
				case 150000:
					Console.SetCursorPosition(10, 0);
					Console.Write('\\');
					break;
			}
		}
	}
}
