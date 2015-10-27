#ifndef CONFIG3DSETTINGS_H
#define CONFIG3DSETTINGS_H

#pragma once
#include <Windows.h>
#include <GL/gl.h>

#include <string>
#include <fstream>
#include <algorithm>


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
		currentFunctionCallIndexPerFrame = 0;
	}
	void increaseFunctionCall()
	{
		currentFunctionCallIndexPerFrame ++;
	}
	static void switchBuffer(GLuint drawBuffer)
	{
		glDrawBuffer(drawBuffer);
	}
	void switchCurrentBuffer()
	{
		glDrawBuffer(currentDrawingBuffer);
		if (currentBoundaryLoc == ObjectBoundary::Start)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			currentBoundaryLoc = ObjectBoundary::Middle;
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
		case 1:
			return GL_BACK_LEFT;
		case 2:
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
};

#endif

