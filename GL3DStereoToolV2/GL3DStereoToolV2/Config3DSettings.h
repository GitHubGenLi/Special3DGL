#ifndef CONFIG3DSETTINGS_H
#define CONFIG3DSETTINGS_H

#pragma once
#include <Windows.h>
#include <GL/gl.h>

#include <string>
#include <fstream>
#include <algorithm>

#include <sstream>
#include <iostream>

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

public:
	Config3DSettings();
	~Config3DSettings();
	bool readConfig3DSettingsFromFile(const std::string & fileName = "config3D.dat");

	static int convertStringToInt(string s);
	static long convertStringToLong(string s);

	bool checkFunctionCall(const string funcName, ObjectType objType, ObjectBoundary bound);
	bool checkFunctionCallNumber(long indexCall, ObjectType objType, ObjectBoundary bound);

	void increaseFrameGL()
	{
		currentFrameGL++;
		std::cout << "Frame: " << currentFrameGL << endl;
		currentFunctionCallIndexPerFrame = 0;
	}
	void increaseFunctionCall(string funcName = "")
	{
		//if (increasedFunCall)
		{
			currentFunctionCallIndexPerFrame++;
		}
		
		if (funcName.length() > 0)
		{
			std::cout << "Current Func call: " << funcName << " Index :" << currentFunctionCallIndexPerFrame << endl;
		}
	}
	static void switchBuffer(GLuint drawBuffer)
	{
		glDrawBuffer(drawBuffer);
	}
	void switchCurrentBuffer()
	{
		if (currentBoundaryLoc != ObjectBoundary::End)
		{
			glDrawBuffer(currentDrawingBuffer);
			//std::cout << "Current buffer: " << currentDrawingBuffer << endl;
			if (currentBoundaryLoc == ObjectBoundary::Start)
			{
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
			}
		}
		
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
		default:
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
	void Config3DSettings::getDrawingBuffer(const string funcName);
	void resetCurrentStatusOfEachFrame();
};

#endif

