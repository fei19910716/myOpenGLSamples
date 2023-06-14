
#pragma once

#include "keys.h"

class ICallbacks
{
public:

    virtual void KeyboardCB(KEY Key, KEY_STATE KeyState = KEY_STATE_PRESS) {};
    
    virtual void MousePassiveCB(double x, double y) {};

    virtual void RenderSceneCB() {};

    virtual void MousePressCB(MOUSE Button, KEY_STATE State, double x, double y) {};

    virtual void FramebufferSizeCB(int width, int height){};

    virtual void ScrollCallback(double x, double y) {};
};
