#pragma once
#include "DXWindow.h"
#include "DXTimer.h"

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