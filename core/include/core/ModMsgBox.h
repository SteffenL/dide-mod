#pragma once

#include <string>

class ModMsgBox
{
public:
	static int Show(void* parent, const std::string& text, unsigned int style = 0);
	static int Show(void* parent, const std::string& text, const std::string& title, unsigned int style = 0);
};
