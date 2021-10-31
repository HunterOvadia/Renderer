#include "DXKeyboard.h"

bool DXKeyboard::IsKeyPressed(unsigned char KeyCode) const
{
	return KeyStates[KeyCode];
}

DXKeyboard::KeyboardEvent DXKeyboard::ReadKey()
{
	if (KeyBuffer.size() > 0u)
	{
		DXKeyboard::KeyboardEvent Event = KeyBuffer.front();
		KeyBuffer.pop();
		return Event;
	}
	else
	{
		return DXKeyboard::KeyboardEvent();
	}
}

bool DXKeyboard::IsKeyEmpty() const
{
	return KeyBuffer.empty();
}

void DXKeyboard::FlushKey()
{
	KeyBuffer = std::queue<KeyboardEvent>();
}

char DXKeyboard::ReadChar()
{
	if (CharBuffer.size() > 0u)
	{
		unsigned char CharCode = CharBuffer.front();
		CharBuffer.pop();
		return CharCode;
	}
	else
	{
		return 0;
	}
}

bool DXKeyboard::IsCharEmpty() const
{
	return CharBuffer.empty();
}

void DXKeyboard::FlushChar()
{
	CharBuffer = std::queue<char>();
}

void DXKeyboard::Flush()
{
	FlushKey();
	FlushChar();
}

void DXKeyboard::EnableAutorepeat()
{
	AutorepeatEnabled = true;
}

void DXKeyboard::DisableAutorepeat()
{
	AutorepeatEnabled = false;
}

bool DXKeyboard::IsAutorepeatEnabled() const
{
	return AutorepeatEnabled;
}

void DXKeyboard::OnKeyPressed(unsigned char KeyCode)
{
	KeyStates[KeyCode] = true;
	KeyBuffer.push(KeyboardEvent(KeyboardEvent::EType::Press, KeyCode));
	TrimBuffer(KeyBuffer);
}

void DXKeyboard::OnKeyReleased(unsigned char KeyCode)
{
	KeyStates[KeyCode] = false;
	KeyBuffer.push(KeyboardEvent(KeyboardEvent::EType::Release, KeyCode));
	TrimBuffer(KeyBuffer);
}

void DXKeyboard::OnChar(char Character)
{
	CharBuffer.push(Character);
	TrimBuffer(CharBuffer);
}

void DXKeyboard::ClearState()
{
	KeyStates.reset();
}

template<typename T>
void DXKeyboard::TrimBuffer(std::queue<T>& Buffer)
{
	while (Buffer.size() > BufferSize)
	{
		Buffer.pop();
	}
}
