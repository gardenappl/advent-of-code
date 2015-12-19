
using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace AdventOfCode
{
	public class Day15 : Day
	{
		static List<Ingredient> ingredients = new List<Ingredient>();
		
		public struct Ingredient
		{
			public string Name;
			public int Capacity;
			public int Durability;
			public int Flavor;
			public int Texture;
			public int Calories;
			
			public static Ingredient Parse(string line)
			{
				var ingredient = new Ingredient();
				
				string pattern = @"^(\w+): capacity (-?\d+), durability (-?\d+), flavor (-?\d+), texture (-?\d+), calories (-?\d+)$";
				
				if(!Regex.IsMatch(line, pattern))
					return new Ingredient();
				Match match = Regex.Match(line, pattern);
				
				ingredient.Name = match.Groups[1].Value;
				ingredient.Capacity = Int32.Parse(match.Groups[2].Value);
				ingredient.Durability = Int32.Parse(match.Groups[3].Value);
				ingredient.Flavor = Int32.Parse(match.Groups[4].Value);
				ingredient.Texture = Int32.Parse(match.Groups[5].Value);
				ingredient.Calories = Int32.Parse(match.Groups[6].Value);
				return ingredient;
			}
		}
		
		public override void Solve()
		{
			foreach(string line in File.ReadAllLines("Day15Input.txt"))
				ingredients.Add(Ingredient.Parse(line));
			Console.WriteLine("Best cookie score: " + LoopRecipes(new int[ingredients.Count]));
			Console.WriteLine("Best cookie score with 500 calories: " + LoopRecipes(new int[ingredients.Count], requiredCalories: 500));
		}
		
		static int LoopRecipes(int[] ingredientAmounts, int spoonsLeft = 100, int currentIngredient = 0, int requiredCalories = -1)
		{
			if(currentIngredient == ingredientAmounts.Length - 1)
			{
				ingredientAmounts[currentIngredient] = spoonsLeft;
				return CalculateScore(ingredientAmounts, requiredCalories);
			}
			int bestScore = 0;
			for(int i = 0; i <= spoonsLeft; i++)
			{
				ingredientAmounts[currentIngredient] = i;
				bestScore = Math.Max(bestScore, LoopRecipes(ingredientAmounts, spoonsLeft - i, currentIngredient + 1, requiredCalories));
			}
			return bestScore;
		}
		
		static int CalculateScore(int[] ingredientAmounts, int requiredCalories = -1 /* no limit */)
		{
			if(ingredientAmounts.Length > ingredients.Count)
				throw new ArgumentException("Trying to make a cookie with unknown ingredients!", new IndexOutOfRangeException());
			int capacity = 0;
			int durability = 0;
			int flavor = 0;
			int texture = 0;
			int calories = 0;
			for(int i = 0; i < ingredientAmounts.Length; i++)
			{
				capacity += ingredients[i].Capacity * ingredientAmounts[i];
				durability += ingredients[i].Durability * ingredientAmounts[i];
				flavor += ingredients[i].Flavor * ingredientAmounts[i];
				texture += ingredients[i].Texture * ingredientAmounts[i];
				calories += ingredients[i].Calories * ingredientAmounts[i];
			}
			if(capacity <= 0 || durability <= 0 || flavor <= 0 || texture <= 0)
				return 0;
			else if(requiredCalories == -1 || requiredCalories == calories)
				return capacity * durability * flavor * texture;
			else
				return 0;
		}
	}
}
