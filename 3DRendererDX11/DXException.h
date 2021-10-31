#pragma once
#include <exception>
#include <string>

class DXException : public std::exception
{
public:
	DXException(int Line, const char* File);
	const char* what() const override;
	virtual const char* GetType() const;
	int GetLine() const;
	const std::string& GetFile() const;
	std::string GetOriginString() const;

protected:
	mutable std::string WhatBuffer;

private:
	int Line;
	std::string File;
};

