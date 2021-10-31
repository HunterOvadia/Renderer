#include "DXException.h"
#include <sstream>

DXException::DXException(int Line, const char* File)
{
	this->Line = Line;
	this->File = File;
}

const char* DXException::what() const
{
	std::ostringstream StringStream;
	StringStream << GetType() << std::endl << GetOriginString();
	WhatBuffer = StringStream.str();
	return WhatBuffer.c_str();
}

const char* DXException::GetType() const
{
	return "DX Exception";
}

int DXException::GetLine() const
{
	return Line;
}

const std::string& DXException::GetFile() const
{
	return File;
}

std::string DXException::GetOriginString() const
{
	std::ostringstream StringStream;
	StringStream << "[File]: " << File << std::endl << "[Line]: " << Line;
	return StringStream.str();
}
