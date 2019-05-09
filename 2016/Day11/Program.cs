
using System;
using System.Collections.Generic;

namespace Day11
{
	public class Program
	{
		public static void Main()
		{
			var floors = new []
			{
				new Floor(new string[0], new []{"H", "Li"}),
				new Floor(new []{"H"}, new string[0]),
				new Floor(new []{"Li"}, new string[0]),
				new Floor(new string[0], new string[0])
//				new Floor(new []{"polonium", "thulium", "promethium", "ruthenium", "cobalt"}, new []{"thulium", "ruthenium", "cobalt"}),
//				new Floor(new string[0], new []{"polonium", "promethium"}),
//				new Floor(new List<string>(), new List<string>()),
//				new Floor(new List<string>(), new List<string>())
			};
			int minSteps = 20;
			MoveRecursive(0, 0, ref minSteps, floors, 0);
			Console.WriteLine(minSteps);
			Console.ReadKey();
		}
		
		static void MoveRecursive(int i, int steps, ref int minSteps, Floor[] prevFloors, int prevI, string gen1 = "", string gen2 = "", string chip1 = "", string chip2 = "")
		{
//			try{
			if(steps > minSteps)
				return;
			
			var floors = new Floor[4];
			for(int k = 0; k < 4; k++)
			{
				floors[k] = new Floor(prevFloors[k].Generators, prevFloors[k].Microchips);
			}
			
			if(gen1 != String.Empty)
			{
				floors[prevI].Generators.Remove(gen1);
				floors[i].Generators.Add(gen1);
			}
			if(gen2 != String.Empty)
			{
				floors[prevI].Generators.Remove(gen2);
				floors[i].Generators.Add(gen2);
			}
			if(chip1 != String.Empty)
			{
				floors[prevI].Microchips.Remove(chip1);
				floors[i].Microchips.Add(chip1);
			}
			if(chip2 != String.Empty)
			{
				floors[prevI].Microchips.Remove(chip2);
				floors[i].Microchips.Add(chip2);
			}
			
			bool success = true;
			for(int k = 0; k < 3; k++)
			{
				if(floors[k].Generators.Count != 0 || floors[k].Microchips.Count != 0)
				{
					success = false;
					break;
				}
			}
			if(success)
			{
				minSteps = steps;
				return;
			}
			
			var currentFloor = floors[i];
			foreach(var chip in currentFloor.Microchips)
			{
				bool shielded = false;
				bool fried = false;
				foreach(var gen in currentFloor.Generators)
				{
					if(chip == gen) shielded = true;
					if(chip != gen) fried = true;
				}
				if(!shielded && fried)
					return;
			}
			
			foreach(var moveGen in currentFloor.Generators)
			{
				if(i != 3) MoveRecursive(i + 1, steps + 1, ref minSteps, floors, i, gen1: moveGen);
				if(i != 0) MoveRecursive(i - 1, steps + 1, ref minSteps, floors, i, gen1: moveGen);
				foreach(var moveGen2 in currentFloor.Generators)
				{
					if(moveGen2 != moveGen)
					{
						if(i != 3) MoveRecursive(i + 1, steps + 1, ref minSteps, floors, i, gen1: moveGen, gen2: moveGen2);
						if(i != 0) MoveRecursive(i - 1, steps + 1, ref minSteps, floors, i, gen1: moveGen, gen2: moveGen2);
					}
				}
			}
			
			foreach(var moveChip in currentFloor.Microchips)
			{
				if(i != 3) MoveRecursive(i + 1, steps + 1, ref minSteps, floors, i, chip1: moveChip);
				if(i != 0) MoveRecursive(i - 1, steps + 1, ref minSteps, floors, i, chip1: moveChip);
				foreach(var moveChip2 in currentFloor.Microchips)
				{
					if(moveChip2 != moveChip)
					{
						if(i != 3) MoveRecursive(i + 1, steps + 1, ref minSteps, floors, i, chip1: moveChip, chip2: moveChip2);
						if(i != 0) MoveRecursive(i - 1, steps + 1, ref minSteps, floors, i, chip1: moveChip, chip2: moveChip2);
					}
				}
			}
			
			foreach(var moveChip in currentFloor.Microchips)
			{
				foreach(var moveGen in currentFloor.Generators)
				{
					if(i != 3) MoveRecursive(i + 1, steps + 1, ref minSteps, floors, i, chip1: moveChip, gen1: moveGen);
					if(i != 0) MoveRecursive(i - 1, steps + 1, ref minSteps, floors, i, chip1: moveChip, gen1: moveGen);
				}
			}
//			}catch(Exception e)
//			{
//				Console.WriteLine("{0}", s);
//			}
		}
		
//		static Floor[] Move(Floor[] floors, int i, int newI, string gen1 = "", string gen2 = "", string chip1 = "", string chip2 = "")
//		{
//			var newFloors = new Floor[4];
//			floors.CopyTo(newFloors, 0);
//			
//			newFloors[i].Generators.Remove(gen1);
//			newFloors[newI].Generators.Add(gen1);
//			newFloors[i].Generators.Remove(gen2);
//			newFloors[newI].Generators.Add(gen2);
//			
//			newFloors[i].Microchips.Remove(chip1);
//			newFloors[newI].Microchips.Add(chip1);
//			newFloors[i].Microchips.Remove(chip2);
//			newFloors[newI].Microchips.Add(chip2);
//			
//			return newFloors;
//		}
		
		struct Floor
		{
			public readonly List<string> Generators;
			public readonly List<string> Microchips;
			
			public Floor(string[] generators, string[] microchips)
			{
				Generators = new List<string>(generators);
				Microchips = new List<string>(microchips);
			}
			
			public Floor(List<string> generators, List<string> microchips)
			{
				Generators = generators;
				Microchips = microchips;
			}
			
			bool IsValid()
			{
				if(Generators.Count == 0)
					return true;
				
				foreach(var chip in Microchips)
				{
					bool fried = true;
					foreach(var gen in Generators)
					{
						if(chip == gen)
						{
							fried = false;
							break;
						}
					}
					if(fried)
						return false;
				}
				return true;
			}
		}
	}
}
