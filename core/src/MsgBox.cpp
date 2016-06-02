#include <core/MsgBox.h>
#include <nowide/convert.hpp>

#include <Windows.h>


int MsgBox::Show(void* parent, const std::string& text, unsigned int style)
{
	return Show(parent, text, std::string(), style);
}

int MsgBox::Show(void* parent, const std::string& text, const std::string& title, unsigned int style)
{
    auto nativeParent = static_cast<HWND>(parent);
    style |= parent ? MB_APPLMODAL : MB_TASKMODAL;

    return ::MessageBoxW(nativeParent, nowide::widen(text).c_str(), nowide::widen(title).c_str(), style);
}
