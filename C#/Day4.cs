
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
			bool foundResult1 = false;
			MD5 md5 = MD5.Create();
			for(int i = 0; i < Int32.MaxValue; i++)
			{
				string hash = GetMD5(md5, string.Join("", input, i));
				//Console.WriteLine("{0,10} -> {1}", i, hash); //This looks like super cool-hax0ring but writing to console is really laggy ;(
				if(hash.StartsWith("00000") && !foundResult1)
				{
					foundResult1 = true;
					Console.WriteLine();
					Console.WriteLine("Result 1: {0}", i);
				}
				if(hash.StartsWith("000000"))
				{
					Console.WriteLine();
					Console.WriteLine("Result 2: {0}", i);
					return;
				}
				if(i % 10000 == 0)
					Console.Write('.'); //Just so that I can tell that my program is working and not just freezing to death.
			}
			Console.WriteLine("We reached Int32.MaxValue... what?");
		}
		
		string GetMD5(MD5 md5, string input)
		{
			//I copy-pasted this line of code from the internet. I would've never figured out MD5 conversion myself.
			byte[] hash = md5.ComputeHash(Encoding.ASCII.GetBytes(input));
			//Formats each byte in the hash array to hexadecimal and makes a string out of that.
			return string.Join("", hash.Select((byte b) => b.ToString("x2"))); //Using Lambdas and Linq is both cool and super-efficient!
		}
	}
}
