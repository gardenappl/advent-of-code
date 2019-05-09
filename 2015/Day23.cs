using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day23 : Day
	{
		struct Instruction
		{
			public string Type;
			public string Operand1;
			public string Operand2;
			
			public Instruction(string line)
			{
				const string pattern = @"^(hlf|tpl|inc|jmp|jie|jio) ([\w\d+-]+),? ?([\w\d+-]*)$";
				var match = Regex.Match(line, pattern);
				Type = match.Groups[1].Value;
				Operand1 = match.Groups[2].Value;
				Operand2 = match.Groups[3].Value;
			}
		}
		
		
		public override void Solve()
		{
			Dictionary<string, uint> registers = new Dictionary<string, uint>();
			List<Instruction> instructions = new List<Instruction>();
			
			string[] file = File.ReadAllLines("Day23Input.txt");
			foreach(string line in file)
				instructions.Add(new Instruction(line));
			
			Console.WriteLine("a = 0; b = 0");
			registers.Add("a", 0);
			registers.Add("b", 0);
			Console.WriteLine("Executing program...");
			ReadInstructions(instructions, registers);
			Console.WriteLine("Value of b: " + registers["b"]);
			
			Console.WriteLine("a = 1; b = 0");
			registers["a"] = 1;
			registers["b"] = 0;
			Console.WriteLine("Executing program...");
			ReadInstructions(instructions, registers);
			Console.WriteLine("Value of b: " + registers["b"]);
		}
		
		void ReadInstructions(List<Instruction> instructions, Dictionary<string, uint> registers)
		{
			int index = 0;
			do
			{
				var instruction = instructions[index];
				try
				{
				switch(instruction.Type)
				{
					case "hlf":
						registers[instruction.Operand1] /= 2;
						index++;
						break;
					case "tpl":
						registers[instruction.Operand1] *= 3;
						index++;
						break;
					case "inc":
						registers[instruction.Operand1]++;
						index++;
						break;
					case "jmp":
						index += Int32.Parse(instruction.Operand1);
						break;
					case "jie":
						if(registers[instruction.Operand1] % 2 == 0)
							index += Int32.Parse(instruction.Operand2);
						else
							index++;
						break;
					case "jio":
						if(registers[instruction.Operand1] == 1)
							index += Int32.Parse(instruction.Operand2);
						else
							index++;
						break;
				}
				}
				catch(Exception)
				{
					Console.WriteLine("Error at line " + index);
					if(string.IsNullOrEmpty(instruction.Operand2))
						Console.WriteLine("{0} {1}", instruction.Type, instruction.Operand1);
					else
						Console.WriteLine("{0} {1}, {2}", instruction.Type, instruction.Operand1, instruction.Operand2);
				}
			}while(index < instructions.Count);
		}
	}
}

