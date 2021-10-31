#pragma once
#include "DXWindow.h"

class App
{
public:
	App();
	int PumpMessages();

private:
	void Update();

private:
	DXWindow Window;
};