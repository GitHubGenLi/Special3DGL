#ifndef CONFIG3DSETTINGS_H
#define CONFIG3DSETTINGS_H

#pragma once
#include <Windows.h>
#include <GL/gl.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>

#include <sstream>
#include <iostream>
#include "Log.h"
#include "ForegroundObject.h"

#include <stdlib.h>
#include <time.h> 
#include <vector>
#include <deque>
#include <set>

#include "Utility.h"

//#include "opengl\OpenGLProc.h"


using namespace std;
using namespace hive;


class Config3DSettings
{
public:
	int BufferIndex; //0: both 1: left 2:right
	long StartInterceptedFrame = 0;

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

	//maintain the status
	GLuint currentDrawingBuffer;
	long currentFunctionCallIndexPerFrame;
	int currentFrameGL;
	ObjectBoundary currentBoundaryLoc;
	ObjectType currentObjectType;

	bool increasedFunCall = true;

	vector<ForegroundObject> ListForegroundObject;

	deque<string> ListRecentFunctionCalled;
	int TotalRecentNumberFunc;
	//int CurrentRecentNumberFunc;

	int totalClear;

	//typedef void (WINAPI * PFN_GLCLEAR)(GLbitfield mask);
	//PFN_GLCLEAR _glClear;

	bool saveAllInterceptedFuncs;
	std::ofstream interceptedWriter;
	set<string> ListInteceptedFunc;
	bool filterInterceptedFuncs = false;

public:
	Config3DSettings();
	~Config3DSettings();
	bool readConfig3DSettingsFromFile(const std::string & fileName = "config3D.dat");
	bool readConfig3DSettingsFromFileV2(const std::string & fileName = "config3D.dat");

	
	bool checkFunctionCall(const string funcName, ObjectType objType, ObjectBoundary bound);
	bool checkFunctionCallNumber(long indexCall, ObjectType objType, ObjectBoundary bound);

	void increaseFrameGL()
	{
		currentFrameGL++;
		//std::cout << "Frame: " << currentFrameGL << endl;
		currentFunctionCallIndexPerFrame = 0;
	}
	void increaseFunctionCall_old(string funcName = "")
	{
		if (increasedFunCall)
		{
			currentFunctionCallIndexPerFrame++;
		}
		
		if (funcName.length() > 0)
		{
			std::cout << "Current Func call: " << funcName << " Index :" << currentFunctionCallIndexPerFrame << endl;
			Log::print("Frame: " + Utility::convertIntToString(currentFrameGL) + " Current Func call: " + funcName + " Index :" + Utility::convertIntToString(currentFunctionCallIndexPerFrame) + "\n");
		}
	}
	void increaseFunctionCall(string funcName)
	{
		//std::cout << "Current Func call: " << funcName << " Index :" << currentFunctionCallIndexPerFrame << endl;

		if (increasedFunCall)
		{
			currentFunctionCallIndexPerFrame++;

			if (funcName == "glClear")
			{
				totalClear++;
				//std::cout << "Increase glClear count: " << totalClear << endl;
			}
			//std::cout << "Current Func call: " << funcName << endl;
			//make sure contain exactly the right numbers of functions
			if (ListRecentFunctionCalled.size() == TotalRecentNumberFunc)
			{
				ListRecentFunctionCalled.pop_front();
			}

			ListRecentFunctionCalled.push_back(funcName);

			if (saveAllInterceptedFuncs)
			{
				//
				ListInteceptedFunc.insert(funcName);
				//this->interceptedWriter << funcName << endl;
			}

			//debug
			//outputRecentListCall();
		}

		/*if (funcName.length() > 0)
		{
			std::cout << "Current Func call: " << funcName << " Index :" << currentFunctionCallIndexPerFrame << endl;
			Log::print("Frame: " + Utility::convertIntToString(currentFrameGL) + " Current Func call: " + funcName + " Index :" + Utility::convertIntToString(currentFunctionCallIndexPerFrame) + "\n");
		}*/
	}
	static void switchBuffer(GLuint drawBuffer)
	{
		glDrawBuffer(drawBuffer);
	}
	void switchCurrentBuffer_old()
	{
		if (currentBoundaryLoc != ObjectBoundary::End)
		{
			
			glDrawBuffer(currentDrawingBuffer);
			//std::cout << "Current buffer: " << currentDrawingBuffer << endl;
			if (currentBoundaryLoc == ObjectBoundary::Start)
			{
				// save OpenGL state
				//glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);

				increasedFunCall = false;

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glClearColor(0, 0, 0, 1);
				currentBoundaryLoc = ObjectBoundary::Middle;
				
				increasedFunCall = true;
			}
			else if (currentBoundaryLoc == ObjectBoundary::End)
			{
				currentDrawingBuffer = GL_BACK;
				currentBoundaryLoc = ObjectBoundary::Start;

				// restore OpenGL state
				//glPopAttrib();
			}
		
		}
		
	}
	void switchCurrentBuffer()
	{
		//if (currentBoundaryLoc != ObjectBoundary::End)
		//{

			glDrawBuffer(currentDrawingBuffer);
			//std::cout << "Current buffer: " << currentDrawingBuffer << endl;
			if (currentBoundaryLoc == ObjectBoundary::Start)
			{
				// save OpenGL state
				//glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
				//std::cout << "In boundary " << endl;

				if (totalClear == 1)
				{
					increasedFunCall = false;
					//std::cout << "1st glClear In boundary " << endl;

					//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					//glClear(GL_COLOR_BUFFER_BIT);
					//_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					//glClearColor(0, 0, 0, 0);
					//glClearColor(0, 0, 0, 1);
					//glClearDepth(1);
					//_glClearColor(0, 0, 0, 1);

					increasedFunCall = true;
				}
				currentBoundaryLoc = ObjectBoundary::Middle;
			}
			else if (currentBoundaryLoc == ObjectBoundary::End)
			{
				currentDrawingBuffer = GL_BACK;
				currentBoundaryLoc = ObjectBoundary::Start;

				// restore OpenGL state
				//glPopAttrib();
			}

		//}
			//std::cout << "glClear count: " << totalClear << endl;

	}
	void setCurrentBuffer(GLuint drawBuffer)
	{
		currentDrawingBuffer = drawBuffer;
	}
	GLuint getBuffer(int index)
	{
		switch (index)
		{
		case 0:
			return GL_BACK;
		case 2: //use this because opposite side
			return GL_BACK_LEFT;
		case 1:
			return GL_BACK_RIGHT;
		case 3:
		{
			int buffer = rand() % 3;
			return getBuffer(buffer);
		}
		default:
			return GL_BACK;
			break;
		}
	}
	GLuint getBuffer()
	{
		return getBuffer(BufferIndex);
	}
	bool startInterception()
	{
		return currentFrameGL >= StartInterceptedFrame;
	}
	void getDrawingBuffer(const string funcName, GLuint &buffer, ObjectType &objType, ObjectBoundary &bound, bool updated = true);
	void getDrawingBufferV2(const string funcName, GLuint &buffer, ObjectType &objType, ObjectBoundary &bound, bool updated = true);
	void getDrawingBuffer(const string funcName);
	void resetCurrentStatusOfEachFrame();
	void loadInterceptedFuncs(const std::string & fileName = "interceptedFunc.dat")
	{
		std::ifstream file(fileName, ifstream::in);
		
		if (!file)
		{	
			filterInterceptedFuncs = false;
			return;
		}

		string line;
		while (std::getline(file, line))
		{
			Utility::trimString(line);
			if (line.length() > 0)
			{
				ListInteceptedFunc.insert(line);
			}
		}

		file.close();

		if (ListInteceptedFunc.size() > 0)
		{
			addDefaultInterceptedFunc();
			filterInterceptedFuncs = true;
		}
		else
		{
			filterInterceptedFuncs = false;
		}

	}
	bool checkNeedInterception(const std::string & funcName)
	{
		bool need = true;
		std::cout << "Total Func call: " << ListInteceptedFunc.size() << endl;
		if (!filterInterceptedFuncs)
		{
			need = true;
			return need;
		}
		return (ListInteceptedFunc.find(funcName) != ListInteceptedFunc.end());
	}
	void addDefaultInterceptedFunc()
	{
		ListInteceptedFunc.insert("wglChoosePixelFormat");
		ListInteceptedFunc.insert("wglSetPixelFormat");
		ListInteceptedFunc.insert("wglChoosePixelFormatARB");
		ListInteceptedFunc.insert("glClear");
		ListInteceptedFunc.insert("wglSwapBuffers");
	}
	void outputRecentListCall()
	{
		Log::out() << "Current Frame: " << currentFrameGL << " with list of functions: ";

		std::stringstream result;
		std::copy(ListRecentFunctionCalled.begin(), ListRecentFunctionCalled.end(), std::ostream_iterator<string>(result, ","));

		Log::out() << result.str() << endl;
		
	}
};

#endif

