#include <string>
#include <3ds.h>
#include "qr.hpp"

class utils : public qr{
    public:
       void InitKeyboard(int buttons = 2);
       SwkbdButton ShowKeyboard(std::string hintText);
       std::string GetKeyboardData();
    private:
        SwkbdState state;
        char swkdb_buffer[1024];
};