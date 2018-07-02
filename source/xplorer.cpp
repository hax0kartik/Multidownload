#include "xplorer.h"
#include <dirent.h>
#include <citro2d.h>
#include <filesystem>
#include <tuple>
#include <iostream>
#include "ui.h"
namespace fs = std::filesystem;

#define MAX_DIR_ELEMENTS_ON_SCREEN 18

extern C2D_TextBuf text_buf;
void stringToC2D(const char *string, C2D_Text *text)
{
    C2D_TextBufClear(text_buf);
    C2D_TextParse(text, text_buf, string);
    C2D_TextOptimize(text);
}

void drawText(std::tuple <float, float, float, float, float, C2D_Text*, uint32_t> *arg)
{
    using namespace std;
    auto args = *arg;   
    C2D_DrawText(get<5>(args), get<6>(args), get<0>(args), get<1>(args), get<2>(args), get<3>(args), get<4>(args), C2D_Color32f(1.0f, 1.0f, 1.0f, 1.0f));
}

void drawDirectory(std::tuple<C2D_Text*, int, int, int>*args)
{
    auto arg = *args;
    auto text = std::get<0>(arg);
    auto total_number_of_elements = std::get<1>(arg);
    auto scroll = std::get<2>(arg);
    auto selected = std::get<3>(arg);
    float y = 0.0f;
    int elements_to_draw = total_number_of_elements;
    int drawn = scroll * MAX_DIR_ELEMENTS_ON_SCREEN;
    elements_to_draw = total_number_of_elements - drawn;
    if(elements_to_draw > MAX_DIR_ELEMENTS_ON_SCREEN) elements_to_draw = MAX_DIR_ELEMENTS_ON_SCREEN;
    for(int i = 0; i < elements_to_draw; i++)
    {
        if(i + drawn == selected) C2D_DrawText(&text[i + drawn], C2D_WithColor, 1.0f, y, 0.5f, 0.5f, 0.5f, C2D_Color32(0xB3, 0xB3, 0xDA, 0xFF));
        else C2D_DrawText(&text[i + drawn], C2D_WithColor, 1.0f, y, 0.5f, 0.5f, 0.5f, C2D_Color32f(1.0f,1.0f,1.0f,1.0f));
        y += 13.0f;
    }
}

void prepareDirectoryForDraw(std::vector<std::string> file_str, std::vector<C2D_Text>& text)
{
    text.clear();
    for(auto i : file_str)
    {
        C2D_Text txt;
        C2D_TextParse(&txt, text_buf, i.c_str());
        C2D_TextOptimize(&txt);
        text.push_back(txt);
    }
    return;
}

int read_directory(const std::string &name, std::vector<std::string>& v)
{
    if(!fs::is_directory(fs::path(name))) return 0;
    v.clear();
    for (auto & p : fs::directory_iterator(name))
        v.push_back(p.path().filename().string());
    if(v.empty()) {v.push_back("EMPTY");}
    return 1;
}

void xplorer::open()
{
    std::string bottomText = "Press \uE000 to open the selected directory\n"
                             "Press \uE001 to go back to the previous directory\n"
                             "\uE079 and \uE07A to explore the contents\n"
                             "Press \uE002 to select the current directory as the\ndownload directory";
    
    stringToC2D(bottomText.c_str(), &this->m_text);
    auto bottom = std::make_tuple(8.0f, 8.0f, 0.0f, 0.52f, 0.52f, &this->m_text, C2D_WithColor);
    uiSetScreenBottom((func_t)drawText, &bottom);
   
    read_directory("/", this->m_files);
    prepareDirectoryForDraw(this->m_files, this->m_files_c2d);
    auto p = std::make_tuple(this->m_files_c2d, (int)this->m_files.size(), 0, 0);
    auto to_pass = &p;
    uiSetScreenTop((func_t)drawDirectory, to_pass);
    while(aptMainLoop())
    {
        hidScanInput();
        if(keysDown() & KEY_DDOWN)
        {   
            this->m_selected++;
            printf("Selected %d\n", this->m_selected);
            if(this->m_selected % MAX_DIR_ELEMENTS_ON_SCREEN == 0) this->m_scroll++;
            if(this->m_selected == m_files.size()) { this->m_selected = 0; this->m_scroll = 0; }
            std::cout << "Scroll :" << this->m_scroll << '\n';
            *to_pass = std::make_tuple(this->m_files_c2d, (int)this->m_files.size(), this->m_scroll, this->m_selected);
        }

        if(keysDown() & KEY_DUP)
        {
            this->m_selected--;
            printf("Selected %d \n", this->m_selected);
            if(this->m_selected % MAX_DIR_ELEMENTS_ON_SCREEN == MAX_DIR_ELEMENTS_ON_SCREEN - 1 && this->m_selected != 0) this->m_scroll--;
            if(this->m_selected < 0) { this->m_selected = m_files.size() - 1; this->m_scroll = m_files.size() / MAX_DIR_ELEMENTS_ON_SCREEN; }
            std::cout << "Scroll :" << this->m_scroll << '\n';
            *to_pass = std::make_tuple(this->m_files_c2d, (int)m_files.size(), this->m_scroll, this->m_selected);
        }

        if(keysDown() & KEY_A)
        {
            if(this->m_cur_directory != "/")
                this->m_cur_directory += '/' + this->m_files[this->m_selected]; 
            else
                this->m_cur_directory += this->m_files[this->m_selected];
            if(!read_directory(this->m_cur_directory, this->m_files)) 
            {   
                this->m_cur_directory = this->m_cur_directory.substr(0, this->m_cur_directory.find_last_of("/"));
                if(this->m_cur_directory.empty()) this->m_cur_directory = "/";
                continue;
            }
            this->m_old_selected.push_back(this->m_selected);
            this->m_old_scroll.push_back(this->m_scroll);
            prepareDirectoryForDraw(this->m_files, this->m_files_c2d);
            this->m_selected = 0;
            this->m_scroll = 0;
            *to_pass = std::make_tuple(this->m_files_c2d, (int)this->m_files.size(), this->m_scroll, this->m_selected);
        }

        if(keysDown() & KEY_B)
        {
            this->m_cur_directory = this->m_cur_directory.substr(0, this->m_cur_directory.find_last_of("/"));
            if(this->m_cur_directory.empty()){ this->m_cur_directory = "/";}
            read_directory(this->m_cur_directory, this->m_files);
            prepareDirectoryForDraw(this->m_files, this->m_files_c2d);
            if(!this->m_old_selected.empty())
            {
                this->m_selected = this->m_old_selected.back(); // Give us the last pos of the selector and then remove it from vector
                this->m_old_selected.pop_back();
                this->m_scroll = this->m_old_scroll.back();
                this->m_old_scroll.pop_back();
            }
            *to_pass = std::make_tuple(this->m_files_c2d, (int)this->m_files.size(), this->m_scroll, this->m_selected);
        }

        if(keysDown() & KEY_X)
        {
            break;
        }
    }
    //uiSetScreenBottom((func_t)drawDirectory, &p);
    this->m_files.clear();
    this->m_dloc = this->m_cur_directory;
}