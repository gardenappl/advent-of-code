
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day7 : Day
	{
		static readonly Dictionary<string, Wire> wires = new Dictionary<string, Wire>();
		
		internal class Wire
		{
			public ushort Value
			{
				get
				{
					if(valueKnown)
						return _value;
					else
						return ComputeValue();
				}
				set
				{
					valueKnown = true;
					_value = value;
				}
			}
			
			readonly string Input1;
			readonly string Operation;
			readonly string Input2;
			public readonly string Name;
			
			bool valueKnown;
			ushort _value;
			
			public Wire(string line)
			{
				const string pattern = @"^(?<input1>[a-z]*\d*) ?(?<op>AND|OR|RSHIFT|LSHIFT|NOT|) ?(?<input2>[a-z]*\d*) -> (?<name>[a-z]+)$";
				var match = Regex.Match(line, pattern);
				
				Input1 = match.Groups["input1"].Value;
				Operation = match.Groups["op"].Value;
				Input2 = match.Groups["input2"].Value;
				Name = match.Groups["name"].Value;
			}
			
			public ushort ComputeValue()
			{
				if(string.IsNullOrEmpty(Operation))
				{
					Value = ParseWireOrSignal(Input1);
					return Value;
				}
				switch(Operation)
				{
					case "AND":
						Value = (ushort)(ParseWireOrSignal(Input1) & ParseWireOrSignal(Input2));
						return Value;
					case "OR":
						Value = (ushort)(ParseWireOrSignal(Input1) | ParseWireOrSignal(Input2));
						return Value;
					case "LSHIFT":
						Value = (ushort)(ParseWireOrSignal(Input1) << ParseWireOrSignal(Input2));
						return Value;
					case "RSHIFT":
						Value = (ushort)(ParseWireOrSignal(Input1) >> ParseWireOrSignal(Input2));
						return Value;
					case "NOT":
						Value = (ushort)(UInt16.MaxValue - ParseWireOrSignal(Input2));
						return Value;
				}
				return 0;
			}
			
			public void ResetValue()
			{
				Value = 0;
				valueKnown = false;
			}
			
			static ushort ParseWireOrSignal(string name)
			{
				try
				{
					return UInt16.Parse(name);
				}
				catch(Exception)
				{
					if(wires.ContainsKey(name))
						return wires[name].Value;
					throw new ArgumentException(string.Format("Wire input either has to be an existing wire name or a number! (was {0})", name), "name");
				}
			}
		}
		
		
		public override void Solve()
		{
			using(var file = new StreamReader("Day7Input.txt"))
			{
				do
				{
					var wire = new Wire(file.ReadLine());
					wires.Add(wire.Name, wire);
				}while(!file.EndOfStream);
			}
			ushort a = wires["a"].Value;
			Console.WriteLine("Value of wire a: " + a);
			
			Console.WriteLine("Assigning value of wire a to wire b and resetting everything else");
			foreach(var wire in wires.Values)
				wire.ResetValue();
			wires["b"].Value = a;
			
			Console.WriteLine("New value of wire a: " + wires["a"].Value);
		}
	}
}
