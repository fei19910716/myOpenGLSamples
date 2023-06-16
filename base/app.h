
#pragma once

#ifndef _WIN32
#include "freetypeGL.h"
#endif

#include "base/window_backend.h"

class App
{
protected:
	App();

	void CalculateFPS();

	void DisplayFPS();

	float GetRunningTime();
	float GetStartTime();
	float GetFrameTime();

protected:
#ifndef _WIN32
	FontRenderer m_fontRenderer;
#endif
private:
	long long 	m_fpsStartTime;
	int 		m_fpsFrameCount;

	long long 	m_startTime;
	long long 	m_previousFrameTime;
	int 		m_fps;
};