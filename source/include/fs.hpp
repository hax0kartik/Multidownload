#include <string>
#include <cstring>
#include <cstdio>
#include <fstream>
using namespace std;
class fs{
	public:
	void openfile(string location);
	void writefile(const char *buffer, size_t size);
	void writefile(string location, string buffer);
	void closefile();
	string readfile(string place);
	private:
	FILE *fp;
};
	