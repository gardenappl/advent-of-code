
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day19 : Day
	{	
		const string pattern = @"^(\w+) => (\w+)$";
		
		public override void Solve()
		{
			List<Tuple<string, string>> replacements = new List<Tuple<string, string>>();
			List<Tuple<string, string>> testReplacements = new List<Tuple<string, string>>();
			string molecule = "";
		
			foreach(string line in File.ReadAllLines("Day19Input.txt"))
			{
				Match match = Regex.Match(line, pattern);
				if(match.Success)
					replacements.Add(Tuple.Create(match.Groups[1].Value, match.Groups[2].Value));
				else if(line.Length > 0)
					molecule = line;
			}
			testReplacements.Add(Tuple.Create("e", "O"));
			testReplacements.Add(Tuple.Create("e", "H"));
			testReplacements.Add(Tuple.Create("H", "HO"));
			testReplacements.Add(Tuple.Create("H", "OH"));
			testReplacements.Add(Tuple.Create("O", "HH"));
			
			
			Console.WriteLine("Calibration result: " + Calibrate(molecule, replacements));
			Console.WriteLine("Test fabrication result: " + Fabricate("HOH", testReplacements, print: true));
			Console.WriteLine("Actual fabrication result: " + Fabricate(molecule, replacements));
		}
		
		int Calibrate(string molecule, List<Tuple<string, string>> replacements)
		{
			HashSet<string> createdMolecules = new HashSet<string>();
			
			foreach(var replacement in replacements)
			{
				int index = -1;
				while((index = molecule.IndexOf(replacement.Item1, index + 1)) != -1)
				{
					string mol1 = molecule.Remove(index, replacement.Item1.Length);
					createdMolecules.Add(mol1.Insert(index, replacement.Item2));
				}
			}
			return createdMolecules.Count;
		}
		
		int Fabricate(string molecule, List<Tuple<string, string>> replacements, int steps = 0, bool print = false)
		{
			if(molecule == "e")
				return steps;
			
			foreach(var replacement in replacements)
			{
				int index = -1;
				while((index = molecule.IndexOf(replacement.Item2, index + 1)) != -1)
				{
					string moleculeN = molecule.Remove(index, replacement.Item2.Length).Insert(index, replacement.Item1);
					if(print)
					{
						for(int i = 0; i < steps; i++)
							Console.Write(' ');
						Console.WriteLine(molecule + " -> " + moleculeN);
					}
					int result = Fabricate(moleculeN, replacements, steps + 1, print);
					if(result != -1)
						return result;
				}
			}
			return -1;
		}
	}
}
