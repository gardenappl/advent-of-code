
using System;
using System.IO;
using System.Collections.Generic;
using Microsoft.Xna.Framework;

namespace AdventOfCode
{
	public class Day3 : Day
	{
		struct Coordinate
		{
			int X, Y;
			public Coordinate(int X, int Y)
			{
				this.X = X;
				this.Y = Y;
			}
			
			public Coordinate Move(char c)
			{
				switch(c)
				{
					case '^':
						Y++;
						return this;
					case '>':
						X++;
						return this;
					case 'v':
						Y--;
						return this;
					case '<':
						X--;
						return this;
					default:
						Console.WriteLine("Found an invalid character. Somebody was messing with the input file!");
						Console.WriteLine("Whoever did it is totally going to the naughtly list this year...");
						return this;
				}
			}
			
			//These 4 methods doesn't seem to be required. But learinng something new is always nice! :D
			public override bool Equals(object obj)
			{
				if(obj is Coordinate)
					return X == ((Coordinate)obj).X && Y == ((Coordinate)obj).Y;
				else
					return false;
			}
			
			public override int GetHashCode()
			{
				return X.GetHashCode() + Y.GetHashCode();
			}

			public static bool operator ==(Coordinate coord1, Coordinate coord2) {
				return coord1.X == coord2.X && coord1.Y == coord2.Y;
			}

			public static bool operator !=(Coordinate coord1, Coordinate coord2) {
				return coord1.X != coord2.X || coord1.Y != coord2.Y;
			}
		}
		
		public override void Solve()
		{
			SolvePart1();
			SolvePart2();
		}
		
		static void SolvePart1()
		{
			var santaPos = new Coordinate(0, 0);
			var visitedHouses = new HashSet<Coordinate>();
			
			using(var file = new StreamReader("Day3Input.txt"))
			{
				do
				{
					visitedHouses.Add(santaPos);
					santaPos.Move((char)file.Read());
				} while(!file.EndOfStream);
			}
			Console.WriteLine("Santa delivered: " + visitedHouses.Count);
		}
		
		static void SolvePart2()
		{
			var santaPos = new Coordinate(0, 0);
			var roboSantaPos = new Coordinate(0, 0);
			bool roboSantasTurn = false;
			var visitedHouses = new HashSet<Coordinate>();
			
			using(var file = new StreamReader("Day3Input.txt"))
			{
				for(int i = 0; !file.EndOfStream; i++)
				{
					if(roboSantasTurn)
					{
						visitedHouses.Add(roboSantaPos);
						roboSantasTurn = false;
						santaPos.Move((char)file.Read());
					}
					else
					{
						if(!visitedHouses.Contains(santaPos))
							visitedHouses.Add(santaPos);
						roboSantasTurn = true;
						roboSantaPos.Move((char)file.Read());
					}
				}
			}
			Console.WriteLine("Santa + RoboSanta delivered: " + visitedHouses.Count);
		}
	}
}
