
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace AdventOfCode
{
	public class Day17 : Day
	{
		static readonly int[] containers = File.ReadAllLines("Day17Input.txt").Select(line => Int32.Parse(line)).ToArray();
		const int eggnog = 150;
		
		public override void Solve()
		{
			var comboList = new List<bool[]>();
			GetPossibleCombinations(comboList, new bool[containers.Length]);
			
//			foreach(bool[] combo in comboList)
//				PrintCombination(combo);
			
			Console.WriteLine("Possible combinations: " + comboList.Count);
			
			int minContainersUsed = Int32.MaxValue;
			int minContaierCombos = 0;
			foreach(bool[] combo in comboList)
			{
				int containersUsed = 0;
				foreach(bool container in combo)
					if(container)
						containersUsed++;
				
				if(containersUsed < minContainersUsed)
				{
					minContainersUsed = containersUsed;
					minContaierCombos = 1;
				}
				else if(containersUsed == minContainersUsed)
					minContaierCombos++;
			}
			
			Console.WriteLine("Combinations with minimal ({0}) containers: {1}", minContainersUsed, minContaierCombos);
		}
		
		static void GetPossibleCombinations(List<bool[]> comboList, bool[] currentCombo, int currentIndex = 0)
		{
			if(currentIndex == currentCombo.Length - 1)
			{
				currentCombo[currentIndex] = true;
				if(CombinationWorks(currentCombo))
					comboList.Add(CopyArray(currentCombo));
				
				currentCombo[currentIndex] = false;
				if(CombinationWorks(currentCombo))
					comboList.Add(CopyArray(currentCombo));
				return;
			}
			
			currentCombo[currentIndex] = true;
			GetPossibleCombinations(comboList, CopyArray(currentCombo), currentIndex + 1);
			
			currentCombo[currentIndex] = false;
			GetPossibleCombinations(comboList, CopyArray(currentCombo), currentIndex + 1);
		}
		
		static T[] CopyArray<T>(T[] combo)
		{
			T[] newCombo = new T[combo.Length];
			for(int i = 0; i < combo.Length; i++)
				newCombo[i] = combo[i];
			return newCombo;
		}
		
		static void PrintCombination(bool[] combo)
		{
			for(int i = 0; i < combo.Length; i++)
				if(combo[i])
					Console.Write("{0} ", containers[i]);
			Console.WriteLine();
		}
		
		static bool CombinationWorks(bool[] combo)
		{
			int totalStorage = 0;
			for(int i = 0; i < combo.Length; i++)
				if(combo[i])
					totalStorage += containers[i];
			return totalStorage == eggnog;
		}
	}
}
