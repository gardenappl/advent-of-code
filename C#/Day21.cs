
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day21 : Day
	{
		List<Item> shop = new List<Item>();
		
		internal class Item
		{
			public string Name;
			public int Cost;
			public int Damage;
			public int Defense;
			public ItemType Type;
			
			public Item(string name, int cost, int damage, int defense, ItemType type)
			{
				Name = name;
				Cost = cost;
				Damage = damage;
				Defense = defense;
				Type = type;
			}
		}
		
		internal enum ItemType
		{
			Weapon,
			Armor,
			Ring
		}
		
		internal struct Inventory
		{
			public Item Weapon;
			public Item Armor;
			public Item Ring1;
			public Item Ring2;
			
			public Inventory(Item weapon, Item armor, Item ring1, Item ring2)
			{
				Weapon = weapon;
				Armor = armor;
				Ring1 = ring1;
				Ring2 = ring2;
			}
			
			public int GetDamageBonus()
			{
				int damage = 0;
				if(Weapon != null)
				damage += Weapon.Damage;
				if(Ring1 != null)
				damage += Ring1.Damage;
				if(Ring2 != null)
				damage += Ring2.Damage;
				return damage;
			}
			
			public int GetDefenseBonus()
			{
				int defense = 0;
				if(Armor != null)
				defense += Armor.Defense;
				if(Ring1 != null)
				defense += Ring1.Defense;
				if(Ring2 != null)
				defense += Ring2.Defense;
				return defense;
			}
			
			public int GetTotalCost()
			{
				int cost = 0;
				if(Weapon != null)
					cost += Weapon.Cost;
				if(Armor != null)
				cost += Armor.Cost;
				if(Ring1 != null)
				cost += Ring1.Cost;
				if(Ring2 != null)
				cost += Ring2.Cost;
				return cost;
			}
		}
		
		internal class Entity
		{
			public string Name;
			public int Life;
			public int MaxLife;
			internal Inventory Items;
			public virtual int Damage
			{
				get{ return _Damage + Items.GetDamageBonus(); }
				set{ _Damage = value; }
			}
			int _Damage;
			public virtual int Defense
			{
				get{ return _Defense + Items.GetDefenseBonus(); }
				set{ _Defense = value; }
			}
			int _Defense;
			
			
			public Entity(string name, int maxLife, int damage, int defense)
			{
				Name = name;
				MaxLife = maxLife;
				Life = maxLife;
				Damage = damage;
				Defense = defense;
			}
			
			internal bool TryKill(Entity enemy, Inventory items)
			{
				Life = MaxLife;
				Items = items;
				enemy.Life = enemy.MaxLife;
				while(true)
				{
					Attack(enemy);
					if(enemy.Life <= 0)
						return true;
					enemy.Attack(this);
					if(Life <= 0)
						return false;
				}
			}
			
			public virtual void Attack(Entity enemy)
			{
				int dmg = Math.Max(1, Damage - enemy.Defense);
				enemy.Life -= dmg;
			}
		}
		
		public override void Solve()
		{
			string input = File.ReadAllText("Day21Input.txt");
			const string pattern = @"^Hit Points: (\d+)\s*Damage: (\d+)\s*Armor: (\d+)\s*$";
			var match = Regex.Match(input, pattern);
			
			var player = new Entity("Henry", 100, 0, 0);
			var boss = new Entity("The Boss", Int32.Parse(match.Groups[1].Value), Int32.Parse(match.Groups[2].Value), Int32.Parse(match.Groups[3].Value));
			
			shop.Add(new Item("Dagger", 8, 4, 0, ItemType.Weapon));
			shop.Add(new Item("Shortsword", 10, 5, 0, ItemType.Weapon));
			shop.Add(new Item("Warhammer", 25, 6, 0, ItemType.Weapon));
			shop.Add(new Item("Longsword", 40, 7, 0, ItemType.Weapon));
			shop.Add(new Item("Greataxe", 74, 8, 0, ItemType.Weapon));
			
			shop.Add(new Item("No armor", 0, 0, 0, ItemType.Armor));
			shop.Add(new Item("Leather", 13, 0, 1, ItemType.Armor));
			shop.Add(new Item("Chainmail", 31, 0, 2, ItemType.Armor));
			shop.Add(new Item("Splintmail", 53, 0, 3, ItemType.Armor));
			shop.Add(new Item("Bandedmail", 75, 0, 4, ItemType.Armor));
			shop.Add(new Item("Platemail", 102, 0, 5, ItemType.Armor));
			
			shop.Add(new Item("No ring", 0, 0, 0, ItemType.Ring));
			shop.Add(new Item("No ring", 0, 0, 0, ItemType.Ring));
			shop.Add(new Item("Damage +1", 25, 1, 0, ItemType.Ring));
			shop.Add(new Item("Damage +2", 50, 2, 0, ItemType.Ring));
			shop.Add(new Item("Damage +3", 100, 3, 0, ItemType.Ring));
			shop.Add(new Item("Defense +1", 20, 0, 1, ItemType.Ring));
			shop.Add(new Item("Defense +2", 40, 0, 2, ItemType.Ring));
			shop.Add(new Item("Defense +3", 80, 0, 3, ItemType.Ring));
			
			var combinations = 
				from w in shop
				where w.Type == ItemType.Weapon
				from a in shop
				where a.Type == ItemType.Armor
				from r1 in shop
				where r1.Type == ItemType.Ring
				from r2 in shop
				where r2.Type == ItemType.Ring && r2 != r1
				select new Inventory(w, a, r1, r2);
			
			Console.WriteLine("Most expensive loss: " + combinations.Where(c => !player.TryKill(boss, c)).Max(c => c.GetTotalCost()));
			Console.WriteLine("Least expensive win: " + combinations.Where(c => player.TryKill(boss, c)).Min(c => c.GetTotalCost()));
		}
	}
}
