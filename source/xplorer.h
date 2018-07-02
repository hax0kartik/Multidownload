#include <3ds.h>
#include <string>
#include <vector>
#include <citro2d.h>
#include <citro3d.h>

class xplorer
{
    public:
    void open();
    std::string getSelectedLocation() { return m_dloc; }
    private:
    std::string m_dloc;
    std::string m_cur_directory = "/";
    std::vector<std::string> m_files;
    std::vector<C2D_Text> m_files_c2d;
    std::vector<int> m_old_selected;
    std::vector<int> m_old_scroll;
    C2D_Text m_text;
    int m_selected = 0;
    int m_scroll = 0;
};