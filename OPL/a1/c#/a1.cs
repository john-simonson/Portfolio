// used http://rosettacode.org/wiki/Combinations#C.23 for reference
using System; // equivalent to using namespace in c++
class Combinations //define combination class
{
	static int k;
	static int n;
	static int [] buf;

	static void Main() // driver function
	{
		init(); // get user inputs
		rec(0,1); // print combinations
	}

	static void init() // initialization function
	{
		Console.WriteLine("Enter k: "); // prompt user
                k = Convert.ToInt32(Console.ReadLine()); // get input
                Console.WriteLine("Enter n: "); //prompt user
                n = Convert.ToInt32(Console.ReadLine()); // get input
				n = n + 1; // add 1 to n
                buf = new int[k]; // make buffer array
	}

	static void rec(int ind, int begin) //combination function
	{
		for (int i = begin; i < n; i++) 
		{
			buf [ind] = i; 
			if (ind + 1 < k) rec(ind+1, buf [ind] + 1); // get combination of length k
			else Console.WriteLine(string.Join(",", buf)); // write line
		}
	}
}
