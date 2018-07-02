#include "utils.h"
#include "ui.h"
#include <string>
#include <fstream>

void utils::InitKeyboard(int buttons)
{
    svcCreateEvent(&ui.event, RESET_STICKY);
    swkbdInit(&this->m_state, SWKBD_TYPE_NORMAL, buttons, -1);
}

SwkbdButton utils::ShowKeyboard(std::string hintText)
{
    swkbdSetHintText(&this->m_state, hintText.c_str());
    swkbdSetValidation(&this->m_state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    SwkbdButton button = swkbdInputText(&this->m_state, this->m_swkdb_buffer, sizeof(this->m_swkdb_buffer));
    svcSignalEvent(ui.event);
    return button;
}

std::string utils::GetKeyboardData()
{
    return std::string(this->m_swkdb_buffer);
}

std::string utils::ReadDownloadLocationFromConfig(std::string path)
{
    std::fstream cfg(path, std::fstream::in);
    this->m_readfile = "/";
    
    if(cfg.is_open())
        cfg >> this->m_readfile;
    else 
        WriteDownloadLocationToConfig("/");
    cfg.close();
    return this->m_readfile;
}

std::string utils::WriteDownloadLocationToConfig(std::string buffer, std::string path)
{
    std::fstream cfg;
    cfg.open(path, std::fstream::out);
    if(cfg.is_open()) cfg << buffer;
    cfg.close();
}