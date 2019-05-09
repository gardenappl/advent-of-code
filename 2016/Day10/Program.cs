
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace Day10
{
	public class Program
	{
		static Dictionary<int, Bot> Bots = new Dictionary<int, Bot>();
		static Dictionary<int, int> Outputs = new Dictionary<int, int>();
		static int ImportantBot = -1;
		public static int ImportantChip1;
		public static int ImportantChip2;
		
		public static void Main(string[] args)
		{
			if(args.Length != 2 || !Int32.TryParse(args[0], out ImportantChip1) || !Int32.TryParse(args[1], out ImportantChip2))
				throw new ArgumentException("Invalid chip arguments");
			
//			Bots.Add(0, new Bot(new Instruction(false, 2), new Instruction(false, 0)));
//			Bots.Add(1, new Bot(new Instruction(false, 1), new Instruction(true, 0)));
//			Bots.Add(2, new Bot(new Instruction(true, 1), new Instruction(true, 0)));
//			Bots[1].ReceiveChip(3);
//			Bots[2].ReceiveChip(2);
//			Bots[2].ReceiveChip(5);
			
			var execute = new List<Bot>();
			
			var instPattern = new Regex(@"bot (\d+) gives low to (bot|output) (\d+) and high to (bot|output) (\d+)");
			var chipPattern = new Regex(@"value (\d+) goes to bot (\d+)");
			using(var reader = new StreamReader("input.txt"))
			{
				while(!reader.EndOfStream)
				{
					string line = reader.ReadLine();
					var match = instPattern.Match(line);
					if(match.Success)
					{
						int botID = Int32.Parse(match.Groups[1].Value);
						bool outputsToBot1 = match.Groups[2].Value == "bot";
						int output1 = Int32.Parse(match.Groups[3].Value);
						bool outputsToBot2 = match.Groups[4].ToString() == "bot";
						int output2 = Int32.Parse(match.Groups[5].Value);
						if(!Bots.ContainsKey(botID))
							Bots[botID] = new Bot(new Instruction(outputsToBot1, output1), new Instruction(outputsToBot2, output2));
						else
						{
							Bots[botID].InstLow = new Instruction(outputsToBot1, output1);
							Bots[botID].InstHigh = new Instruction(outputsToBot2, output2);
						}
					}
					match = chipPattern.Match(line);
					if(match.Success)
					{
						int chip = Int32.Parse(match.Groups[1].Value);
						int botID = Int32.Parse(match.Groups[2].Value);
						if(!Bots.ContainsKey(botID))
							Bots.Add(botID, new Bot());
						if(Bots[botID].ChipLow != -1)
							execute.Add(Bots[botID]);
						Bots[botID].ReceiveChip(chip, false);
					}
				}
			}
			
			foreach(var bot in execute)
				bot.Execute();
			Console.WriteLine("Answer Part 1: " + ImportantBot);
			
//			foreach(var kvp in Outputs)
//				Console.WriteLine("{0}: {1}", kvp.Key, kvp.Value);
			Console.WriteLine("Answer Part 2: " + Outputs[0] * Outputs[1] * Outputs[2]);
			
			Console.ReadKey();
		}
		
		class Bot
		{
			public int ChipLow = -1;
			public int ChipHigh = -1;
			public Instruction InstLow;
			public Instruction InstHigh;
			
			public Bot() {}
			
			public Bot(Instruction instLow, Instruction instHigh)
			{
				InstLow = instLow;
				InstHigh = instHigh;
			}
			
			public bool ReceiveChip(int chip, bool recursiveExecute = true)
			{
				bool success = false;
				if(ChipLow == -1)
				{
					ChipLow = chip;
					return false;
				}
				if((ChipLow == ImportantChip1 && chip == ImportantChip2) ||
				   (chip == ImportantChip1 && ChipLow == ImportantChip2))
					success = true;
				if(ChipLow < chip)
					ChipHigh = chip;
				else
				{
					ChipHigh = ChipLow;
					ChipLow = chip;
				}
				if(recursiveExecute)
					Execute();
				return success;
			}
			
			public void Execute()
			{
				InstLow.Execute(ref ChipLow);
				InstHigh.Execute(ref ChipHigh);
			}
		}
		
		struct Instruction
		{
			public bool OutputsToBot;
			public int Output;
			
			public Instruction(bool toBot, int output)
			{
				OutputsToBot = toBot;
				Output = output;
			}
			
			public void Execute(ref int chip)
			{
				int tmpChip = chip;
				chip = -1;
				if(OutputsToBot)
				{
					if(Bots[Output].ReceiveChip(tmpChip) && ImportantBot == -1)
						ImportantBot = Output;
					return;
				}
				Outputs[Output] = tmpChip;
			}
		}
	}
}
