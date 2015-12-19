
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
			for(int i = 0; i < Int32.MaxValue; i++)
			{
				string hash = GetMD5(md5, string.Join("", input, i));
				if(hash.StartsWith("00000"))
				{
					Console.WriteLine();
					Console.WriteLine("MD5 hash with 5 zeroes: " + input + i);
					break;
				}
				if(i % 10000 == 0)
					Console.Write('.'); //Just so that I can tell that my program is working and not just freezing to death.
			}
			for(int i = 0; i < Int32.MaxValue; i++)
			{
				string hash = GetMD5(md5, string.Join("", input, i));
				if(hash.StartsWith("000000"))
				{
					Console.WriteLine();
					Console.WriteLine("MD5 hash with 6 zeroes: " + input + i);
					return;
				}
				if(i % 10000 == 0)
					Console.Write('.');
			}
			Console.WriteLine("After {0} operations, the answer wasn't found.", Int32.MaxValue);
		}
		
		string GetMD5(MD5 md5, string input)
		{
			byte[] hash = md5.ComputeHash(Encoding.ASCII.GetBytes(input));
			//Formats each byte in the hash array to hexadecimal and makes a string out of that.
			return string.Join("", hash.Select((byte b) => b.ToString("x2")));
		}
	}
}
