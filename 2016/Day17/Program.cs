
using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace Day17
{
	public class Program
	{
		static MD5 md5 = MD5.Create();
		
		public static void Main()
		{
			int x = 0;
			int y = 0;
			string password = File.ReadAllText("input.txt");
			int steps = Int32.MaxValue;
			string shortest = "Not Found";
			FindShortest(password, 0, 0, ref steps, ref shortest);
			Console.WriteLine("Shortest Path: " + shortest);
			FindLongest(password, 0, 0, ref steps);
			Console.WriteLine("Longest Path Length: " + steps);
			Console.ReadKey();
		}
		
		static void FindShortest(string password, int x, int y, ref int minSteps, ref string shortest, int steps = 0)
		{
			if(steps > minSteps)
				return;
			if(x == 3 && y == 3)
			{
				if(steps < minSteps)
				{
					minSteps = steps;
					shortest = password.Remove(0, 8);
				}
				return;
			}
			bool[] directions = GetDirections(password);
			if(directions[0] && y != 0) //up
			{
				FindShortest(password + "U", x, y - 1, ref minSteps, ref shortest, steps + 1);
			}
			if(directions[1] && y != 3) //down
			{
				FindShortest(password + "D", x, y + 1, ref minSteps, ref shortest, steps + 1);
			}
			if(directions[2] && x != 0) //left
			{
				FindShortest(password + "L", x - 1, y, ref minSteps, ref shortest, steps + 1);
			}
			if(directions[3] && x != 3) //right
			{
				FindShortest(password + "R", x + 1, y, ref minSteps, ref shortest, steps + 1);
			}
		}
		
		static void FindLongest(string password, int x, int y, ref int maxSteps, int steps = 0)
		{
			if(x == 3 && y == 3)
			{
				if(steps > maxSteps)
				{
					maxSteps = steps;
				}
				return;
			}
			bool[] directions = GetDirections(password);
			if(directions[0] && y != 0) //up
			{
				FindLongest(password + "U", x, y - 1, ref maxSteps, steps + 1);
			}
			if(directions[1] && y != 3) //down
			{
				FindLongest(password + "D", x, y + 1, ref maxSteps, steps + 1);
			}
			if(directions[2] && x != 0) //left
			{
				FindLongest(password + "L", x - 1, y, ref maxSteps, steps + 1);
			}
			if(directions[3] && x != 3) //right
			{
				FindLongest(password + "R", x + 1, y, ref maxSteps, steps + 1);
			}
		}
		
		static bool[] GetDirections(string input)
		{
			bool[] directions = new bool[4];
			byte[] data = md5.ComputeHash(Encoding.UTF8.GetBytes(input));
			directions[0] = ((data[0] & 0xF0) >> 4) > 10;
			directions[1] = (data[0] & 0x0F) > 10;
			directions[2] = ((data[1] & 0xF0) >> 4) > 10;
			directions[3] = (data[1] & 0x0F) > 10;
			
			return directions;
		}
	}
}
