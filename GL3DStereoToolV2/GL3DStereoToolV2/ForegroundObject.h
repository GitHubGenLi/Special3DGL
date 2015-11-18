#ifndef FOREGROUNDOBJECT_H
#define FOREGROUNDOBJECT_H

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <deque>

using namespace std;

#include "Utility.h"
#include "Log.h"

using namespace hive;

class ForegroundObject
{
public:
	int BufferIndex;
	vector<string> BeginForeground;
	vector<string> EndForeground;
	int MaxNumberFunction;
public:
	ForegroundObject();
	ForegroundObject(int buffer);
	~ForegroundObject();
	void addFunction(string func, bool begin = true);
	void addFunctions(string funcs, bool begin = true);
	bool checkBoundaryObject(const deque<string> & listFunc, int &buffer, ObjectBoundary &bound)
	{
		//std::cout << "checkBoundaryObject! " << endl;

		bool inSide = false;
		
		//check beginning of the object
		if (BeginForeground.size() != listFunc.size())
		{
			inSide = false;
		}
		else //equa size, then check each element
		{
			inSide = true;

			for (int index = 0; index < BeginForeground.size(); index++)
			{
				if (BeginForeground[index].compare(listFunc[index]) != 0)
				{
					inSide = false;
					break;
				}
			}
			if (inSide)
			{
				bound = ObjectBoundary::Start;
				buffer = BufferIndex;

				Log::out() << "checkBoundaryObject: Start of the object." << endl;
			}
		}
		Log::out() << "checkBoundaryObject: checking end of the object." << endl;
		outputBoundaryObject();
		//check the end of the object
		if (!inSide)
		{
			if (EndForeground.size() != listFunc.size())
			{
				inSide = false;
			}
			else //equa size, then check each element
			{
				inSide = true;

				for (int index = 0; index < EndForeground.size(); index++)
				{
					if (EndForeground[index].compare(listFunc[index]) != 0)
					{
						inSide = false;
						break;
					}
				}
			}
			if (inSide)
			{
				bound = ObjectBoundary::End;
				buffer = BufferIndex; //Note: not used if end of boundary, just return for info

				Log::out() << "checkBoundaryObject: End of the object." << endl;
			}
		}

		return inSide;
	}
	void outputBoundaryObject()
	{
		std::stringstream result;
		std::copy(BeginForeground.begin(), BeginForeground.end(), std::ostream_iterator<string>(result, ","));
		Log::out() << "Begin object: " << result.str() << endl;

		result = stringstream();
		std::copy(EndForeground.begin(), EndForeground.end(), std::ostream_iterator<string>(result, ","));
		Log::out() << "End object: " << result.str() << endl;
	}
};

#endif