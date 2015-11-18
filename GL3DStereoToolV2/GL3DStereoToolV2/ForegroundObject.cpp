#include "ForegroundObject.h"


ForegroundObject::ForegroundObject()
{
	BufferIndex = 0;
}
ForegroundObject::ForegroundObject(int buffer)
{
	BufferIndex = buffer;
}

ForegroundObject::~ForegroundObject()
{
}
void ForegroundObject::addFunction(string func, bool begin)
{
	if (begin)
	{
		BeginForeground.push_back(func);
	}
	else
	{
		EndForeground.push_back(func);
	}
}
void ForegroundObject::addFunctions(string funcs, bool begin)
{
	size_t pos = 0;
	std::string token;
	vector<string> list;

	while ((pos = funcs.find(",")) != std::string::npos) {
		token = funcs.substr(0, pos);
		
		Utility::trimString(token);

		list.push_back(token);
		std::cout << token << std::endl;
		funcs.erase(0, pos + 1);
	}
	Utility::trimString(funcs);
	list.push_back(funcs);
	std::cout << funcs << std::endl;

	if (begin)
	{
		BeginForeground.insert(BeginForeground.end(), list.begin(), list.end());
	}
	else
	{
		EndForeground.insert(EndForeground.end(), list.begin(), list.end());
	}

	MaxNumberFunction = std::max(BeginForeground.size(), EndForeground.size());
	//std::cout << "Max numbers: " << MaxNumberFunction  << std::endl;

}