#include "DXMouse.h"

std::pair<int, int> DXMouse::GetPos() const
{
    return { X, Y };
}

int DXMouse::GetPosX() const
{
    return X;
}

int DXMouse::GetPosY() const
{
    return Y;
}

bool DXMouse::IsLeftPressed() const
{
    return bIsLeftPressed;
}

bool DXMouse::IsRightPressed() const
{
    return bIsRightPressed;
}

DXMouse::MouseEvent DXMouse::Read()
{
    if (Buffer.size() > 0u)
    {
        MouseEvent Event = Buffer.front();
        Buffer.pop();
        return Event;
    }
    else
    {
        return MouseEvent();
    }
}

void DXMouse::Flush()
{
    Buffer = std::queue<MouseEvent>();
}

void DXMouse::OnMouseMove(int X, int Y)
{
    this->X = X;
    this->Y = Y;

    Buffer.push(MouseEvent(MouseEvent::EType::Move, *this));
    TrimBuffer();
}

void DXMouse::OnLeftPressed(int X, int Y)
{
    bIsLeftPressed = true;
    Buffer.push(MouseEvent(MouseEvent::EType::LPress, *this));
    TrimBuffer();
}

void DXMouse::OnLeftReleased(int X, int Y)
{
    bIsLeftPressed = false;
    Buffer.push(MouseEvent(MouseEvent::EType::LRelease, *this));
    TrimBuffer();
}

void DXMouse::OnRightPressed(int X, int Y)
{
    bIsRightPressed = true;
    Buffer.push(MouseEvent(MouseEvent::EType::RPress, *this));
    TrimBuffer();
}

void DXMouse::OnRightReleased(int X, int Y)
{
    bIsRightPressed = false;
    Buffer.push(MouseEvent(MouseEvent::EType::RRelease, *this));
    TrimBuffer();
}

void DXMouse::OnWheelUp(int X, int Y)
{
    Buffer.push(MouseEvent(MouseEvent::EType::WheelUp, *this));
    TrimBuffer();
}

void DXMouse::OnWheelDown(int X, int Y)
{
    Buffer.push(MouseEvent(MouseEvent::EType::WheelDown, *this));
    TrimBuffer();
}

void DXMouse::TrimBuffer()
{
    while (Buffer.size() > BufferSize)
    {
        Buffer.pop();
    }
}
