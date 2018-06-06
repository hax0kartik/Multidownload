#include <string>
#include <3ds.h>

class utils{
    public:
       void InitKeyboard(int buttons = 2);
       SwkbdButton ShowKeyboard(std::string hintText);
       std::string GetKeyboardData();
    private:
        SwkbdState state;
        char swkdb_buffer[1024];
};