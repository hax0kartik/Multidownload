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
    int i = 0;
    text.clear();
    for(auto it = file_str.begin(); it != file_str.end(); ++it)
    {
        C2D_Text txt;
        C2D_TextParse(&txt, text_buf, it->c_str());
        C2D_TextOptimize(&txt);
        text.push_back(txt);
        i++;
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

std::string open_xplorer()
{
    std::vector<std::string> files;
    std::string cur_directory = "/";

    std::string bottomText = "Press \uE000 to open the selected directory\n"
                             "Press \uE001 to go back to the previous directory\n"
                             "\uE079 and \uE07A to explore the contents\n"
                             "Press \uE002 to select the current directory as the\ndownload directory";
    C2D_Text text;
    stringToC2D(bottomText.c_str(), &text);
    auto bottom = std::make_tuple(8.0f, 8.0f, 0.0f, 0.52f, 0.52f, &text, C2D_WithColor);
    uiSetScreenBottom((func_t)drawText, &bottom);
   
    read_directory("/", files);
    std::vector <C2D_Text>files_c2d;
    prepareDirectoryForDraw(files, files_c2d);
    auto p = std::make_tuple(files_c2d, (int)files.size(), 0, 0);
    auto to_pass = &p;
    uiSetScreenTop((func_t)drawDirectory, to_pass); 
    int selected = 0;
    std::vector <int> old_selected;
    int scroll = 0;
    std::vector <int> old_scroll;
    while(aptMainLoop())
    {
        hidScanInput();
        if(keysDown() & KEY_DDOWN)
        {   
            selected++;
            printf("Selected %d\n", selected);
            if(selected % MAX_DIR_ELEMENTS_ON_SCREEN == 0) scroll++;
            if(selected == files.size()) { selected = 0; scroll = 0; }
            std::cout << "Scroll :" << scroll << '\n';
            *to_pass = std::make_tuple(files_c2d, (int)files.size(), scroll, selected);
        }

        if(keysDown() & KEY_DUP)
        {
            selected--;
            printf("Selected %d \n", selected);
            if(selected % MAX_DIR_ELEMENTS_ON_SCREEN == MAX_DIR_ELEMENTS_ON_SCREEN - 1 && selected != 0) scroll--;
            if(selected < 0) { selected = files.size() - 1; scroll = files.size() / MAX_DIR_ELEMENTS_ON_SCREEN; }
            std::cout << "Scroll :" << scroll << '\n';
            *to_pass = std::make_tuple(files_c2d, (int)files.size(), scroll, selected);
        }

        if(keysDown() & KEY_A)
        {
            if(cur_directory != "/")
                cur_directory += '/' + files[selected]; 
            else
                cur_directory += files[selected];
            if(!read_directory(cur_directory, files)) 
            {   
                cur_directory = cur_directory.substr(0, cur_directory.find_last_of("/"));
                if(cur_directory.empty()) cur_directory = "/";
                continue;
            }
            old_selected.push_back(selected);
            old_scroll.push_back(scroll);
            prepareDirectoryForDraw(files, files_c2d);
            selected = 0;
            scroll = 0;
            *to_pass = std::make_tuple(files_c2d, (int)files.size(), scroll, selected);
        }

        if(keysDown() & KEY_B)
        {
            cur_directory = cur_directory.substr(0, cur_directory.find_last_of("/"));
            if(cur_directory.empty()) cur_directory = "/";
            read_directory(cur_directory, files);
            prepareDirectoryForDraw(files, files_c2d);
            selected = old_selected.back(); // Give us the last pos of the selector and then remove it from vector
            old_selected.pop_back();
            scroll = old_scroll.back();
            old_scroll.pop_back();
            *to_pass = std::make_tuple(files_c2d, (int)files.size(), scroll, selected);
        }

        if(keysDown() & KEY_X)
        {
            break;
        }
    }
    //uiSetScreenBottom((func_t)drawDirectory, &p);

    files.clear();
    return cur_directory;
}