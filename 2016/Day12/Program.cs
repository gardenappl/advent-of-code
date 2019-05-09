
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace Day12
{
	public class Program
	{
		static Dictionary<char, int> Registers = new Dictionary<char, int>();
		static List<Instruction> Instructions = new List<Instruction>();
		static int Index;
		
		public static void Main()
		{
			var cpyPattern = new Regex(@"cpy (\D) (\D)");
			var cpyIntPattern = new Regex(@"cpy (-?\d+) (\D)");
			var incPattern = new Regex(@"inc (\D)");
			var decPattern = new Regex(@"dec (\D)");
			var jnzPattern = new Regex(@"jnz (\D) (-?\d+)");
			var jnzIntPattern = new Regex(@"jnz (-?\d+) (-?\d+)");
			
			using(var reader = new StreamReader("input.txt"))
			{
				while(!reader.EndOfStream)
				{
					string line = reader.ReadLine();
					var match = cpyPattern.Match(line);
					if(match.Success)
					{
						char register1 = match.Groups[1].Value[0];
						char register2 = match.Groups[2].Value[0];
						Instructions.Add(new CopyInstruction(register1, register2));
						if(!Registers.ContainsKey(register1))
							Registers[register1] = 0;
						if(!Registers.ContainsKey(register2))
							Registers[register2] = 0;
						continue;
					}
					match = cpyIntPattern.Match(line);
					if(match.Success)
					{
						int value = Int32.Parse(match.Groups[1].Value);
						char register = match.Groups[2].Value[0];
						Instructions.Add(new IntCopyInstruction(value, register));
						if(!Registers.ContainsKey(register))
							Registers[register] = 0;
					}
					match = incPattern.Match(line);
					if(match.Success)
					{
						char register = match.Groups[1].Value[0];
						Instructions.Add(new IncrementInstruction(register));
						if(!Registers.ContainsKey(register))
							Registers[register] = 0;
					}
					match = decPattern.Match(line);
					if(match.Success)
					{
						char register = match.Groups[1].Value[0];
						Instructions.Add(new DecrementInstruction(register));
						if(!Registers.ContainsKey(register))
							Registers[register] = 0;
					}
					match = jnzPattern.Match(line);
					if(match.Success)
					{
						char register = match.Groups[1].Value[0];
						int value = Int32.Parse(match.Groups[2].Value);
						Instructions.Add(new JumpInstruction(register, value));
						if(!Registers.ContainsKey(register))
							Registers[register] = 0;
					}
					match = jnzIntPattern.Match(line);
					if(match.Success)
					{
						int value1 = Int32.Parse(match.Groups[1].Value);
						int value2 = Int32.Parse(match.Groups[2].Value);
						Instructions.Add(new IntJumpInstruction(value1, value2));
					}
				}
			}
			while(Index < Instructions.Count)
			{
//				Console.WriteLine(Index + 1);
				Instructions[Index].Execute();
//				foreach(var kvp in Registers)
//					Console.WriteLine(kvp);
//				Console.ReadKey();
			}
			Console.WriteLine("a: " + Registers['a']);
			Index = 0;
			Registers['a'] = 0;
			Registers['b'] = 0;
			Registers['c'] = 1;
			Registers['d'] = 0;
			Console.WriteLine("Resetting, c=1");
			while(Index < Instructions.Count)
			{
				Instructions[Index].Execute();
			}
			Console.WriteLine("a: " + Registers['a']);
			Console.ReadKey();
		}
		
		class IntCopyInstruction : Instruction
		{
			int Value;
			
			public IntCopyInstruction(int value, char register) : base(register)
			{
				Value = value;
			}
			
			public override void Execute()
			{
				Registers[Register] = Value;
				Index++;
			}
		}
		
		class CopyInstruction : Instruction
		{
			char Register2;
			
			public CopyInstruction(char register1, char register2) : base(register1)
			{
				Register2 = register2;
			}
			
			public override void Execute()
			{
				Registers[Register2] = Registers[Register];
				Index++;
			}
		}
		
		class IncrementInstruction : Instruction
		{
			public IncrementInstruction(char register) : base(register) {}
			
			public override void Execute()
			{
				Registers[Register]++;
				Index++;
			}
		}
		
		class DecrementInstruction : Instruction
		{
			public DecrementInstruction(char register) : base(register) {}
			
			public override void Execute()
			{
				Registers[Register]--;
				Index++;
			}
		}
		
		class JumpInstruction : Instruction
		{
			int Value;
			
			public JumpInstruction(char register, int value) : base(register)
			{
				Register = register;
				Value = value;
			}
			
			public override void Execute()
			{
				if(Registers[Register] != 0)
					Index += Value;
				else
					Index++;
			}
		}
		
		class IntJumpInstruction : Instruction
		{
			int Value;
			bool Jump;
			
			public IntJumpInstruction(int value1, int value2) : base(' ')
			{
				Jump = value1 != 0;
				Value = value2;
			}
			
			public override void Execute()
			{
				if(Jump)
					Index += Value;
				else
					Index++;
			}
		}
		
		class Instruction
		{
			protected char Register;
			
			protected Instruction(char register)
			{
				Register = register;
			}
			
			public virtual void Execute()
			{
				Index++;
			}
		}
	}
}
