#ifndef FOREGROUNDOBJECT_H
#define FOREGROUNDOBJECT_H

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

#include "Utility.h"

class ForegroundObject
{
public:
	int BufferIndex;
	vector<string> BeginForeground;
	vector<string> EndForeground;

public:
	ForegroundObject();
	ForegroundObject(int buffer);
	~ForegroundObject();
	void addFunction(string func, bool begin = true);
	void addFunctions(string funcs, bool begin = true);
	
};

#endif