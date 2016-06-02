#include <core/ModMsgBox.h>

#include <core/MsgBox.h>


#define MOD_SHORT_NAME "DIDE mod"
#define MOD_MSGBOX_TITLE_PREFIX "[" MOD_SHORT_NAME "] "

int ModMsgBox::Show(void* parent, const std::string& text, unsigned int style)
{
	return Show(parent, text, std::string(), style);
}

int ModMsgBox::Show(void* parent, const std::string& text, const std::string& title, unsigned int style)
{
	std::string titleMod(MOD_MSGBOX_TITLE_PREFIX);
	titleMod += title;
	return MsgBox::Show(parent, text, titleMod, style);
}
