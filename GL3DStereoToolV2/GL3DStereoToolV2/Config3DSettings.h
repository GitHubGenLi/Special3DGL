#pragma once
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

class Config3DSettings
{
public:
	int bufferIndex; //0: both 1: left 2:right
	//background
	string StartFunctionCallBackground;
	long StartFunctionCallBackgroundNumber;
	string EndFunctionCallBackground;
	long EndFunctionCallBackgroundNumber;

	//forground
	string StartFunctionCallForegound;
	long StartFunctionCallForegoundNumber;
	string EndFunctionCallForegound;
	long EndFunctionCallForegoundNumber;

public:
	Config3DSettings();
	~Config3DSettings();
	bool readConfig3DSettingsFromFile(const std::string & fileName = "config3D.dat");

	static int convertStringToInt(string s);
	static long convertStringToLong(string s);
};

