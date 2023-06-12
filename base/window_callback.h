
#pragma once

#include "keys.h"

class ICallbacks
{
public:

    virtual void KeyboardCB(KEY Key, KEY_STATE KeyState = KEY_STATE_PRESS) {};
    
    virtual void PassiveMouseCB(int x, int y) {};

    virtual void RenderSceneCB() {};

    virtual void MouseCB(MOUSE Button, KEY_STATE State, int x, int y) {};

    virtual void FramebufferSizeCB(int width, int height){};
};
