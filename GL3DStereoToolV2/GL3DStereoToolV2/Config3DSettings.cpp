#include "Config3DSettings.h"

#include <sstream>
#include <iostream>

Config3DSettings::Config3DSettings()
{
}


Config3DSettings::~Config3DSettings()
{

}
bool Config3DSettings::readConfig3DSettingsFromFile(const std::string & fileName)
{
	// open the file
	ifstream input(fileName.c_str());
	if (!input) return false;

	struct {
		// convert string to boolean
		bool readBool(std::string text) {
			// convert to lower case
			std::transform(text.begin(), text.end(), text.begin(), ::tolower);

			// accept sensible values for true (anything else is false)
			return (text == "yes") || (text == "true") || (text == "1");
		}
	} local;

	// read key
	string key;

	do {
		string line;
		//input >> ws >> line;
		input >> line;

		if (!input) break;

		// identify the key
		if (line == "#DrawingBuffer:")
		{
			key = "#DrawingBuffer:";
			line = "";
		}
		else if (line == "#Background:")
		{
			key = "#Background:";
			line = "";
		}
		else if (line == "#Foregound:")
		{
			key = "#Foregound:";
			line = "";
		}
		else {
			// unrecognised keys are currently ignored
		}

		// look for values for the current key
		if (key == "#DrawingBuffer:" && line.length() > 0)
		{
			cout << "DrawingBuffer: " << endl;

			//for each value, parse it into 3 parts: key, value and comment (if has). The comment is specified by the ; and key by the :
			if (line.length() > 0)
			{
				size_t pos = 0;
				std::string token;
				pos = line.find(";"); //find comments

				cout << "Line: " << line << endl;
				cout << "Pos 1: " << pos << endl;
				if (pos != std::string::npos)
				{
					line.erase(pos, line.length() - pos);
				}
				
				pos = line.find(":");

				if (pos != std::string::npos)
				{
					string subKey = line.substr(0, pos);
					string value = line.substr(pos + 1, line.length() - pos - 1);
					
					cout << "subKey: " << subKey << endl;
					cout << "value: " << value << endl;
					
					if (subKey == "Buffer")
					{
						this->bufferIndex = convertStringToInt(value);
					}
				}
			}
		}
		else if (key == "#Background:" && line.length() > 0)
		{
			cout << "Background: " << endl;

			if (line.length() > 0)
			{
				size_t pos = 0;
				std::string token;
				pos = line.find(";"); //find comments

				cout << "Line: " << line << endl;

				if (pos != std::string::npos)
				{
					line.erase(pos, line.length() - pos);
				}

				pos = line.find(":");

				if (pos != std::string::npos)
				{
					string subKey = line.substr(0, pos);
					string value = line.substr(pos + 1, line.length() - pos - 1);

					cout << "subKey: " << subKey << endl;
					cout << "value: " << value << endl;

					if (subKey == "StartFunctionCallBackground")
					{
						this->StartFunctionCallBackground = value;
					}
					else if (subKey == "StartFunctionCallBackgroundNumber")
					{
						this->StartFunctionCallBackgroundNumber = convertStringToLong(value);
					}
					else if (subKey == "EndFunctionCallBackground")
					{
						this->EndFunctionCallBackground = value;
					}
					else if (subKey == "EndFunctionCallBackgroundNumber")
					{
						this->EndFunctionCallBackgroundNumber = convertStringToLong(value);
					}
				}
			}
		}
		else if (key == "#Foregound:" && line.length() > 0)
		{
			cout << "Foregound: " << endl;

			if (line.length() > 0)
			{
				size_t pos = 0;
				std::string token;
				pos = line.find(";"); //find comments

				cout << "Line: " << line << endl;

				if (pos != std::string::npos)
				{
					line.erase(pos, line.length() - pos);
				}

				pos = line.find(":");

				if (pos != std::string::npos)
				{
					string subKey = line.substr(0, pos);
					string value = line.substr(pos + 1, line.length() - pos - 1);

					cout << "subKey: " << subKey << endl;
					cout << "value: " << value << endl;

					if (subKey == "StartFunctionCallForegound")
					{
						this->StartFunctionCallForegound = value;
					}
					else if (subKey == "StartFunctionCallForegoundNumber")
					{
						this->StartFunctionCallForegoundNumber = convertStringToLong(value);
					}
					else if (subKey == "EndFunctionCallForegound")
					{
						this->EndFunctionCallForegound = value;
					}
					else if (subKey == "EndFunctionCallForegoundNumber")
					{
						this->EndFunctionCallForegoundNumber = convertStringToLong(value);
					}
				}
			}
		}

	} while (true);

	// close the file
	input.close();

	return true;
}
int Config3DSettings::convertStringToInt(string s)
{
	int number;
	istringstream buffer(s);
	buffer >> number;

	return number;
}
long Config3DSettings::convertStringToLong(string s)
{
	long number;
	istringstream buffer(s);
	buffer >> number;

	return number;
}