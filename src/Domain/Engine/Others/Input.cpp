#include "Input.h"

#include "Canvas.h"

std::map<Input::Key, Input::ButtonInfo> Input::keyInfos;

float Input::lastMouseWheelDelta = 0.0f;
glm::vec2 Input::mouseCoords = glm::vec2(0.0f);
glm::vec2 Input::lastMouseCoords = glm::vec2(0.0f);
std::map<Input::MouseButton, Input::ButtonInfo> Input::mouseInfo;

Input::Input() {} //private, never called

void Input::Init() //called from Canvas
{
}

void Input::OnNewFrame()
{
    for(auto it = keyInfos.begin(); it != keyInfos.end(); ++it)
    {
        ButtonInfo &kInfo = it->second;
        if(kInfo.up) //After a frame where it was Up
        {
            keyInfos.erase(it);
        }
        else if(kInfo.down)
        {
            kInfo.down = false; //Not down anymore, just pressed.
        }
    }

    for(auto it = mouseInfo.begin(); it != mouseInfo.end(); ++it)
    {
        ButtonInfo &mbInfo = it->second;
        if(mbInfo.up) //After a frame where it was Up
        {
            mouseInfo.erase(it);
        }
        else if(mbInfo.down)
        {
            mbInfo.down = false; //Not down anymore, just pressed.
        }
    }

    lastMouseWheelDelta = 0.0f;
    lastMouseCoords = mouseCoords;
}

void Input::HandleInputMousWheel(QWheelEvent *event)
{
    lastMouseWheelDelta = float(event->delta()) / (24.0f * 15.0f);
}

void Input::HandleInputMouseMove(QMouseEvent *event)
{
    mouseCoords.x = event->x();
    mouseCoords.y = event->y();
}

void Input::HandleInputMousePress(QMouseEvent *event)
{
    MouseButton mb = static_cast<MouseButton>(event->button());
    if(mouseInfo.find(mb) == mouseInfo.end())
    {   //Only if it was not down/pressed before
        mouseInfo[mb] = ButtonInfo(false, true, true);
    }
}

void Input::HandleInputMouseRelease(QMouseEvent *event)
{
    MouseButton mb = static_cast<MouseButton>(event->button());
    if(mouseInfo.find(mb) != mouseInfo.end() &&
       mouseInfo[mb].pressed)
    {   //Only if it was pressed before
        mouseInfo[mb] = ButtonInfo(true, false, false);
    }
}


void Input::HandleInputKeyPress(QKeyEvent *event)
{
    //Only capture first press, not repeated ones
    if(event->isAutoRepeat()) return;

    Key k = static_cast<Key>(event->key());
    if(keyInfos.find(k) == keyInfos.end())
    {   //Only if it was not down/pressed before
        keyInfos[k] = ButtonInfo(false, true, true);
    }
}

void Input::HandleInputKeyReleased(QKeyEvent *event)
{
    //Only capture actual release, not repeated ones
    if(event->isAutoRepeat()) return;

    Key k = static_cast<Key>(event->key());
    if(keyInfos.find(k) != keyInfos.end() &&
       keyInfos[k].pressed)
    {   //Only if it was pressed before
        keyInfos[k] = ButtonInfo(true, false, false);
    }
}


bool Input::GetKey(Input::Key k)
{
    return keyInfos.find(k) != keyInfos.end() &&
           keyInfos[k].pressed;
}

bool Input::GetKeyUp(Input::Key k)
{
    return keyInfos.find(k) != keyInfos.end() &&
           keyInfos[k].up;
}

bool Input::GetKeyDown(Input::Key k)
{
    return keyInfos.find(k) != keyInfos.end() &&
            keyInfos[k].down;
}

float Input::GetMouseWheel()
{
    return lastMouseWheelDelta;
}


bool Input::GetMouseButton(Input::MouseButton mb)
{
    return mouseInfo.find(mb) != mouseInfo.end() &&
           mouseInfo[mb].pressed;
}

bool Input::GetMouseButtonUp(Input::MouseButton mb)
{
    return mouseInfo.find(mb) != mouseInfo.end() &&
           mouseInfo[mb].up;
}

bool Input::GetMouseButtonDown(Input::MouseButton mb)
{
    return mouseInfo.find(mb) != mouseInfo.end() &&
            mouseInfo[mb].down;
}

float Input::GetMouseAxisX()
{
    return GetMouseDeltaX() / Canvas::GetWidth();
}

float Input::GetMouseAxisY()
{
    return GetMouseDeltaY() / Canvas::GetHeight();
}

float Input::GetMouseDeltaX()
{
    return mouseCoords.x - lastMouseCoords.x;
}

float Input::GetMouseDeltaY()
{
    return mouseCoords.y - lastMouseCoords.y;
}

glm::vec2 Input::GetMouseCoords()
{
    return mouseCoords;
}
