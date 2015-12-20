
using System;
using System.Collections.Generic;
using System.IO;

namespace AdventOfCode
{
	public class Day16 : Day
	{
		struct Sue
		{
			public readonly int Children;
			public readonly int Cats;
			public readonly int Samoyeds;
			public readonly int Pomeranians;
			public readonly int Akitas;
			public readonly int Vizslas;
			public readonly int Goldfish;
			public readonly int Trees;
			public readonly int Cars;
			public readonly int Perfumes;
			
			public Sue(string line)
			{
				string info = line.Substring(line.IndexOf(':') + 1);
				string[] infoParts = info.Split(',');
				Children = -1;
				Cats = -1;
				Samoyeds = -1;
				Pomeranians = -1;
				Akitas = -1;
				Vizslas = -1;
				Goldfish = -1;
				Trees = -1;
				Cars = -1;
				Perfumes = -1;
				foreach(string part in infoParts)
				{
					string[] s = part.Split(':');
					switch(s[0])
					{
						case " children":
							Children = Int32.Parse(s[1]);
							break;
						case " cats":
							Cats = Int32.Parse(s[1]);
							break;
						case " samoyeds":
							Samoyeds = Int32.Parse(s[1]);
							break;
						case " pomeranians":
							Pomeranians = Int32.Parse(s[1]);
							break;
						case " akitas":
							Akitas = Int32.Parse(s[1]);
							break;
						case " vizslas":
							Vizslas = Int32.Parse(s[1]);
							break;
						case " goldfish":
							Goldfish = Int32.Parse(s[1]);
							break;
						case " trees":
							Trees = Int32.Parse(s[1]);
							break;
						case " cars":
							Cars = Int32.Parse(s[1]);
							break;
						case " perfumes":
							Perfumes = Int32.Parse(s[1]);
							break;
					}
				}
			}
		}
		
		readonly List<Sue> auntSues = new List<Sue>();
		const int needChildren = 3;
		const int needCats = 7;
		const int needSamoyeds = 2;
		const int needPomeranians = 3;
		const int needAkitas = 0;
		const int needVizslas = 0;
		const int needGoldfish = 5;
		const int needTrees = 3;
		const int needCars = 2;
		const int needPerfumes = 1;
		
		public override void Solve()
		{
			using(var file = new StreamReader("Day16Input.txt"))
			{
				do
				{
					auntSues.Add(new Sue(file.ReadLine()));
				}while (!file.EndOfStream);
			}
			
			int auntSue1 = 0;
			
			for(int i = 1; i <= auntSues.Count; i++)
			{
				var aunt = auntSues[i - 1];
				if(needAkitas != aunt.Akitas && aunt.Akitas != -1)
					continue;
				if(needCars != aunt.Cars && aunt.Cars != -1)
					continue;
				if(needCats != aunt.Cats && aunt.Cats != -1)
					continue;
				if(needChildren != aunt.Children && aunt.Children != -1)
					continue;
				if(needGoldfish != aunt.Goldfish && aunt.Goldfish != -1)
					continue;
				if(needPerfumes != aunt.Perfumes && aunt.Perfumes != -1)
					continue;
				if(needPomeranians != aunt.Pomeranians && aunt.Pomeranians != -1)
					continue;
				if(needSamoyeds != aunt.Samoyeds && aunt.Samoyeds != -1)
					continue;
				if(needTrees != aunt.Trees && aunt.Trees != -1)
					continue;
				if(needVizslas != aunt.Vizslas && aunt.Vizslas != -1)
					continue;
				auntSue1 = i;
			}
			Console.WriteLine("The aunt Sue that sent the gift 1: " + auntSue1);
			
			
			int auntSue2 = 0;
			
			for(int i = 1; i <= auntSues.Count; i++)
			{
				var aunt = auntSues[i - 1];
				if(needAkitas != aunt.Akitas && aunt.Akitas != -1)
					continue;
				if(needCars != aunt.Cars && aunt.Cars != -1)
					continue;
				if(needCats >= aunt.Cats && aunt.Cats != -1)
					continue;
				if(needChildren != aunt.Children && aunt.Children != -1)
					continue;
				if(needGoldfish <= aunt.Goldfish && aunt.Goldfish != -1)
					continue;
				if(needPerfumes != aunt.Perfumes && aunt.Perfumes != -1)
					continue;
				if(needPomeranians <= aunt.Pomeranians && aunt.Pomeranians != -1)
					continue;
				if(needSamoyeds != aunt.Samoyeds && aunt.Samoyeds != -1)
					continue;
				if(needTrees >= aunt.Trees && aunt.Trees != -1)
					continue;
				if(needVizslas != aunt.Vizslas && aunt.Vizslas != -1)
					continue;
				auntSue2 = i;
			}
			Console.WriteLine("The aunt Sue that sent the gift 2: " + auntSue2);
		}
	}
}
