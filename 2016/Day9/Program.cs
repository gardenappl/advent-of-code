
using System;
using System.IO;
using System.Text;

namespace Day9
{
	public class Program
	{
		public static void Main()
		{
			Part1();
//			Console.WriteLine(GetDecompressedLength("(25x3)(3x3)ABC(2x3)XY(5x2)PQRSTX(18x9)(3x2)TWO(5x7)SEVEN"));
			Console.WriteLine("Answer Part 2: " + GetDecompressedLength(File.ReadAllText("input.txt")));
			Console.ReadKey();
		}
		
		public static void Part1()
		{
			long resultLength = 0;
			int count = 0;
			int repeat = 0;
			bool markerCount = false;
			bool markerRepeat = false;
			string currentNum = "";
			using(var reader = new StreamReader("input.txt"))
			{
				while(!reader.EndOfStream)
				{
					char c = (char)reader.Read();
					if(c == '(')
					{
						markerCount = true;
						markerRepeat = false;
						continue;
					}
					if(c == 'x')
					{
						count = Int32.Parse(currentNum);
						currentNum = String.Empty;
						markerCount = false;
						markerRepeat = true;
						continue;
					}
					if(c == ')')
					{
						repeat = Int32.Parse(currentNum);
						currentNum = String.Empty;
//						var sb = new StringBuilder();
						for(int i = 0; i < count; i++)
						{
							reader.Read();
//							if(reader.Peek() != -1)
//							{
//							sb.Append((char)reader.Read());
//							}
						}
//						var s = sb.ToString();
//						Console.WriteLine(count + "x" + repeat);
//						Console.WriteLine(s);
//						Console.ReadKey();
						for(int i = 0; i < repeat; i++)
							resultLength += count;
//							result.Append(sb);
						markerCount = false;
						markerRepeat = false;
						continue;
					}
					if(markerCount || markerRepeat)
						currentNum += c;
//					else
//						resultLength++;
//					Console.WriteLine(resultLength);
//					Console.ReadKey();
				}
			}
			Console.WriteLine("Answer Part 1: " + resultLength);
		}
		
		public static long GetDecompressedLength(string input)
		{
			long length = 0;
			
			for(int i = 0; i < input.Length; i++)
			{
				if(input[i] == '(')
				{
					int count = 0;
					int repeat = 0;
					
					var sb = new StringBuilder();
					while(input[++i] != 'x')
					{
						sb.Append(input[i]);
					}
					count = Int32.Parse(sb.ToString());
					sb.Clear();
					while(input[++i] != ')')
					{
						sb.Append(input[i]);
					}
					repeat = Int32.Parse(sb.ToString());
					length += GetDecompressedLength(input.Substring(i + 1, count)) * repeat;
					i += count;
				}
				else if(!Char.IsWhiteSpace(input[i]))
					length++;
			}
			return length;
		}
	}
}
