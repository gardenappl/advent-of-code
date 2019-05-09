
using System;

namespace AdventOfCode
{
	public class Day25 : Day
	{
		public override void Solve()
		{
			long value = 20151125;
			int x = 1;
			int y = 1;
			while(!(x == 3019 && y == 3010))
			{
				x++;
				y--;
				if(y == 0)
				{
					y = x;
					x = 1;
				}
				value *= 252533;
				value %= 33554393;
			}
			Console.WriteLine(value);
		}
	}
}
