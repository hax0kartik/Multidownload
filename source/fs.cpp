#include "fs.hpp"
using namespace std;
void fs::openfile(string location)
{
	fp = fopen(location.c_str(),"w+b");
}

void fs::writefile(const char *buffer, size_t size)
{
	fwrite(buffer,size,1,fp);	
}
void fs::writefile(string location, string buffer)
{
	fs::openfile(location);
	fs::writefile(buffer.c_str(), buffer.size());
	fs::closefile();
}
string fs::readfile(string place)
{
	ifstream input_file(place);
	if (!input_file.is_open()) 
	{ 
      input_file.close();// check for successful opening
      return "error";
	}
    string buffer;
    input_file >> buffer;
	input_file.close();
	return buffer;
}
void fs::closefile()
{
	fclose(fp);
}