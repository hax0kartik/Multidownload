#include <string>
#include <3ds.h>
#include "qr.hpp"
#include "download.hpp"
#include "xplorer.h"

class utils : public qr{
    public:
       DownloadManager dm;
       xplorer xp;
       void InitKeyboard(int buttons = 2);
       SwkbdButton ShowKeyboard(std::string hintText);
       std::string GetKeyboardData();
       std::string ReadDownloadLocationFromConfig(std::string path = "/3ds/multidownload++.cfg");
       std::string WriteDownloadLocationToConfig(std::string buffer, std::string path = "/3ds/multidownload++.cfg");
    private:
        SwkbdState m_state;
        char m_swkdb_buffer[1024];
        std::string m_readfile;
};