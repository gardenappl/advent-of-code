
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace AdventOfCode
{
	public class Day22 : Day
	{
		struct Player
		{
			public int Life;
			public int Mana;
			public int Shield;
			public int Recharge;
		}
		
		struct Boss
		{
			public int Life;
			public int Damage;
			public int Poison;
		}
		
		int[] manaCosts = {53, 73, 113, 173, 229};
		Random rnd = new Random();
		
		void Turn(Player player, Boss boss, int spell, ref int leastManaWin, bool playerTurn = true, int manaSpent = 0, bool hardMode = false)
		{
			//Update effects
			if(playerTurn && hardMode)
				if(player.Life-- <= 0)
					return;
			if(player.Shield > 0)
				player.Shield--;
			if(player.Recharge > 0)
			{
				player.Mana += 101;
				player.Recharge--;
			}
			if(boss.Poison > 0)
			{
				boss.Life -= 3;
				boss.Poison--;
				if(boss.Life <= 0)
				{
					leastManaWin = Math.Min(leastManaWin, manaSpent);
					return;
				}
			}
			
			//Do the things
			if(playerTurn)
			{
				if(player.Mana >= manaCosts[spell])
				{
					manaSpent += manaCosts[spell];
					player.Mana -= manaCosts[spell];
					switch(spell)
					{
						case 0: //Magic missile
							boss.Life -= 4;
							break;
						case 1: //Drain
							boss.Life -= 2;
							player.Life += 2;
							break;
						case 2: //Shield
							if(player.Shield > 0)
								return;
							player.Shield = 6;
							break;
						case 3: //Poison
							if(boss.Poison > 0)
								return;
							boss.Poison = 6;
							break;
						case 4: //Recharge
							if(player.Recharge > 0)
								return;
							player.Recharge = 5;
							break;
					}
				}
				else
					return;
			}
			else
				player.Life -= Math.Max(1, player.Shield > 0 ? boss.Damage - 7 : boss.Damage);
			
			
			//Check for deaths
			if(boss.Life <= 0)
			{
				leastManaWin = Math.Min(leastManaWin, manaSpent);
				return;
			}
			if(player.Life <= 0)
				return;
			
			//If it's already higher than the current minimal there's no point in continuing...
			if(manaSpent >= leastManaWin)
				return;
			
			//If nobody's dead, turn again!
			int[] spells = {0, 1, 2, 3, 4};
			
			//IMPORTANT: Randomize things. Otherwise you end up with infinite Magic Missile/Drain spam.
			spells.OrderBy(x => rnd.Next());
			if(playerTurn)  //If the next turn isn't the player than the next spell doesn't matter.
				Turn(player, boss, 0, ref leastManaWin, false, manaSpent, hardMode);
			else
				foreach(int i in spells)
					Turn(player, boss, i, ref leastManaWin, true, manaSpent, hardMode);
		}
		
		public override void Solve()
		{
			var player = new Player();
			player.Life = 50;
			player.Mana = 500;
			var boss = new Boss();
			string[] file = File.ReadAllLines("Day22Input.txt");
			boss.Life = Int32.Parse(file[0].Split(':')[1]);
			boss.Damage = Int32.Parse(file[1].Split(':')[1]);
			
			int leastMana = Int32.MaxValue;
			for(int spell = 0; spell < 5; spell++)
				Turn(player, boss, spell, ref leastMana);
			
			Console.WriteLine("Minimal mana: " + leastMana);
			
			leastMana = Int32.MaxValue;
			for(int spell = 0; spell < 5; spell++)
				Turn(player, boss, spell, ref leastMana, hardMode: true);
			
			Console.WriteLine("Switching to Hardmode...");
			Console.WriteLine("Minimal mana: " + leastMana);
		}
	}
}