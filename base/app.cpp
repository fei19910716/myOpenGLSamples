#include "app.h"
#include "base/utils.h"

#ifndef _WIN32
Markup sMarkup = { (char*)"Arial", 64, 1, 0, 0.0, 0.0,
                   {.1,1.0,1.0,.5}, {1,1,1,0},
                   0, {1,0,0,1}, 0, {1,0,0,1},
                   0, {0,0,0,1}, 0, {0,0,0,1} };
#endif

App::App()
#ifndef _WIN32
    : m_fontRenderer(sMarkup)
#endif
{
    m_fpsFrameCount = 0;
    m_fpsStartTime = 0;
    m_fps = 0;

    m_previousFrameTime = m_fpsStartTime = m_startTime = UTILS::GetCurrentTimeMillis(); // App开始运行的时间
}


/**
 * caculate the frame count per second(fps)
*/
void App::CalculateFPS()
{
    m_fpsFrameCount++;

    long long currentTime = UTILS::GetCurrentTimeMillis();

    if (currentTime - m_fpsStartTime >= 1000) { // FPS为1秒的帧数
        m_fpsStartTime = currentTime;
        m_fps = m_fpsFrameCount;
        m_fpsFrameCount = 0;
    }
}

void App::DisplayFPS()
{
    char text[32];
    ZERO_MEM(text);
    SNPRINTF(text, sizeof(text), "FPS: %d", m_fps);

#ifndef _WIN32
    m_fontRenderer.RenderText(10, 10, text);
#endif
}

/**
 * app total running time since start
*/
float App::GetRunningTime()
{
    float RunningTime = (float)((double)UTILS::GetCurrentTimeMillis() - (double)m_startTime) / 1000.0f;
    return RunningTime;
}

float App::GetStartTime(){
    float time = (float)((double)m_startTime) / 1000.0f;
    return time;
}

float App::GetFrameTime(){
    long long currentTime = UTILS::GetCurrentTimeMillis();
    float frameTime = (float)((double)currentTime - (double)m_previousFrameTime) / 1000.0f;
    m_previousFrameTime = currentTime;

    return frameTime;
}
