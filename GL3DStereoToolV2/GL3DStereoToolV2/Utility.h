#pragma once

#include <stdlib.h>
#include <time.h> 
#include <vector>
#include <string>
#include <sstream>

using namespace std;

enum ObjectType
{
	Background,
	Foreground
};
enum ObjectBoundary
{
	Start,
	Middle,
	End
};

class Utility
{
public:
	//static int convertStringToInt(string s);
	//static long convertStringToLong(string s);
	static int convertStringToInt(string s)
	{
		int number;
		istringstream buffer(s);
		buffer >> number;

		return number;
	}
	static long convertStringToLong(string s)
	{
		long number;
		istringstream buffer(s);
		buffer >> number;

		return number;
	}

	static string convertIntToString(int num)
	{
		string Result;          // string which will contain the result
		ostringstream convert;   // stream used for the conversion

		convert << num;      // insert the textual representation of 'Number' in the characters in the stream
		Result = convert.str(); // set 'Result' to the contents of the stream

		return Result;
	}
	static void trimString(string& str)
	{
		string::size_type pos = str.find_last_not_of(' ');
		if (pos != string::npos)
		{
			str.erase(pos + 1);
			pos = str.find_first_not_of(' ');

			if (pos != string::npos)
				str.erase(0, pos);
		}
		else str.erase(str.begin(), str.end());
	}

	Utility();
	~Utility();
};

