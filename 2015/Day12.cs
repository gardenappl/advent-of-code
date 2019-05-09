
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace AdventOfCode
{
	public class Day12 : Day
	{
		public override void Solve()
		{
//			var nums = new List<int>();
//			var currentNum = new StringBuilder();
//			
//			foreach(char c in File.ReadAllText("Day12Input.txt"))
//			{
//				if(char.IsDigit(c) || c == '-')
//					currentNum.Append(c);
//				else if(currentNum.Length != 0)
//				{
//					Console.WriteLine(Convert.ToInt32(currentNum.ToString()));
//					nums.Add(Convert.ToInt32(currentNum.ToString()));
//					currentNum.Clear();
//				}
//			}
//			
//			Console.WriteLine(nums.Sum(x => x));
			int index = 0;
			var json = JsonElement.Parse(File.ReadAllText("Day12Input.txt"), ref index);
			
			Console.WriteLine(SearchNums(json, false));
			Console.WriteLine(SearchNums(json, true));
		}
		
		int SearchNums(JsonElement element, bool ignoreRed)
		{
			int currentSum = 0;
			SearchNumsRecursive(element, ignoreRed, ref currentSum);
			return currentSum;
		}
		
		void SearchNumsRecursive(JsonElement element, bool ignoreRed, ref int currentSum)
		{
			if(element is JsonArray)
				foreach(var member in ((JsonArray)element).Values)
					SearchNumsRecursive(member, ignoreRed, ref currentSum);
			else if(element is JsonNumber)
				currentSum += ((JsonNumber)element).Value;
			else if(element is JsonObject)
			{
				int childSum = 0;
				bool ignore = false;
				foreach(var member in ((JsonObject)element).Elements.Values)
				{
					if(ignoreRed && member is JsonString && ((JsonString)member).Value == "red")
					{
						ignore = true;
						break;
					}
					SearchNumsRecursive(member, ignoreRed, ref childSum);
				}
				if(!ignore)
					currentSum += childSum;
			}
		}
		
		class JsonElement
		{
			public static JsonElement Parse(string document, ref int index)
			{
				switch(document[index])
				{
					case '{':
						return new JsonObject(document, ref index);
					case '[':
						return new JsonArray(document, ref index);
					case '-':
						return new JsonNumber(document, ref index);
					case '"':
						return new JsonString(document, ref index);
					default:
						if(char.IsDigit(document[index]))
							return new JsonNumber(document, ref index);
						throw new Exception(document + " " + index);
				}
			}
		}
		
		class JsonObject : JsonElement
		{
			public Dictionary<string, JsonElement> Elements;
			
			public JsonObject(string document, ref int index)
			{
				Elements = new Dictionary<string, JsonElement>();
				
				JsonParser.Eat(document, ref index, '{');
				
				do
				{
					JsonParser.SkipWhiteSpace(document, ref index);
					string value = new JsonString(document, ref index).Value;
					
					JsonParser.Eat(document, ref index, ':');
					JsonParser.SkipWhiteSpace(document, ref index);
					Elements.Add(value, JsonElement.Parse(document, ref index));
				}while(JsonParser.Eat(document, ref index, ',', false));
				JsonParser.Eat(document, ref index, '}');
			}
		}
		
		class JsonNumber : JsonElement
		{
			public int Value;
			
			public JsonNumber(string document, ref int index)
			{
				var sb = new StringBuilder();
				while(char.IsDigit(document[index]) || document[index] == '-')
				{
					sb.Append(document[index]);
					index++;
				}
				Value = Int32.Parse(sb.ToString());
			}
		}
		
		class JsonArray : JsonElement
		{
			public List<JsonElement> Values;
			
			public JsonArray(string document, ref int index)
			{
				Values = new List<JsonElement>();
				JsonParser.Eat(document, ref index, '[');
				do
				{
					JsonParser.SkipWhiteSpace(document, ref index);
					Values.Add(JsonElement.Parse(document, ref index));
				} while(JsonParser.Eat(document, ref index, ',', false));
				JsonParser.Eat(document, ref index, ']');
			}
		}
		
		class JsonString : JsonElement
		{
			public string Value;
			
			public JsonString(string document, ref int index)
			{
				var sb = new StringBuilder();
				
				JsonParser.Eat(document, ref index, '"');
				while(document[index] != '"')
				{
					sb.Append(document[index]);
					index++;
				}
				JsonParser.Eat(document, ref index, '"');
				
				Value = sb.ToString();
			}
		}
		
		class JsonParser
		{
			public static void SkipWhiteSpace(string document, ref int index)
			{
				while(char.IsWhiteSpace(document[index]))
					index++;
			}
			
			public static bool Eat(string document, ref int index, char c, bool throwException = true)
			{
				if(document[index] == c)
				{
					index++;
					return true;
				}
				if(throwException)
				{
					Console.WriteLine(document.Substring(0, index + 1));
					Console.WriteLine(document[index]);
					throw new ArgumentException(string.Format("Error while parsing JSON object: at index {0} expected {1}", index, c));
				}
				else
					return false;
			}
		}
	}
}
