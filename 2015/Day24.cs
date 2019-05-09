
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
		
		BigInteger FindBestCombo(int[] packages, int groups)
		{
			packages = packages.OrderByDescending(x => x).ToArray();
			int groupWeight = packages.Sum(x => x) / groups;
			
			BigInteger smallestQE = -1;
			int smallest1 = packages.Count() / groups + 1;
			FindBestComboRecursive(packages, groupWeight, groups, new int[packages.Length], ref smallestQE, ref smallest1);
			return smallestQE;
		}
		
		/* A "group" array contains has the same length as "packages" and it's integers are stored in the same index as in "packages".
		 * For example: if the packages are {3, 4, 1, 19} then a group containing the second and last package will be {0, 4, 0, 19}.
	     * Maybe it's overengineering but I wanted this program to work with inputs where there are multiple packages with the same size.
		 */
		void FindBestComboRecursive(int[] packages, int neededWeight, int groupsAmount, int[] firstGroup, ref BigInteger smallestQE, ref int smallest1, int index = 0)
		{
			if(CountPackages(firstGroup) > smallest1)
				return;
			
			if(index == packages.Length)
			{
				if(GetGroupWeight(firstGroup) != neededWeight)
					return;
				
				int[][] groups = new int[groupsAmount][];
				groups[0] = firstGroup;
				for(int i = 1; i < groupsAmount; i++)
					groups[i] = new int[packages.Length];
				if(!CanDistributeOtherGroups(packages, 0, groups))
				{
					Console.Write('|');
					return;
				}
				
				Console.Write('.');
				var QE = CountQE(firstGroup);
				int count = CountPackages(firstGroup);
				if(count < smallest1)
				{
					smallest1 = count;
					smallestQE = QE;
					return;
				}
				else if(count == smallest1)
					if(QE < smallestQE || smallestQE == -1)
						smallestQE = QE;
				return;
			}
			
			//Try adding the next package
			FindBestComboRecursive(packages, neededWeight, groupsAmount, AddPackage(index, packages[index], firstGroup), ref smallestQE, ref smallest1, index + 1);
			//Try not adding the next package
			FindBestComboRecursive(packages, neededWeight, groupsAmount, firstGroup, ref smallestQE, ref smallest1, index + 1);
		}
		
		bool CanDistributeOtherGroups(int[] packages, int index, params int[][] groups)
		{
			int[] weights = new int[groups.Length];
			
			for(int i = 0; i < weights.Length; i++)
			{
				weights[i] = GetGroupWeight(groups[i]);
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
			
			if(groups[0][index] != 0) //If current package is already in group 1
				if(CanDistributeOtherGroups(packages, index + 1, CopyArray(groups))) //Iterate more and don't put anything to groups 2 or 3
					return true;
			
			for(int i = 1; i < groups.Length; i++)
				if(CanDistributeOtherGroups(packages, index + 1, AddPackage(i, index, packages[index], groups)))
					return true;
			return false;
		}
		
		int[] AddPackage(int index, int package, int[] group)
		{
			int[] copy = new int[group.Length];
			Array.Copy(group, copy, group.Length);
			copy[index] = package;
			return copy;
		}
		
		int[][] AddPackage(int groupNum, int index, int package, int[][] groups)
		{
			int[][] copy = CopyArray(groups);
			copy[groupNum][index] = package;
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
		
		int GetGroupWeight(int[] group)
		{
			int weight = 0;
			foreach(int package in group)
				if(package != 0)
					weight += package;
			return weight;
		}
		
		int CountPackages(int[] group)
		{
			int count = 0;
			foreach(int package in group)
				if(package != 0)
					count++;
			return count;
		}
		
		BigInteger CountQE(int[] group)
		{
			BigInteger quantumEntanglement = -1;
			foreach(int package in group)
			{
				if(package != 0)
				{
					if(quantumEntanglement == -1)
						quantumEntanglement = package;
					else
						quantumEntanglement *= package;
				}
			}
			return quantumEntanglement;
		}
		
		void PrintGroup(int[] group)
		{
			Console.Write(string.Join(" ", group));
			Console.WriteLine("| Weight: {0} QE: {1}", GetGroupWeight(group), CountQE(group));
		}
		
	}
}