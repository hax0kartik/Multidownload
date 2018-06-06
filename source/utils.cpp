#include "utils.h"
#include "ui.h"
#include <string>

void utils::InitKeyboard(int buttons)
{
    svcCreateEvent(&ui.event, RESET_STICKY);
    swkbdInit(&this->state, SWKBD_TYPE_NORMAL, buttons, -1);
}

SwkbdButton utils::ShowKeyboard(std::string hintText)
{
    swkbdSetHintText(&this->state, hintText.c_str());
    swkbdSetValidation(&this->state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    SwkbdButton button = swkbdInputText(&this->state, this->swkdb_buffer, sizeof(this->swkdb_buffer));
    svcSignalEvent(ui.event);
    return button;
}

std::string utils::GetKeyboardData()
{
    return std::string(this->swkdb_buffer);
}