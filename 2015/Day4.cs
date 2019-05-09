
using System;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;

namespace AdventOfCode
{
	public class Day4 : Day
	{
		public override void Solve()
		{
			string input = File.ReadAllText("Day4Input.txt");
			MD5 md5 = MD5.Create();
			bool found5Zeroes = false;
			
			for(int i = 0; i < Int32.MaxValue; i++)
			{
				string hash = GetMD5(md5, string.Concat(input, i));
				if(hash.StartsWith("00000") && !found5Zeroes)
				{
					Console.WriteLine();
					Console.WriteLine("MD5 hash with 5 zeroes: {0}{1}", input, i);
					found5Zeroes = true;
				}
				if(hash.StartsWith("000000"))
				{
					Console.WriteLine();
					Console.WriteLine("MD5 hash with 6 zeroes: {0}{1}", input, i);
					return;
				}
				if(i % 10000 == 0)
					Console.Write('.'); //Just so that I can tell that my program is working and not freezing to death.
			}
			
			Console.WriteLine("After {0} operations, the answer wasn't found.", Int32.MaxValue);
		}
		
		string GetMD5(MD5 md5, string input)
		{
			byte[] hash = md5.ComputeHash(Encoding.ASCII.GetBytes(input));
			//Formats each byte in the hash array to hexadecimal and combines them into a string.
			return string.Join("", hash.Select((byte b) => b.ToString("x2")));
		}
	}
}
