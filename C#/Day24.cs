
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;

namespace AdventOfCode
{
	public class Day24 : Day
	{
		Random rnd = new Random();
		
		public override void Solve()
		{
			string[] input = File.ReadAllLines("Day24Input.txt");
			int[] packages = new int[input.Length];
			for(int line = 0; line < input.Length; line++)
				packages[line] = Int32.Parse(input[line]);
			
			
			Console.WriteLine("\nSmallest QE: " + FindBestCombo(packages, 3));
			Console.WriteLine("\nSmallest QE: " + FindBestCombo(packages, 4));
		}
		
		long FindBestCombo(int[] packages, int groups)
		{
			packages = packages.OrderByDescending(x => x).ToArray();
			int groupWeight = packages.Sum(x => x) / groups;
			
			long smallestQE = Int64.MaxValue;
			int smallest1 = (int)Math.Ceiling((double)packages.Count() / groups);
			FindBestComboRecursive(packages, groupWeight, groups, new bool[packages.Length], ref smallestQE, ref smallest1);
			return smallestQE;
		}
		
		void FindBestComboRecursive(int[] packages, int neededWeight, int groupsAmount, bool[] firstGroup, ref long smallestQE, ref int smallest1, int index = 0)
		{
			if(CountPackages(firstGroup) > smallest1)
				return;
			
			if(index >= packages.Length) //If gone through all packages
			{
				if(GetGroupWeight(firstGroup, packages) != neededWeight)
					return;
				
				bool[][] groups = new bool[groupsAmount][];
				groups[0] = firstGroup;
				for(int i = 1; i < groupsAmount; i++)
					groups[i] = new bool[packages.Length];
				if(!CanDistributeOtherGroups(packages, 0, groups))
				{
					Console.Write('|');
					return;
				}
				
				Console.Write('.');
				long QE = CountQE(firstGroup, packages);
				int count = CountPackages(firstGroup);
				if(count < smallest1)
				{
					smallest1 = count;
					smallestQE = QE;
					return;
				}
				else if(count == smallest1)
					if(QE < smallestQE)
						smallestQE = QE;
				return;
			}
			
			//Try adding the next package
			FindBestComboRecursive(packages, neededWeight, groupsAmount, AddPackage(index, firstGroup), ref smallestQE, ref smallest1, index + 1);
			//Try not adding the next package
			FindBestComboRecursive(packages, neededWeight, groupsAmount, firstGroup, ref smallestQE, ref smallest1, index + 1);
		}
		
		bool CanDistributeOtherGroups(int[] packages, int index, params bool[][] groups)
		{
			int[] weights = new int[groups.Length];
			
			for(int i = 0; i < weights.Length; i++)
			{
				weights[i] = GetGroupWeight(groups[i], packages);
				if(weights[i] > weights[0])
					return false;
			}
			
			if(index >= packages.Length)
			{
				foreach(int weight in weights)
					if(weight != weights[0])
						return false;
				return true;
			}
			
			if(groups[0][index]) //If current package is already in group 1
				if(CanDistributeOtherGroups(packages, index + 1, CopyArray(groups))) //Iterate more and don't put anything to groups 2 or 3
					return true;
			
			for(int i = 1; i < groups.Length; i++)
				if(CanDistributeOtherGroups(packages, index + 1, AddPackage(i, index, groups)))
					return true;
			return false;
		}
		
		bool[] AddPackage(int packageNum, bool[] group)
		{
			bool[] copy = new bool[group.Length];
			Array.Copy(group, copy, group.Length);
			copy[packageNum] = true;
			return copy;
		}
		
		bool[][] AddPackage(int groupNum, int packageNum, bool[][] groups)
		{
			bool[][] copy = CopyArray(groups);
			copy[groupNum][packageNum] = true;
			return copy;
		}
		
		T[][] CopyArray<T>(T[][] array)
		{
			T[][] copy = new T[array.Length][];
			for(int i = 0; i < array.Length; i++)
			{
				T[] inner = new T[array[i].Length];
				array[i].CopyTo(inner, 0);
				copy[i] = inner;
			}
			return copy;
		}
		
		int GetGroupWeight(bool[] group, int[] packages)
		{
			int weight = 0;
			for(int usedPackage = 0; usedPackage < group.Length; usedPackage++)
				if(group[usedPackage])
					weight += packages[usedPackage];
			return weight;
		}
		
		int CountPackages(bool[] group)
		{
			int count = 0;
			foreach(bool usedPackage in group)
				if(usedPackage)
					count++;
			return count;
		}
		
		long CountQE(bool[] group, int[] packages)
		{
			try
			{
				long quantumEntanglement = -1;
				for(int usedPackage = 0; usedPackage < group.Length; usedPackage++)
				{
					if(group[usedPackage])
					{
						if(quantumEntanglement == -1)
							quantumEntanglement = packages[usedPackage];
						else
							quantumEntanglement *= packages[usedPackage];
					}
				}
				return quantumEntanglement;
			}
			catch(OverflowException)
			{
				return Int64.MaxValue;
			}
		}
		
		void PrintGroup(bool[] group, int[] packages)
		{
			for(int usedPackage = 0; usedPackage < group.Length; usedPackage++)
				if(group[usedPackage])
					Console.Write("{0} ", packages[usedPackage]);
			Console.WriteLine("| Weight: {0} QE: {1}", GetGroupWeight(group, packages), CountQE(group, packages));
		}
		
	}
}
